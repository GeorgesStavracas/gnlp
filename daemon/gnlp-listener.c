/* gnlp-listener.c
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

#define G_LOG_DOMAIN "Listener"

#include "gnlp.h"
#include "gnlp-enum-types.h"
#include "gnlp-input-parser.h"
#include "gnlp-listener.h"
#include "gnlp-voice-command-parser.h"

#include <gio/gio.h>
#include <julius/juliuslib.h>

enum
{
  COMMAND,
  N_PARSERS
};

struct _GnlpListener
{
  GObject             parent;

  gboolean            listening;

  GnlpListenerMode    mode;

  Jconf              *config;
  Recog              *recognizer;

  GnlpInputParser    *parsers[N_PARSERS];

  GnlpContext        *context;
};

G_DEFINE_TYPE (GnlpListener, gnlp_listener, G_TYPE_OBJECT)

enum
{
  PROP_0,
  PROP_LISTENING,
  PROP_MODE,
  PROP_CONTEXT,
  N_PROPS
};

enum
{
  COMMAND_RECEIVED,
  SPEECH_RECOGNIZED,
  LAST_SIGNAL
};

G_LOCK_DEFINE_STATIC (signal_lock);

static guint signals[LAST_SIGNAL] = { 0, };

typedef struct
{
  GnlpListener *self;
  gdouble       confidence;
  gchar        *recognized_speech;
} SignalData;

static inline void
set_dialog_state (GnlpListener *self,
                  GnlpState     state)
{
  gnlp_dialog_state_set_state (gnlp_context_get_dialog_state (self->context), state);
}

static void
free_signal_data (gpointer user_data)
{
  SignalData *data = user_data;

  g_clear_object (&data->self);
  g_free (data->recognized_speech);
  g_free (data);
}

static gboolean
emit_speech_recognized (gconstpointer data)
{
  const SignalData *signal_data = data;

  G_LOCK (signal_lock);

  g_debug ("Speech recognized: %s", signal_data->recognized_speech);

  g_signal_emit (signal_data->self,
                 signals[SPEECH_RECOGNIZED],
                 0,
                 signal_data->recognized_speech ? signal_data->recognized_speech : "",
                 signal_data->confidence);

  G_UNLOCK (signal_lock);

  return G_SOURCE_REMOVE;
}

static void
set_listening (GnlpListener *self,
               gboolean      listening)
{
  G_LOCK (signal_lock);

  if (self->listening != listening)
    {
      self->listening = listening;
      g_object_notify (G_OBJECT (self), "listening");
    }

  G_UNLOCK (signal_lock);
}

/*
 * Callback to be called when start waiting speech input.
 */
static void
status_recready (Recog *recog,
                 void  *user_data)
{
  GnlpListener *self = GNLP_LISTENER (user_data);

  if (recog->jconf->input.speech_input != SP_MIC)
    return;

  set_dialog_state (self, GNLP_STATE_WAITING);
  set_listening (self, FALSE);
}

/*
 * Callback to be called when speech input is triggered.
 */
static void
status_recstart (Recog *recog,
                 void  *user_data)
{
  GnlpListener *self = GNLP_LISTENER (user_data);

  if (recog->jconf->input.speech_input != SP_MIC)
    return;

  set_dialog_state (self, GNLP_STATE_LISTENING);
  set_listening (self, TRUE);
}

static const gchar*
error_message (gint status)
{
  /* outout message according to the status code */
  switch(status)
    {
    case J_RESULT_STATUS_REJECT_POWER:
      return "input rejected by power";

    case J_RESULT_STATUS_TERMINATE:
      return "input teminated by request";

    case J_RESULT_STATUS_ONLY_SILENCE:
      return "input rejected by decoder (silence input result)";

    case J_RESULT_STATUS_REJECT_GMM:
      return "input rejected by GMM";

    case J_RESULT_STATUS_REJECT_SHORT:
      return "input rejected by short input";

    case J_RESULT_STATUS_REJECT_LONG:
      return "input rejected by long input";

    case J_RESULT_STATUS_FAIL:
      return "search failed";

    default:
      return "unknown error";
    }
}

static inline void
parse_input (GnlpListener *self,
             GStrv         words,
             gdouble      *word_confidence,
             const gchar  *sentence,
             gdouble       total_confidence)
{
  SignalData *data;
  gboolean valid;

  valid = FALSE;

  switch (self->mode)
    {
    case GNLP_LISTENER_MODE_NONE:
    case GNLP_LISTENER_MODE_FREE_SPEECH:
    case GNLP_LISTENER_MODE_QUESTION:
      /* TODO: implement other listening modes */
      break;

    case GNLP_LISTENER_MODE_COMMAND:
      valid = gnlp_input_parser_parse (self->parsers[COMMAND],
                                       words,
                                       word_confidence,
                                       sentence,
                                       total_confidence);
      break;

    default:
      g_assert_not_reached ();
      break;
    }

  if (valid)
    return;

  /* Send a generic signal */
  data = g_new0 (SignalData, 1);
  data->self = g_object_ref (self);
  data->recognized_speech = g_strdup (sentence);
  data->confidence = total_confidence;

  g_idle_add_full (G_PRIORITY_DEFAULT_IDLE,
                   (GSourceFunc) emit_speech_recognized,
                   data,
                   free_signal_data);
}

/*
 * Callback to output final recognition result.
 * This function will be called just after recognition of an input ends
 */
static void
output_result (Recog *recog,
               void  *user_data)
{
  GnlpListener *self;
  RecogProcess *r;
  WORD_INFO *winfo;
  Sentence *s;
  WORD_ID *seq;
  gdouble confidence;
  int i;
  int seqnum;
  int n;

  self = user_data;
  confidence = 1;

  /* all recognition results are stored at each recognition process
     instance */
  for (r = recog->process_list; r != NULL; r = r->next)
    {
      /* skip the process if the process is not alive */
      if (!r->live)
        continue;

      /* check result status */
      if (r->result.status < 0)
        {
          g_debug ("Error parsing voice: %s", error_message (r->result.status));
          continue;
        }

      /* output results for all the obtained sentences */
      winfo = r->lm->winfo;

      for (n = 0; n < r->result.sentnum; n++)
        {
          GPtrArray *words;
          GString *sentence;
          GArray *wconfidence;

          s = &(r->result.sent[n]);
          seq = s->word;
          seqnum = s->word_num;

          /* output word sequence like Julius */
          sentence = g_string_new (NULL);
          words = g_ptr_array_new ();

          for (i = 1; i < seqnum - 1; i++)
            {
              g_ptr_array_add (words, winfo->woutput[seq[i]]);
              g_string_append (sentence, winfo->woutput[seq[i]]);

              if (i < seqnum - 2)
                g_string_append (sentence, " ");
            }

          g_ptr_array_add (words, NULL);

          g_debug ("Sentence: %s", sentence->str);

          /* confidence scores */
          g_debug ("Confidence:");
          wconfidence = g_array_new (FALSE, FALSE, sizeof(gdouble));

          for (i = 0; i < seqnum; i++)
            {
              g_debug ("\t%-10.10s  %5.3f", winfo->woutput[seq[i]], s->confidence[i]);
              confidence *= s->confidence[i];
              g_array_append_val (wconfidence, s->confidence[i]);
            }

          g_debug ("Total: %lf", confidence);

          parse_input (self,
                       (GStrv) words->pdata,
                       (gpointer) wconfidence->data,
                       sentence->str,
                       confidence);

          g_clear_pointer (&words, g_ptr_array_unref);
        }
    }
}

static void
run_julius (GTask       *task,
            const gchar *config)
{
  GnlpListener *self;
  int ret;

  self = g_task_get_source_object (task);

  /* Setup the main Julius config */
  self->config = j_config_load_string_new ((gchar*) config);

  if (!self->config)
    {
      g_critical ("Try '-setting' for built-in engine configuration.\n");
      return;
    }

  self->recognizer = j_create_instance_from_jconf (self->config);
  if (!self->recognizer)
    {
      g_critical ("Error in startup\n");
      return;
    }

  callback_add (self->recognizer, CALLBACK_EVENT_SPEECH_READY, status_recready, self);
  callback_add (self->recognizer, CALLBACK_EVENT_SPEECH_START, status_recstart, self);
  callback_add (self->recognizer, CALLBACK_RESULT, output_result, self);

  if (!j_adin_init (self->recognizer))
    return;

  j_recog_info (self->recognizer);

  /* Open the voice stream and start parsing */
  ret = j_open_stream (self->recognizer, NULL);

  switch (ret)
    {
    case -1:
      g_critical ("Error in input stream\n");
      goto out;

    case -2:
      g_critical ("Failed to begin input stream\n");
      goto out;

    default:
      break;
    }

  /*
   * Enter main loop to recognize the input stream
   * finish after whole input has been processed and input reaches end
   */
  ret = j_recognize_stream (self->recognizer);

  if (ret == -1)
    return;

out:
  j_close_stream (self->recognizer);
  j_recog_free (self->recognizer);
}

static gchar*
read_config_at_path (const gchar *path)
{
  GString *string;
  GError *error;
  GFile *file;
  gchar *aux, *file_conf;
  gsize i;

  struct {
    const gchar *arg;
    const gchar *param;
  } fields[] = {
      { "-h",     "acoustic_model" },
      { "-hlist", "tiedlist"       },
      { "-dfa",   "dfa"            },
      { "-v",     "grammar_dict"   }
  };

  error = NULL;
  string = g_string_new (NULL);

  g_debug ("Loading configuration from %s", path);

  for (i = 0; i < G_N_ELEMENTS (fields); i++)
    {
      aux = g_build_filename (path, fields[i].param, NULL);
      g_string_append_printf (string, "%s %s\n", fields[i].arg, aux);
      g_free (aux);
    }

  /* Append the file's config */
  aux = g_build_filename (path, "config", NULL);
  file = g_file_new_for_path (aux);

  g_file_load_contents (file, NULL, &file_conf, NULL, NULL, &error);

  if (error)
    {
      g_critical ("Error loading configuration: %s", error->message);
      g_clear_error (&error);
    }
  else
    {
      g_string_append (string, file_conf);
    }

  g_clear_object (&file);
  g_free (file_conf);
  g_free (aux);

  return g_string_free (string, FALSE);
}

static void
start_listener_sync (GTask        *task,
                     gpointer      source_object,
                     gpointer      task_data,
                     GCancellable *cancellable)
{
  GnlpListener *self;
  GnlpLanguage *language;
  gchar *config;

  self = GNLP_LISTENER (source_object);
  language = gnlp_context_get_language (self->context);

  if (!gnlp_language_has_quirk (language, GNLP_LANGUAGE_QUIRK_LISTEN))
    {
      g_debug ("The current language does not support listening.");
      return;
    }

  config = read_config_at_path (gnlp_language_get_path (language));

  /* Do not let the daemon die */
  g_application_hold (g_application_get_default ());

  /* Disable julius output */
  jlog_set_output (NULL);

  run_julius (task, config);

  g_free (config);
}

/*
 * Callbacks
 */
static void
language_changed_cb (GnlpContext *context,
                     GParamSpec   *pspec,
                     GnlpListener *self)
{
  /* TODO */
}

static void
gnlp_listener_finalize (GObject *object)
{
  G_OBJECT_CLASS (gnlp_listener_parent_class)->finalize (object);
}

static void
gnlp_listener_constructed (GObject *object)
{
  GnlpListener *self = GNLP_LISTENER (object);

#define PARSER(type) g_object_new (type, "context", self->context, NULL)

  self->parsers[COMMAND] = PARSER (GNLP_TYPE_VOICE_COMMAND_PARSER);

#undef PARSER

  G_OBJECT_CLASS (gnlp_listener_parent_class)->constructed (object);
}

static void
gnlp_listener_get_property (GObject    *object,
                            guint       prop_id,
                            GValue     *value,
                            GParamSpec *pspec)
{
  GnlpListener *self = GNLP_LISTENER (object);

  switch (prop_id)
    {
    case PROP_LISTENING:
      g_value_set_boolean (value, self->listening);
      break;

    case PROP_MODE:
      g_value_set_enum (value, self->mode);
      break;

    case PROP_CONTEXT:
      g_value_set_object (value, self->context);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gnlp_listener_set_property (GObject      *object,
                            guint         prop_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
  GnlpListener *self = GNLP_LISTENER (object);

  switch (prop_id)
    {
    case PROP_MODE:
      gnlp_listener_set_mode (self, g_value_get_enum (value));
      break;

    case PROP_CONTEXT:
      if (!g_set_object (&self->context, g_value_get_object (value)))
        break;

      g_signal_connect_swapped (self->context,
                                "notify::language",
                                G_CALLBACK (language_changed_cb),
                                self);
      g_object_notify (object, "context");
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gnlp_listener_class_init (GnlpListenerClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gnlp_listener_finalize;
  object_class->constructed = gnlp_listener_constructed;
  object_class->get_property = gnlp_listener_get_property;
  object_class->set_property = gnlp_listener_set_property;

  g_object_class_install_property (object_class,
                                   PROP_LISTENING,
                                   g_param_spec_boolean ("listening",
                                                         "Listening",
                                                         "Listening",
                                                         FALSE,
                                                         G_PARAM_READABLE));

  g_object_class_install_property (object_class,
                                   PROP_MODE,
                                   g_param_spec_enum ("mode",
                                                      "Mode",
                                                      "Mode",
                                                      GNLP_TYPE_LISTENER_MODE,
                                                      GNLP_LISTENER_MODE_NONE,
                                                      G_PARAM_READWRITE));

  g_object_class_install_property (object_class,
                                   PROP_CONTEXT,
                                   g_param_spec_object ("context",
                                                        "Context",
                                                        "Context",
                                                        GNLP_TYPE_CONTEXT,
                                                        G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

  signals[COMMAND_RECEIVED] = g_signal_new ("command-received",
                                            GNLP_TYPE_LISTENER,
                                            G_SIGNAL_RUN_FIRST,
                                            0, NULL, NULL, NULL,
                                            G_TYPE_NONE,
                                            4,
                                            G_TYPE_STRING,
                                            G_TYPE_STRING,
                                            G_TYPE_STRING,
                                            G_TYPE_DOUBLE);

  signals[SPEECH_RECOGNIZED] = g_signal_new ("speech-recognized",
                                             GNLP_TYPE_LISTENER,
                                             G_SIGNAL_RUN_FIRST,
                                             0, NULL, NULL, NULL,
                                             G_TYPE_NONE,
                                             2,
                                             G_TYPE_STRING,
                                             G_TYPE_DOUBLE);


}

static void
gnlp_listener_init (GnlpListener *self)
{
  /*
   * TODO: the initial mode should me NONE, and we must detect the input
   * type when that happens. Right now, however, the COMMAND mode is enough.
   */
  self->mode = GNLP_LISTENER_MODE_COMMAND;
}

GnlpListener*
gnlp_listener_new (GnlpContext *context)
{
  return g_object_new (GNLP_TYPE_LISTENER,
                       "context", context,
                       NULL);
}

void
gnlp_listener_run (GnlpListener *self)
{
  GTask *task;

  task = g_task_new (self, NULL, NULL, NULL);
  g_task_run_in_thread (task, start_listener_sync);

  g_object_unref (task);
}

GnlpListenerMode
gnlp_listener_get_mode (GnlpListener *self)
{
  g_return_val_if_fail (GNLP_IS_LISTENER (self), GNLP_LISTENER_MODE_NONE);

  return self->mode;
}

void
gnlp_listener_set_mode (GnlpListener     *self,
                        GnlpListenerMode  mode)
{
  g_return_if_fail (GNLP_IS_LISTENER (self));

  if (self->mode == mode)
    return;

  /* TODO: change Julius mode too */
  self->mode = mode;
  g_object_notify (G_OBJECT (self), "mode");
}
