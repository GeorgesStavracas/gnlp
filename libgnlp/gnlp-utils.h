/* gnlp-utils.h
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

#ifndef GNLP_UTILS_H
#define GNLP_UTILS_H

#if !defined (GNLP_INSIDE) && !defined (GNLP_COMPILATION)
#error "Only <gnlp-0/gnlp.h> can be included directly."
#endif

#include <gio/gio.h>

G_BEGIN_DECLS

void                 gnlp_get_available_languages                     (GCancellable        *cancellable,
                                                                       GAsyncReadyCallback  callback,
                                                                       gpointer             user_data);

GPtrArray*           gnlp_get_available_languages_finish              (GAsyncResult  *result,
                                                                       GError       **error);

GPtrArray*           gnlp_get_available_languages_sync                (GCancellable  *cancellable,
                                                                       GError       **error);

G_END_DECLS

#endif /* GNLP_UTILS_H */

