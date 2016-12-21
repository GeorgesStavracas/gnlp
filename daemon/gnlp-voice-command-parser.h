/* gnlp-voice-command-parser.h
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

#ifndef GNLP_VOICE_COMMAND_PARSER_H
#define GNLP_VOICE_COMMAND_PARSER_H

#include <glib-object.h>

G_BEGIN_DECLS

#define GNLP_TYPE_VOICE_COMMAND_PARSER (gnlp_voice_command_parser_get_type())

G_DECLARE_FINAL_TYPE (GnlpVoiceCommandParser, gnlp_voice_command_parser, GNLP, VOICE_COMMAND_PARSER, GObject)

GnlpVoiceCommandParser *gnlp_voice_command_parser_new (void);

G_END_DECLS

#endif /* GNLP_VOICE_COMMAND_PARSER_H */

