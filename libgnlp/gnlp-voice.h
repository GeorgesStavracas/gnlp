/* gnlp-voice.h
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

#ifndef GNLP_VOICE_H
#define GNLP_VOICE_H

#include <gio/gio.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define GNLP_TYPE_VOICE (gnlp_voice_get_type())

#define GNLP_VOICE_AGE_NOT_SPECIFIED 0

typedef enum
{
  GNLP_VOICE_GENDER_NONE,
  GNLP_VOICE_GENDER_MALE,
  GNLP_VOICE_GENDER_FEMALE
} GnlpVoiceGender;

typedef struct _GnlpVoice GnlpVoice;

GType                gnlp_voice_get_type                         (void) G_GNUC_CONST;

GnlpVoice*           gnlp_voice_copy                             (GnlpVoice          *self);

GnlpVoice*           gnlp_voice_ref                              (GnlpVoice          *self);

void                 gnlp_voice_unref                            (GnlpVoice          *self);

const gchar*         gnlp_voice_get_name                         (GnlpVoice          *self);

GnlpVoiceGender      gnlp_voice_get_gender                       (GnlpVoice          *self);

gint                 gnlp_voice_get_age                          (GnlpVoice          *self);

void                 gnlp_voice_list_available_voices            (GCancellable        *cancellable,
                                                                  GAsyncReadyCallback  callback,
                                                                  gpointer             user_data);

GPtrArray*           gnlp_voice_list_available_voices_finish     (GAsyncResult        *result,
                                                                  GError             **error);

GPtrArray*           gnlp_voice_list_available_voices_sync       (GCancellable        *cancellable,
                                                                  GError             **error);

G_DEFINE_AUTOPTR_CLEANUP_FUNC (GnlpVoice, gnlp_voice_unref)

G_END_DECLS

#endif /* GNLP_VOICE_H */
