/* gnlp-daemon.c
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

#define G_LOG_DOMAIN "Daemon"

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "gnlp-daemon.h"
#include "gnlp-listener.h"
#include "gnlp-resources.h"
#include "gnlp-settings.h"
#include "gnlp-speaker.h"

#include <stdlib.h>
#include <glib/gi18n.h>

struct _GnlpDaemon
{
  GApplication        parent;

  GnlpListener       *listener;
  GnlpSpeechListener *dbus_listener;

  GnlpSettings       *settings;
  GnlpSpeechSettings *dbus_settings;

  GnlpSpeaker        *speaker;
  GnlpSpeechSpeaker  *dbus_speaker;

  GDBusObjectManagerServer *dbus_server;

  GnlpLanguage       *language;
};

G_DEFINE_TYPE (GnlpDaemon, gnlp_daemon, G_TYPE_APPLICATION)

enum
{
  PROP_0,
  PROP_LISTENER,
  N_PROPS
};


/*
 * Settings
 */

static void
setup_speech_settings (GnlpDaemon *self)
{
  GnlpObjectSkeleton *object;

  /* Init the listener and the DBus object */
  self->settings = gnlp_settings_new ();
  self->dbus_settings = gnlp_speech_settings_skeleton_new ();

  /* Export the listener */
  object = gnlp_object_skeleton_new ("/org/gnome/Gnlp/Speech/Settings");
  gnlp_object_skeleton_set_speech_settings (object, self->dbus_settings);
  g_dbus_object_manager_server_export (self->dbus_server, G_DBUS_OBJECT_SKELETON (object));

  g_clear_object (&object);

  g_object_bind_property (self->settings,
                          "language-name",
                          self->dbus_settings,
                          "language",
                          G_BINDING_BIDIRECTIONAL);
}


/*
 * Listener
 */

static void
setup_speech_listener (GnlpDaemon *self)
{
  GnlpObjectSkeleton *object;

  /* Init the listener and the DBus object */
  self->listener = gnlp_listener_new (self->settings);
  self->dbus_listener = gnlp_speech_listener_skeleton_new ();

  /* Export the listener */
  object = gnlp_object_skeleton_new ("/org/gnome/Gnlp/Speech/Listener");
  gnlp_object_skeleton_set_speech_listener (object, self->dbus_listener);
  g_dbus_object_manager_server_export (self->dbus_server, G_DBUS_OBJECT_SKELETON (object));

  g_clear_object (&object);

  g_signal_connect_swapped (self->listener,
                            "speech-recognized",
                            G_CALLBACK (gnlp_speech_listener_emit_speech_recognized),
                            self->dbus_listener);

  g_object_bind_property (self->listener,
                          "listening",
                          self->dbus_listener,
                          "listening",
                          G_BINDING_DEFAULT);

  /* Start listening voice input */
  gnlp_listener_run (self->listener);
}

/*
 * Speaker
 */
static gboolean
handle_read_text (GnlpDaemon            *self,
                  GDBusMethodInvocation *invocation,
                  const gchar           *text,
                  GnlpSpeechSpeaker     *speaker)
{
  gnlp_speaker_read (self->speaker, invocation, text);

  return TRUE;
}

static void
setup_speech_speaker (GnlpDaemon *self)
{
  GnlpObjectSkeleton *object;

  /* Init the speaker and the DBus object */
  self->speaker = gnlp_speaker_new (self->settings);
  self->dbus_speaker = gnlp_speech_speaker_skeleton_new ();

  /* Export the listener */
  object = gnlp_object_skeleton_new ("/org/gnome/Gnlp/Speech/Speaker");
  gnlp_object_skeleton_set_speech_speaker (object, self->dbus_speaker);
  g_dbus_object_manager_server_export (self->dbus_server, G_DBUS_OBJECT_SKELETON (object));

  g_clear_object (&object);

  g_signal_connect_swapped (self->speaker,
                            "text-read",
                            G_CALLBACK (gnlp_speech_speaker_complete_read_text),
                            self->dbus_speaker);

  g_signal_connect_swapped (self->dbus_speaker,
                            "handle-read-text",
                            G_CALLBACK (handle_read_text),
                            self);
}


/*
 * Overrides
 */

static void
gnlp_daemon_finalize (GObject *object)
{
  G_OBJECT_CLASS (gnlp_daemon_parent_class)->finalize (object);
}

static void
gnlp_daemon_get_property (GObject    *object,
                          guint       prop_id,
                          GValue     *value,
                          GParamSpec *pspec)
{
  GnlpDaemon *self = GNLP_DAEMON (object);

  switch (prop_id)
    {
    case PROP_LISTENER:
      g_value_set_object (value, self->listener);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gnlp_daemon_set_property (GObject      *object,
                          guint         prop_id,
                          const GValue *value,
                          GParamSpec   *pspec)
{
  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gnlp_daemon_startup (GApplication *application)
{
  g_debug ("Starting up daemon...");

  /* Hook up to the parent class */
  G_APPLICATION_CLASS (gnlp_daemon_parent_class)->startup (application);

  g_debug ("Startup finished");
}

static gboolean
gnlp_daemon_dbus_register (GApplication     *application,
                           GDBusConnection  *connection,
                           const gchar      *object_path,
                           GError          **error)
{
  GnlpDaemon *self;

  self = GNLP_DAEMON (application);

  /* Chain up before anything else */
  if (!G_APPLICATION_CLASS (gnlp_daemon_parent_class)->dbus_register (application,
                                                                      connection,
                                                                      object_path,
                                                                      error))
    {
      return FALSE;
    }


  g_debug ("DBus name registered: \"%s\"", object_path);

  /* The main DBus server-side object manager */
  self->dbus_server = g_dbus_object_manager_server_new ("/org/gnome/Gnlp");
  g_dbus_object_manager_server_set_connection (self->dbus_server, connection);

  setup_speech_settings (self);
  setup_speech_listener (self);
  setup_speech_speaker  (self);

  return TRUE;
}

static void
gnlp_daemon_class_init (GnlpDaemonClass *klass)
{
  GApplicationClass *application_class = G_APPLICATION_CLASS (klass);
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gnlp_daemon_finalize;
  object_class->get_property = gnlp_daemon_get_property;
  object_class->set_property = gnlp_daemon_set_property;

  application_class->startup = gnlp_daemon_startup;
  application_class->dbus_register = gnlp_daemon_dbus_register;

  g_object_class_install_property (object_class,
                                   PROP_LISTENER,
                                   g_param_spec_object ("listener",
                                                        "Listener",
                                                        "Listener of voice speech",
                                                        GNLP_TYPE_LISTENER,
                                                        G_PARAM_READABLE));
}

static void
gnlp_daemon_init (GnlpDaemon *self)
{
}

GnlpDaemon*
gnlp_daemon_new (void)
{
  return g_object_new (GNLP_TYPE_DAEMON,
                       "flags", G_APPLICATION_IS_SERVICE,
                       "application-id", "org.gnome.Gnlp",
                       NULL);
}

GnlpListener*
gnlp_daemon_get_listener (GnlpDaemon *self)
{
  g_return_val_if_fail (GNLP_IS_DAEMON (self), NULL);

  return self->listener;
}

GnlpSpeechListener*
gnlp_daemon_get_speech_listener (GnlpDaemon *self)
{
  g_return_val_if_fail (GNLP_IS_DAEMON (self), NULL);

  return self->dbus_listener;
}
