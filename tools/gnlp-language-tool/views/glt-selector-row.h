/* glt-selector-row.h
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

#ifndef GLT_SELECTOR_ROW_H
#define GLT_SELECTOR_ROW_H

#include "gnlp.h"

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GLT_TYPE_SELECTOR_ROW (glt_selector_row_get_type())

G_DECLARE_FINAL_TYPE (GltSelectorRow, glt_selector_row, GLT, SELECTOR_ROW, GtkListBoxRow)

GtkWidget*           glt_selector_row_new                        (GnlpLanguage       *language);

GnlpLanguage*        glt_selector_row_get_language               (GltSelectorRow     *self);

G_END_DECLS

#endif /* GLT_SELECTOR_ROW_H */

