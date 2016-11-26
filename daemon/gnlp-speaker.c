/* gnlp-speaker.c
 *
 * Copyright (C) 2016 Georges Basile Stavracas Neto <georges.stavracas@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#define G_LOG_DOMAIN "Speaker"

#include "gnlp-dbus-generated.h"
#include "gnlp-speaker.h"

#include <espeak-ng/espeak_ng.h>

#define MAX_ERROR_MESSAGE_LENGTH 1024

struct _GnlpSpeaker
{
  GObject             parent;

  GQueue             *read_queue;
  GCancellable       *cancellable;

  GnlpSettings       *settings;

  gboolean            running : 1;
};

typedef struct
{
  gchar                 *text;
  GDBusMethodInvocation *invocation;
} SpeakData;

G_DEFINE_TYPE (GnlpSpeaker, gnlp_speaker, G_TYPE_OBJECT)

enum
{
  PROP_0,
  PROP_SETTINGS,
  N_PROPS
};

enum
{
  TEXT_READ,
  LAST_SIGNAL
};

G_LOCK_DEFINE_STATIC (queue_lock);
G_LOCK_DEFINE_STATIC (running_lock);

static guint signals [LAST_SIGNAL] = { 0, };
static GParamSpec *properties [N_PROPS] = { NULL, };

/*
 * SpeakData utilities
 */

static SpeakData*
speak_data_new (const gchar           *text,
                GDBusMethodInvocation *invocation)
{
  SpeakData *data;

  data = g_new0 (SpeakData, 1);
  data->text = g_strdup (text);
  data->invocation = g_object_ref (invocation);

  return data;
}

static void
speak_data_free (SpeakData *data)
{
  g_clear_object (&data->invocation);
  g_free (data->text);
  g_free (data);
}


/*
 * Speak utilities
 */

#define PARSE_ESPEAK_ERROR(status) \
gchar buffer[MAX_ERROR_MESSAGE_LENGTH]; \
espeak_ng_GetStatusCodeMessage (status, buffer, MAX_ERROR_MESSAGE_LENGTH); \
g_critical ("Error speaking message: %s", buffer);

static void
initialize_espeak_ng (void)
{
  espeak_ng_ERROR_CONTEXT error_context;
  espeak_ng_STATUS status;
  const wchar_t pronounced_puctionation[] = {'"', 0};

  g_debug ("Initializing the speaker...");

  error_context = NULL;

  espeak_Initialize (AUDIO_OUTPUT_SYNCHRONOUS, 0, NULL, 0);
  status = espeak_ng_Initialize (&error_context);

  if (status != ENS_OK)
    {
      g_critical ("Error initializing eSpeak-NG");
      return;
    }

  espeak_ng_InitializeOutput (ENOUTPUT_MODE_SPEAK_AUDIO, 0, NULL);

  espeak_ng_SetParameter (espeakPUNCTUATION, espeakPUNCT_SOME, 0);
  espeak_ng_SetPunctuationList (pronounced_puctionation);
}

static void
terminate_espeak_ng (void)
{
  g_debug ("Shutting down speaker");

  espeak_ng_Terminate ();
}


/*
 * Auxiliary methods
 */

static void
set_running (GnlpSpeaker *self,
             gboolean     running)
{
  G_LOCK (running_lock);

  self->running = running;

  G_UNLOCK (running_lock);
}

static void
get_running (GnlpSpeaker *self,
             gboolean    *running)
{
  if (!running)
    return;

  G_LOCK (running_lock);

  *running = self->running;

  G_UNLOCK (running_lock);
}

/*
 * Process the queue of to-be-spoken texts
 */
static void
queue_process_finished_cb (GObject      *source,
                           GAsyncResult *result,
                           gpointer      user_data)
{
  GnlpSpeaker *self = GNLP_SPEAKER (source);

  g_debug ("Finished reading queue");

  set_running (self, FALSE);
}

static void
process_queue_in_thread (GTask        *task,
                         gpointer      source_object,
                         gpointer      task_data,
                         GCancellable *cancellable)
{
  GnlpSpeaker *self = source_object;

  while (!g_queue_is_empty (self->read_queue))
    {
      espeak_ng_STATUS status;
      SpeakData *data;
      guint uid;

      if (g_cancellable_is_cancelled (cancellable))
        break;

      G_LOCK (queue_lock);
      data = g_queue_pop_head (self->read_queue);
      G_UNLOCK (queue_lock);

      g_debug ("Reading text \"%s\"", data->text);

      status = espeak_ng_Synthesize (data->text,
                                     g_utf8_strlen (data->text, -1),
                                     0, POS_WORD, 0,
                                     espeakCHARS_UTF8 | espeakSSML | espeakENDPAUSE,
                                     &uid,
                                     NULL);

      if (status != ENS_OK)
        {
          PARSE_ESPEAK_ERROR (status);
          goto loop;
        }

      /* Wait until the speech synthesis is finished to procceed */
      status = espeak_ng_Synchronize();

      if (status != ENS_OK)
        {
          PARSE_ESPEAK_ERROR (status);
          goto loop;
        }

      g_signal_emit (self,
                     signals[TEXT_READ],
                     0,
                     data->invocation);
loop:
      g_clear_pointer (&data, speak_data_free);
    }
}

static void
start_process_queue (GnlpSpeaker *self)
{
  GTask *task;

  g_debug ("Starting to process speak queue");

  set_running (self, TRUE);

  task = g_task_new (self, self->cancellable, queue_process_finished_cb, self);
  g_task_run_in_thread (task, process_queue_in_thread);

  g_clear_object (&task);
}


/*
 * Overrides
 */

static void
gnlp_speaker_finalize (GObject *object)
{
  GnlpSpeaker *self = (GnlpSpeaker *)object;

  terminate_espeak_ng ();

  g_cancellable_cancel (self->cancellable);
  g_clear_object (&self->cancellable);
  g_queue_free_full (self->read_queue, (GDestroyNotify) speak_data_free);

  G_OBJECT_CLASS (gnlp_speaker_parent_class)->finalize (object);
}

static void
gnlp_speaker_get_property (GObject    *object,
                           guint       prop_id,
                           GValue     *value,
                           GParamSpec *pspec)
{
  GnlpSpeaker *self = GNLP_SPEAKER (object);

  switch (prop_id)
    {
    case PROP_SETTINGS:
      g_value_set_object (value, self->settings);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gnlp_speaker_set_property (GObject      *object,
                           guint         prop_id,
                           const GValue *value,
                           GParamSpec   *pspec)
{
  GnlpSpeaker *self = GNLP_SPEAKER (object);

  switch (prop_id)
    {
    case PROP_SETTINGS:
      if (g_set_object (&self->settings, g_value_get_object (value)))
        g_object_notify_by_pspec (object, properties[PROP_SETTINGS]);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gnlp_speaker_class_init (GnlpSpeakerClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gnlp_speaker_finalize;
  object_class->get_property = gnlp_speaker_get_property;
  object_class->set_property = gnlp_speaker_set_property;

  signals[TEXT_READ] = g_signal_new ("text-read",
                                     GNLP_TYPE_SPEAKER,
                                     G_SIGNAL_RUN_FIRST,
                                     0, NULL, NULL, NULL,
                                     G_TYPE_NONE,
                                     1,
                                     G_TYPE_DBUS_METHOD_INVOCATION);

  properties[PROP_SETTINGS] = g_param_spec_object ("settings",
                                                   "Settings",
                                                   "Settings",
                                                   GNLP_TYPE_SETTINGS,
                                                   G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
gnlp_speaker_init (GnlpSpeaker *self)
{
  self->read_queue = g_queue_new ();
  self->cancellable = g_cancellable_new ();

  initialize_espeak_ng ();
}

GnlpSpeaker *
gnlp_speaker_new (GnlpSettings *settings)
{
  return g_object_new (GNLP_TYPE_SPEAKER,
                       "settings", settings,
                       NULL);
}

void
gnlp_speaker_read (GnlpSpeaker           *self,
                   GDBusMethodInvocation *invocation,
                   const gchar           *text)
{
  SpeakData *data;
  gboolean running;

  g_return_if_fail (GNLP_IS_SPEAKER (self));

  data = speak_data_new (text, invocation);

  /* Thread-safely push the read to the queue */
  G_LOCK (queue_lock);
  g_queue_push_tail (self->read_queue, data);
  G_UNLOCK (queue_lock);

  get_running (self, &running);

  if (!running)
    start_process_queue (self);
}
