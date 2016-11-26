/* glt-workbench.h
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

#ifndef GLT_WORKBENCH_H
#define GLT_WORKBENCH_H

#include "glt-application.h"
#include "glt-view.h"

#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GLT_TYPE_WORKBENCH (glt_workbench_get_type())

G_DECLARE_FINAL_TYPE (GltWorkbench, glt_workbench, GLT, WORKBENCH, GtkApplicationWindow)

GltWorkbench*        glt_workbench_new                           (GltApplication     *application);

void                 glt_workbench_show_view                     (GltWorkbench       *self,
                                                                  GltView            *view);

void                 glt_workbench_show_view_from_name           (GltWorkbench       *self,
                                                                  const gchar        *name);

G_END_DECLS

#endif /* GLT_WORKBENCH_H */

