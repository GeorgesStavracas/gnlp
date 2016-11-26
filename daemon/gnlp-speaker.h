/* gnlp-speaker.h
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

#ifndef GNLP_SPEAKER_H
#define GNLP_SPEAKER_H

#include "gnlp-settings.h"

#include <glib-object.h>
#include <gio/gio.h>

G_BEGIN_DECLS

#define GNLP_TYPE_SPEAKER (gnlp_speaker_get_type())

G_DECLARE_FINAL_TYPE (GnlpSpeaker, gnlp_speaker, GNLP, SPEAKER, GObject)

GnlpSpeaker*         gnlp_speaker_new                            (GnlpSettings       *settings);

void                 gnlp_speaker_read                           (GnlpSpeaker           *self,
                                                                  GDBusMethodInvocation *invocation,
                                                                  const gchar           *text);

G_END_DECLS

#endif /* GNLP_SPEAKER_H */

