/* glt-view.c
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

#include "glt-view.h"

G_DEFINE_INTERFACE (GltView, glt_view, GTK_TYPE_WIDGET)

static void
glt_view_default_init (GltViewInterface *iface)
{
  g_object_interface_install_property (iface,
                                       g_param_spec_object ("icon",
                                                            "Icon",
                                                            "The view icon",
                                                            G_TYPE_ICON,
                                                            G_PARAM_READABLE));

  g_object_interface_install_property (iface,
                                       g_param_spec_string ("name",
                                                            "Name",
                                                            "The view name",
                                                            NULL,
                                                            G_PARAM_READABLE));


  g_object_interface_install_property (iface,
                                       g_param_spec_string ("title",
                                                            "Title",
                                                            "The view title",
                                                            NULL,
                                                            G_PARAM_READABLE));

}

const GIcon*
glt_view_get_icon (GltView *self)
{
  g_return_val_if_fail (GLT_IS_VIEW (self), NULL);
  g_return_val_if_fail (GLT_VIEW_GET_IFACE (self)->get_icon, NULL);

  return GLT_VIEW_GET_IFACE (self)->get_icon (self);
}

const gchar*
glt_view_get_name (GltView *self)
{
  g_return_val_if_fail (GLT_IS_VIEW (self), NULL);
  g_return_val_if_fail (GLT_VIEW_GET_IFACE (self)->get_name, NULL);

  return GLT_VIEW_GET_IFACE (self)->get_name (self);
}

const gchar*
glt_view_get_title (GltView *self)
{
  g_return_val_if_fail (GLT_IS_VIEW (self), NULL);
  g_return_val_if_fail (GLT_VIEW_GET_IFACE (self)->get_title, NULL);

  return GLT_VIEW_GET_IFACE (self)->get_title (self);
}
