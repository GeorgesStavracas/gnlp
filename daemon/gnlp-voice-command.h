/* gnlp-voice-command.h
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

#ifndef GNLP_VOICE_COMMAND_H
#define GNLP_VOICE_COMMAND_H

#include <glib-object.h>

G_BEGIN_DECLS

#define GNLP_TYPE_VOICE_COMMAND (gnlp_voice_command_get_type())

typedef struct _GnlpVoiceCommand GnlpVoiceCommand;

GType                gnlp_voice_command_get_type                 (void) G_GNUC_CONST;

GnlpVoiceCommand*    gnlp_voice_command_new                      (void);

GnlpVoiceCommand*    gnlp_voice_command_copy                     (GnlpVoiceCommand   *self);

GnlpVoiceCommand*    gnlp_voice_command_ref                      (GnlpVoiceCommand   *self);

void                 gnlp_voice_command_unref                    (GnlpVoiceCommand   *self);

const gchar*         gnlp_voice_command_get_target               (GnlpVoiceCommand   *self);

void                 gnlp_voice_command_set_target               (GnlpVoiceCommand   *self,
                                                                  const gchar        *target);

const gchar*         gnlp_voice_command_get_action               (GnlpVoiceCommand   *self);

void                 gnlp_voice_command_set_action               (GnlpVoiceCommand   *self,
                                                                  const gchar        *action);

GPtrArray*           gnlp_voice_command_get_parameters           (GnlpVoiceCommand   *self);

void                 gnlp_voice_command_push_parameter           (GnlpVoiceCommand   *self,
                                                                  const gchar        *parameter);

gchar*               gnlp_voice_command_pop_parameter            (GnlpVoiceCommand   *self);

G_DEFINE_AUTOPTR_CLEANUP_FUNC (GnlpVoiceCommand, gnlp_voice_command_unref)

G_END_DECLS

#endif /* GNLP_VOICE_COMMAND_H */

