/* glt-view-row.h
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

#ifndef GLT_VIEW_ROW_H
#define GLT_VIEW_ROW_H

#include "glt-view.h"

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GLT_TYPE_VIEW_ROW (glt_view_row_get_type())

G_DECLARE_FINAL_TYPE (GltViewRow, glt_view_row, GLT, VIEW_ROW, GtkListBoxRow)

GtkWidget*           glt_view_row_new                            (GltView             *view);

GltView*             glt_view_row_get_view                       (GltViewRow          *self);

G_END_DECLS

#endif /* GLT_VIEW_ROW_H */

