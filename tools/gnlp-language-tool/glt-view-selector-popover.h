/* glt-view-selector-popover.h
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

#ifndef GLT_VIEW_SELECTOR_POPOVER_H
#define GLT_VIEW_SELECTOR_POPOVER_H

#include "glt-view.h"

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GLT_TYPE_VIEW_SELECTOR_POPOVER (glt_view_selector_popover_get_type())

G_DECLARE_FINAL_TYPE (GltViewSelectorPopover, glt_view_selector_popover, GLT, VIEW_SELECTOR_POPOVER, GtkPopover)

GtkWidget*           glt_view_selector_popover_new               (void);

void                 glt_view_selector_popover_add_view          (GltViewSelectorPopover *self,
                                                                  GltView                *view);

GltView*             glt_view_selector_popover_get_view          (GltViewSelectorPopover *self);

void                 glt_view_selector_popover_set_view          (GltViewSelectorPopover *self,
                                                                  GltView                *view);

G_END_DECLS

#endif /* GLT_VIEW_SELECTOR_POPOVER_H */

