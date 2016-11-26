/* glt-view.h
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

#ifndef GLT_VIEW_H
#define GLT_VIEW_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GLT_TYPE_VIEW (glt_view_get_type ())

G_DECLARE_INTERFACE (GltView, glt_view, GLT, VIEW, GtkWidget)

struct _GltViewInterface
{
  GTypeInterface      parent;

  const GIcon*       (*get_icon)                                 (GltView            *self);

  const gchar*       (*get_name)                                 (GltView            *self);

  const gchar*       (*get_title)                                (GltView            *self);
};

const GIcon*         glt_view_get_icon                           (GltView             *self);

const gchar*         glt_view_get_name                           (GltView             *self);

const gchar*         glt_view_get_title                          (GltView             *self);

G_END_DECLS

#endif /* GLT_VIEW_H */
