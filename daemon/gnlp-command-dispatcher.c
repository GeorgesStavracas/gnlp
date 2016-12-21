/* gnlp-command-dispatcher.c
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

#include "gnlp-command-dispatcher.h"
#include "gnlp-daemon.h"

struct _GnlpCommandDispatcher
{
  GObject             parent;

  gchar              *last_target;
  GnlpVoiceCommand   *previous_command;
};

G_DEFINE_TYPE (GnlpCommandDispatcher, gnlp_command_dispatcher, G_TYPE_OBJECT)

enum
{
  PROP_0,
  PROP_LAST_TARGET,
  PROP_PREVIOUS_COMMAND,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

/* Auxiliary functions */
static inline GnlpSpeechListener*
get_dbus_listener (void)
{
  GnlpDaemon *d = GNLP_DAEMON (g_application_get_default ());
  return gnlp_daemon_get_speech_listener (d);
}

static void
gnlp_command_dispatcher_finalize (GObject *object)
{
  GnlpCommandDispatcher *self = (GnlpCommandDispatcher *)object;

  g_clear_pointer (&self->last_target, g_free);
  g_clear_pointer (&self->previous_command, gnlp_voice_command_unref);

  G_OBJECT_CLASS (gnlp_command_dispatcher_parent_class)->finalize (object);
}

static void
gnlp_command_dispatcher_get_property (GObject    *object,
                                      guint       prop_id,
                                      GValue     *value,
                                      GParamSpec *pspec)
{
  GnlpCommandDispatcher *self = GNLP_COMMAND_DISPATCHER (object);

  switch (prop_id)
    {
    case PROP_LAST_TARGET:
      g_value_set_string (value, self->last_target);
      break;

    case PROP_PREVIOUS_COMMAND:
      g_value_set_boxed (value, self->previous_command);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gnlp_command_dispatcher_set_property (GObject      *object,
                                      guint         prop_id,
                                      const GValue *value,
                                      GParamSpec   *pspec)
{
  G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
}

static void
gnlp_command_dispatcher_class_init (GnlpCommandDispatcherClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gnlp_command_dispatcher_finalize;
  object_class->get_property = gnlp_command_dispatcher_get_property;
  object_class->set_property = gnlp_command_dispatcher_set_property;

  properties[PROP_LAST_TARGET] = g_param_spec_string ("last-target",
                                                      "Last target",
                                                      "Last target dispatched",
                                                      "",
                                                      G_PARAM_READABLE);

  properties[PROP_PREVIOUS_COMMAND] = g_param_spec_boxed ("previous-command",
                                                          "Previously executed command",
                                                          "Previously executed command",
                                                          GNLP_TYPE_VOICE_COMMAND,
                                                          G_PARAM_READABLE);
}

static void
gnlp_command_dispatcher_init (GnlpCommandDispatcher *self)
{
}

/**
 * gnlp_command_dispatcher_new:
 *
 * Creates a new #GnlpCommandDispatcher
 *
 * Returns: (transfer full): a newly created #GnlpCommandDispatcher
 */
GnlpCommandDispatcher*
gnlp_command_dispatcher_new (void)
{
  return g_object_new (GNLP_TYPE_COMMAND_DISPATCHER, NULL);
}

/**
 * gnlp_command_dispatcher_dispatch_command:
 * @self: a #GnlpCommandDispatcher
 * @command: a #GnlpVoiceCommand
 *
 * Executes and/or dispatches @command through DBus.
 *
 * Returns: %TRUE on success, %FALSE on failure.
 */
gboolean
gnlp_command_dispatcher_dispatch_command (GnlpCommandDispatcher *self,
                                          GnlpVoiceCommand      *command)
{
  GnlpSpeechListener *dbus_listener;
  GPtrArray *params;
  const gchar *target;

  g_return_val_if_fail (GNLP_IS_COMMAND_DISPATCHER (self), FALSE);

  params = gnlp_voice_command_get_parameters (command);
  target = gnlp_voice_command_get_target (command);
  dbus_listener = get_dbus_listener ();

  if (!dbus_listener)
    return FALSE;

  if (!target && !self->last_target)
    return FALSE;

  /* Save the current target */
  if (target)
    {
      g_clear_pointer (&self->last_target, g_free);
      self->last_target = g_strdup (target);
      g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_LAST_TARGET]);
    }

  g_clear_pointer (&self->previous_command, gnlp_voice_command_unref);
  self->previous_command = gnlp_voice_command_ref (command);
  g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_PREVIOUS_COMMAND]);

  /* TODO: check if we handle the target and command internally */

  /* Send the DBus message */
  gnlp_speech_listener_emit_command_received (dbus_listener,
                                              target,
                                              gnlp_voice_command_get_action (command),
                                              params ? (const gchar* const *) params->pdata : NULL,
                                              1.0);

  return TRUE;
}

/**
 * gnlp_command_dispatcher_get_last_target:
 * @self: a #GnlpCommandDispatcher
 *
 * Retrieves the last valid target dispatched by @self.
 *
 * Returns: (transfer none)(nullable): a string
 */
const gchar*
gnlp_command_dispatcher_get_last_target (GnlpCommandDispatcher *self)
{
  g_return_val_if_fail (GNLP_IS_COMMAND_DISPATCHER (self), NULL);

  return self->last_target;
}

/**
 * gnlp_command_dispatcher_get_previous_command:
 * @self: a #GnlpCommandDispatcher
 *
 * Retrieves the previous successfully dispatched command by @self.
 *
 * Returns: (transfer none)(nullable): a string
 */
GnlpVoiceCommand*
gnlp_command_dispatcher_get_previous_command (GnlpCommandDispatcher *self)
{
  g_return_val_if_fail (GNLP_IS_COMMAND_DISPATCHER (self), NULL);

  return self->previous_command;
}
