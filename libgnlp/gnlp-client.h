/* gnlp-client.h
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

#ifndef GNLP_CLIENT_H
#define GNLP_CLIENT_H

#if !defined (GNLP_INSIDE) && !defined (GNLP_COMPILATION)
#error "Only <gnlp-0/gnlp.h> can be included directly."
#endif

#include <gio/gio.h>

G_BEGIN_DECLS

#define GNLP_TYPE_CLIENT (gnlp_client_get_type())

G_DECLARE_FINAL_TYPE (GnlpClient, gnlp_client, GNLP, CLIENT, GObject)

void                 gnlp_client_new                             (const gchar         *module,
                                                                  GCancellable        *cancellable,
                                                                  GAsyncReadyCallback  callback,
                                                                  gpointer             user_data);

GnlpClient*          gnlp_client_new_finish                      (GAsyncResult       *result,
                                                                  GError            **error);

GnlpClient*          gnlp_client_new_sync                        (const gchar        *module,
                                                                  GCancellable       *cancellable,
                                                                  GError            **error);

gboolean             gnlp_client_is_listening                    (GnlpClient         *self);

const gchar*         gnlp_client_get_language                    (GnlpClient         *self);

void                 gnlp_client_set_language                    (GnlpClient         *self,
                                                                  const gchar        *language);

GPtrArray*           gnlp_client_get_available_languages         (void);

void                 gnlp_client_read_text                       (GnlpClient          *self,
                                                                  const gchar         *text,
                                                                  GCancellable        *cancellable,
                                                                  GAsyncReadyCallback  callback,
                                                                  gpointer             user_data);

gboolean             gnlp_client_read_text_finished              (GnlpClient         *client,
                                                                  GAsyncResult       *result,
                                                                  GError            **error);

gboolean             gnlp_client_read_text_sync                  (GnlpClient         *self,
                                                                  const gchar        *text,
                                                                  GCancellable       *cancellable,
                                                                  GError            **error);

G_END_DECLS

#endif /* GNLP_CLIENT_H */

