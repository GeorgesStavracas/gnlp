/* glt-language-view.h
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

#ifndef GLT_LANGUAGE_VIEW_H
#define GLT_LANGUAGE_VIEW_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GLT_TYPE_LANGUAGE_VIEW (glt_language_view_get_type())

G_DECLARE_FINAL_TYPE (GltLanguageView, glt_language_view, GLT, LANGUAGE_VIEW, GtkBox)

G_END_DECLS

#endif /* GLT_LANGUAGE_VIEW_H */

