/* gnlp-command-dispatcher.h
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

#ifndef GNLP_COMMAND_DISPATCHER_H
#define GNLP_COMMAND_DISPATCHER_H

#include "gnlp-voice-command.h"

#include <glib-object.h>

G_BEGIN_DECLS

#define GNLP_TYPE_COMMAND_DISPATCHER (gnlp_command_dispatcher_get_type())

G_DECLARE_FINAL_TYPE (GnlpCommandDispatcher, gnlp_command_dispatcher, GNLP, COMMAND_DISPATCHER, GObject)

GnlpCommandDispatcher* gnlp_command_dispatcher_new                (void);

gboolean             gnlp_command_dispatcher_dispatch_command     (GnlpCommandDispatcher *self,
                                                                   GnlpVoiceCommand      *command);

const gchar*         gnlp_command_dispatcher_get_last_target      (GnlpCommandDispatcher *self);

GnlpVoiceCommand*    gnlp_command_dispatcher_get_previous_command (GnlpCommandDispatcher *self);

G_END_DECLS

#endif /* GNLP_COMMAND_DISPATCHER_H */

