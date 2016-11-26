/* gnlp-client.c
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

#include "gnlp-client.h"
#include "gnlp-dbus-generated.h"
#include "gnlp-language.h"
#include "gnome-languages.h"

#include <glib/gi18n.h>

#define GNLP_LISTENER_PATH "/org/gnome/Gnlp/Speech/Listener"
#define GNLP_SETTINGS_PATH "/org/gnome/Gnlp/Speech/Settings"
#define GNLP_SPEAKER_PATH  "/org/gnome/Gnlp/Speech/Speaker"

struct _GnlpClient
{
  GObject             parent;

  gchar              *module_name;

  /*< private >*/
  GnlpSpeechListener *listener;
  GnlpSpeechSettings *settings;
  GnlpSpeechSpeaker  *speaker;
  GDBusObjectManager *object_manager;
  gboolean            is_initialized : 1;
  GError             *initialization_error;
};

static void          gnlp_client_initable_iface_init            (GInitableIface *iface);

G_DEFINE_TYPE_WITH_CODE (GnlpClient, gnlp_client, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (G_TYPE_INITABLE, gnlp_client_initable_iface_init)
                         G_IMPLEMENT_INTERFACE (G_TYPE_ASYNC_INITABLE, NULL));

enum
{
  PROP_0,
  PROP_IS_LISTENING,
  PROP_MODULE_NAME,
  PROP_LANGUAGE,
  N_PROPS
};

enum
{
  COMMAND_RECEIVED,
  SPEECH_RECOGNIZED,
  LAST_SIGNAL
};

static guint signals [LAST_SIGNAL] = { 0, };
static GParamSpec *properties [N_PROPS] = { NULL, };

/*
 * Listener-related methods
 */
static void
on_command_received (GnlpSpeechListener *listener,
                     const gchar        *module,
                     const gchar        *command,
                     const gchar        *arg,
                     gdouble             confidence,
                     GnlpClient         *self)
{
  gboolean should_emit;

  should_emit = !self->module_name || g_strcmp0 (module, self->module_name) == 0;

  if (!should_emit)
    return;

  g_signal_emit (self, signals[COMMAND_RECEIVED], 0, module, command, arg, confidence);
}

static void
on_listening_state_changed (GnlpSpeechListener *listener,
                            GParamSpec         *pspec,
                            GObject            *self)
{
  g_object_notify_by_pspec (self, properties[PROP_IS_LISTENING]);
}

static void
on_speech_recognized (GnlpSpeechListener *listener,
                      const gchar        *recognized_speech,
                      gdouble             confidence,
                      GnlpClient        *self)
{
  g_signal_emit (self, signals[SPEECH_RECOGNIZED], 0, recognized_speech, confidence);
}

/*
 * GInitable iface implementation
 */
G_LOCK_DEFINE_STATIC (init_lock);

static gboolean
gnlp_client_initable_init (GInitable     *initable,
                           GCancellable  *cancellable,
                           GError       **error)
{
  GDBusObject *object;
  GnlpClient *self;
  gboolean valid;

  self = GNLP_CLIENT (initable);
  valid = FALSE;

  G_LOCK (init_lock);

  /*
   * If we already initialized the object, it MUST have the
   * object manager, otherwise it's invalid and should return
   * NULL.
   */
  if (self->is_initialized)
    {
      if (!self->object_manager || self->initialization_error)
        goto out;
    }

  self->object_manager = gnlp_object_manager_client_new_for_bus_sync (G_BUS_TYPE_SESSION,
                                                                      G_DBUS_OBJECT_MANAGER_CLIENT_FLAGS_NONE,
                                                                      "org.gnome.Gnlp",
                                                                      "/org/gnome/Gnlp",
                                                                      cancellable,
                                                                      &self->initialization_error);

  /*
   * If something happened and the object returned NULL, we simply
   * don't set valid to TRUE.
   */
  if (!self->object_manager || self->initialization_error)
    goto out;

  /* Setup the listener */
  object = g_dbus_object_manager_get_object (self->object_manager, GNLP_LISTENER_PATH);

  if (object)
    {
      self->listener = gnlp_object_peek_speech_listener (GNLP_OBJECT (object));

      g_signal_connect (self->listener,
                        "command-received",
                        G_CALLBACK (on_command_received),
                        self);

      g_signal_connect (self->listener,
                        "speech-recognized",
                        G_CALLBACK (on_speech_recognized),
                        self);

      g_signal_connect (self->listener,
                        "notify::listening",
                        G_CALLBACK (on_listening_state_changed),
                        self);
    }
  else
    {
      /*
       * Is the previous call returned NULL, certainly the
       * Gnlp daemon is not running.
       */
      g_set_error (&self->initialization_error,
                   G_DBUS_ERROR,
                   G_DBUS_ERROR_FAILED,
                   "%s", _("The voice recognition object is not available, check if Gnlp daemon is running"));

      goto out;
    }

  /* Settings */
  object = g_dbus_object_manager_get_object (self->object_manager, GNLP_SETTINGS_PATH);

  if (object)
    {
      valid = TRUE;

      self->settings = gnlp_object_peek_speech_settings (GNLP_OBJECT (object));

      g_object_bind_property (self->settings,
                              "language",
                              self,
                              "language",
                              G_BINDING_BIDIRECTIONAL | G_BINDING_SYNC_CREATE);
    }
  else
    {
      g_set_error (&self->initialization_error,
                   G_DBUS_ERROR,
                   G_DBUS_ERROR_FAILED,
                   "%s", _("The settings object is not available, check if Gnlp daemon is running"));

      goto out;
    }

  /* Speaker */
  object = g_dbus_object_manager_get_object (self->object_manager, GNLP_SPEAKER_PATH);

  if (object)
    {
      valid = TRUE;

      self->speaker = gnlp_object_peek_speech_speaker (GNLP_OBJECT (object));
    }
  else
    {
      g_set_error (&self->initialization_error,
                   G_DBUS_ERROR,
                   G_DBUS_ERROR_FAILED,
                   "%s", _("The speaker object is not available, check if Gnlp daemon is running"));

      goto out;
    }

out:
  self->is_initialized = TRUE;

  if (!valid)
    g_propagate_error (error, g_error_copy (self->initialization_error));

  G_UNLOCK (init_lock);

  return valid;
}

static void
gnlp_client_initable_iface_init (GInitableIface *iface)
{
  iface->init = gnlp_client_initable_init;
}

static void
gnlp_client_finalize (GObject *object)
{
  GnlpClient *self = (GnlpClient *)object;

  g_clear_pointer (&self->module_name, g_free);
  g_clear_object (&self->object_manager);

  G_OBJECT_CLASS (gnlp_client_parent_class)->finalize (object);
}

static void
gnlp_client_get_property (GObject    *object,
                          guint       prop_id,
                          GValue     *value,
                          GParamSpec *pspec)
{
  GnlpClient *self = GNLP_CLIENT (object);

  switch (prop_id)
    {
    case PROP_IS_LISTENING:
      g_value_set_boolean (value, gnlp_speech_listener_get_listening (self->listener));
      break;

    case PROP_LANGUAGE:
      g_value_set_string (value, gnlp_client_get_language (self));
      break;

    case PROP_MODULE_NAME:
      g_value_set_string (value, self->module_name);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gnlp_client_set_property (GObject      *object,
                          guint         prop_id,
                          const GValue *value,
                          GParamSpec   *pspec)
{
  GnlpClient *self = GNLP_CLIENT (object);

  switch (prop_id)
    {
    case PROP_LANGUAGE:
      gnlp_client_set_language (self, g_value_get_string (value));
      break;

    case PROP_MODULE_NAME:
      self->module_name = g_value_dup_string (value);
      g_object_notify_by_pspec (object, properties[PROP_MODULE_NAME]);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gnlp_client_class_init (GnlpClientClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gnlp_client_finalize;
  object_class->get_property = gnlp_client_get_property;
  object_class->set_property = gnlp_client_set_property;

  /**
   * GnlpClient:command-received:
   *
   * Emited when a voice command is received.
   *
   * Since: 0.1.0
   */
  signals[COMMAND_RECEIVED] = g_signal_new ("command-received",
                                            GNLP_TYPE_CLIENT,
                                            G_SIGNAL_RUN_FIRST,
                                            0, NULL, NULL, NULL,
                                            G_TYPE_NONE,
                                            4,
                                            G_TYPE_STRING,
                                            G_TYPE_STRING,
                                            G_TYPE_STRING,
                                            G_TYPE_DOUBLE);

  /**
   * GnlpClient:speech-recognized:
   *
   * Emited when a speech is recognized.
   *
   * Since: 0.1.0
   */
  signals[SPEECH_RECOGNIZED] = g_signal_new ("speech-recognized",
                                             GNLP_TYPE_CLIENT,
                                             G_SIGNAL_RUN_FIRST,
                                             0, NULL, NULL, NULL,
                                             G_TYPE_NONE,
                                             2,
                                             G_TYPE_STRING,
                                             G_TYPE_DOUBLE);

  /**
   * GnlpClient:is-listening:
   *
   * Whether a voice command is being spoken or not.
   *
   * Since: 0.1.0
   */
  properties[PROP_IS_LISTENING] = g_param_spec_boolean ("is-listening",
                                                        "Is listening",
                                                        "Whether it's listening a voice command or not",
                                                        FALSE,
                                                        G_PARAM_READABLE);

  /**
   * GnlpClient:module-name:
   *
   * The current module name.
   *
   * Since: 0.1.0
   */
  properties[PROP_MODULE_NAME] = g_param_spec_string ("module-name",
                                                      "Module name",
                                                      "The module name",
                                                      NULL,
                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

  /**
   * GnlpClient:language:
   *
   * The current language.
   *
   * Since: 0.1.0
   */
  properties[PROP_LANGUAGE] = g_param_spec_string ("language",
                                                   "Language",
                                                   "The current language",
                                                   NULL,
                                                   G_PARAM_READWRITE);

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
gnlp_client_init (GnlpClient *self)
{
}

/**
 * gnlp_client_new:
 * @module: (nullable): the module name
 * @cancellable: (nullable): optional #GCancellable object
 * @callback: (scope async): a #GAsyncReadyCallback to call when the initialization is done
 * @user_data: (closure): data to pass to the callback function
 *
 * Asynchronously creates a new #GnlpClient. When it finishes, @callback will
 * be called.
 *
 * Since: 0.1.0
 */
void
gnlp_client_new (const gchar         *module,
                 GCancellable        *cancellable,
                 GAsyncReadyCallback  callback,
                 gpointer             user_data)
{
  g_async_initable_new_async (GNLP_TYPE_CLIENT,
                              G_PRIORITY_DEFAULT,
                              cancellable,
                              callback,
                              user_data,
                              "module-name", module,
                              NULL);
}

/**
 * gnlp_client_new_finish:
 * @result: a #GAsyncResult
 * @error: (nullable): return location for a #GError
 *
 * Finishes an asynchronous initialization started in gnlp_client_new(). If
 * an error occurs, the functions sets @error and returns %NULL.
 *
 * Returns: (transfer full) (nullable): a #GnlpClient
 *
 * Since: 0.1.0
 */
GnlpClient*
gnlp_client_new_finish (GAsyncResult  *result,
                        GError       **error)
{
  GObject *client;
  GObject *source;

  source = g_async_result_get_source_object (result);
  client = g_async_initable_new_finish (G_ASYNC_INITABLE (source), result, error);

  g_clear_object (&source);

  return client ? GNLP_CLIENT (client) : NULL;
}

/**
 * gnlp_client_new_sync:
 * @module: (nullable): the module name for this client
 * @cancellable: (nullable): optional #GCancellable object
 * @error: (nullable): return location for a #GError
 *
 * Initializes a #GnlpClient exactly like the asynchronous gnlp_client_new(),
 * but in a synchronous way. This version will block the running thread.
 *
 * Returns: (transfer full) (nullable): a #GnlpClient
 *
 * Since: 0.1.0
 */
GnlpClient*
gnlp_client_new_sync (const gchar   *module,
                      GCancellable  *cancellable,
                      GError       **error)
{
  GInitable *client;

  client = g_initable_new (GNLP_TYPE_CLIENT,
                           cancellable,
                           error,
                           "module-name", module,
                           NULL);

  return client ? GNLP_CLIENT (client) : NULL;
}

/**
 * gnlp_client_is_listening:
 * @self: a #GnlpClient
 *
 * Retrieves whether a voice command is being spoken or not.
 *
 * Returns: %TRUE if a voice command is being passed, %FALSE otherwise.
 *
 * Since: 0.1.0
 */
gboolean
gnlp_client_is_listening (GnlpClient *self)
{
  g_return_val_if_fail (GNLP_IS_CLIENT (self), FALSE);

  return gnlp_speech_listener_get_listening (self->listener);
}

/**
 * gnlp_client_get_language:
 * @self: a #GnlpClient
 *
 * Retrieves the current language that Gnlp is running against.
 *
 * Returns: (transfer none)(nullable): the current language
 *
 * Since: 0.1.0
 */
const gchar*
gnlp_client_get_language (GnlpClient *self)
{
  g_return_val_if_fail (GNLP_IS_CLIENT (self), NULL);

  return gnlp_speech_settings_get_language (self->settings);
}

/**
 * gnlp_client_set_language:
 * @self: a #GnlpClient
 * @language: the language which Gnlp will run against.
 *
 * Sets the language that Gnlp will run agains. Notice that
 * it depends on the availability of language data to work.
 *
 * Since: 0.1.0
 */
void
gnlp_client_set_language (GnlpClient  *self,
                          const gchar *language)
{
  g_return_if_fail (GNLP_IS_CLIENT (self));

  gnlp_speech_settings_set_language (self->settings, language);
}

/**
 * gnlp_client_read_text:
 * @self: a #GnlpClient
 * @message_id: (nullable): the unique identifier of the message
 * @text: the text to be read by Gnlp
 * @cancellable: (nullable): a #GCancellable
 * @callback: (scope async): a #GAsyncReadyCallback to call when the initialization is done
 * @user_data: (closure): data to pass to the callback function
 *
 * Reads @text.
 *
 * Since: 0.1.0
 */
void
gnlp_client_read_text (GnlpClient          *self,
                       const gchar         *text,
                       GCancellable        *cancellable,
                       GAsyncReadyCallback  callback,
                       gpointer             user_data)
{
  g_return_if_fail (GNLP_IS_CLIENT (self));

  gnlp_speech_speaker_call_read_text (self->speaker,
                                      text,
                                      cancellable,
                                      callback,
                                      user_data);
}

/**
 * gnlp_client_read_text_finished:
 * @self: a #GnlpClient
 * @result: a #GAsyncResult
 * @error: (nullable): return location for a #GError, or %NULL
 *
 * Finishes the text speaking operation started by @gnlp_client_read_text.
 *
 * Returns: %TRUE if the operation was successfull, %FALSE otherwise.
 *
 * Since: 0.1.0
 */
gboolean
gnlp_client_read_text_finished (GnlpClient    *self,
                                GAsyncResult  *result,
                                GError       **error)
{
  g_return_val_if_fail (GNLP_IS_CLIENT (self), FALSE);

  return gnlp_speech_speaker_call_read_text_finish (self->speaker, result, error);
}

/**
 * gnlp_client_read_text_sync:
 * @self: a #GnlpClient
 * @text: the text to be read by Gnlp
 * @cancellable: (nullable): a #GCancellable, or %NULL
 * @error: (nullable): return location for a #GError
 *
 * Synchronously reads @text. This will block the mainloop until it's
 * finished. See @gnlp_client_read_text for the non-blocking version
 * of this call.
 *
 * Returns: %TRUE if the operation was successfull, %FALSE otherwise
 *
 * Since: 0.1.0
 */
gboolean
gnlp_client_read_text_sync (GnlpClient    *self,
                            const gchar   *text,
                            GCancellable  *cancellable,
                            GError       **error)
{
  g_return_val_if_fail (GNLP_IS_CLIENT (self), FALSE);

  return gnlp_speech_speaker_call_read_text_sync (self->speaker,
                                                  text,
                                                  cancellable,
                                                  error);
}
