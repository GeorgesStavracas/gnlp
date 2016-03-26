/* gnlp-context.h
 *
 * Copyright (C) 2016 Georges Basile Stavracas Neto <georges.stavracas@gmail.com>
 *
 * This file is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.
 *
 * This file is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GNLP_CONTEXT_H
#define GNLP_CONTEXT_H

#include <gio/gio.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define GNLP_TYPE_CONTEXT (gnlp_context_get_type())

G_DECLARE_FINAL_TYPE (GnlpContext, gnlp_context, GNLP, CONTEXT, GObject)

void                 gnlp_context_new                            (const gchar         *application_id,
                                                                  GCancellable        *cancellable,
                                                                  GAsyncReadyCallback  callback,
                                                                  gpointer             user_data);

GnlpContext*         gnlp_context_new_finish                     (GAsyncResult       *result,
                                                                  GError            **error);

GnlpContext*         gnlp_context_new_sync                       (const gchar        *application_id,
                                                                  GCancellable       *cancellable,
                                                                  GError            **error);

void                 gnlp_context_list_operations                (GnlpContext         *self,
                                                                  const gchar         *language,
                                                                  GCancellable        *cancellable,
                                                                  GAsyncReadyCallback  callback,
                                                                  gpointer             user_data);

GList*               gnlp_context_list_operations_finish         (GAsyncResult       *result,
                                                                  GError            **error);

GList*               gnlp_context_list_operations_sync           (GnlpContext        *self,
                                                                  const gchar        *language,
                                                                  GCancellable       *cancellable,
                                                                  GError            **error);

G_END_DECLS

#endif /* GNLP_CONTEXT_H */

