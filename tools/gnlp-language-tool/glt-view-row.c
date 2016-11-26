/* glt-view-row.c
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

#include "glt-view-row.h"

struct _GltViewRow
{
  GtkListBoxRow       parent;

  GtkWidget          *view_acceletator_label;
  GtkWidget          *view_icon;
  GtkWidget          *view_name_label;

  GltView            *view;
};

G_DEFINE_TYPE (GltViewRow, glt_view_row, GTK_TYPE_LIST_BOX_ROW)

enum {
  PROP_0,
  PROP_VIEW,
  N_PROPS
};

static GParamSpec *properties [N_PROPS] = { NULL, };

static void
glt_view_row_finalize (GObject *object)
{
  GltViewRow *self = (GltViewRow *)object;

  G_OBJECT_CLASS (glt_view_row_parent_class)->finalize (object);
}

static void
glt_view_row_get_property (GObject    *object,
                                     guint       prop_id,
                                     GValue     *value,
                                     GParamSpec *pspec)
{
  GltViewRow *self = GLT_VIEW_ROW (object);

  switch (prop_id)
    {
    case PROP_VIEW:
      g_value_set_object (value, self->view);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
glt_view_row_set_property (GObject      *object,
                                     guint         prop_id,
                                     const GValue *value,
                                     GParamSpec   *pspec)
{
  GltViewRow *self = GLT_VIEW_ROW (object);

  switch (prop_id)
    {
    case PROP_VIEW:
      self->view = g_value_dup_object (value);

      g_object_bind_property (self->view,
                              "visible",
                              self,
                              "visible",
                              G_BINDING_DEFAULT | G_BINDING_SYNC_CREATE);

      g_object_bind_property (self->view,
                              "icon",
                              self->view_icon,
                              "gicon",
                              G_BINDING_DEFAULT | G_BINDING_SYNC_CREATE);

      g_object_bind_property (self->view,
                              "title",
                              self->view_name_label,
                              "label",
                              G_BINDING_DEFAULT | G_BINDING_SYNC_CREATE);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
glt_view_row_class_init (GltViewRowClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->finalize = glt_view_row_finalize;
  object_class->get_property = glt_view_row_get_property;
  object_class->set_property = glt_view_row_set_property;

  properties[PROP_VIEW] = g_param_spec_object ("view",
                                               "",
                                               "",
                                               GLT_TYPE_VIEW,
                                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

  g_object_class_install_properties (object_class, N_PROPS, properties);

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/glt/ui/view-row.ui");

  gtk_widget_class_bind_template_child (widget_class, GltViewRow, view_acceletator_label);
  gtk_widget_class_bind_template_child (widget_class, GltViewRow, view_icon);
  gtk_widget_class_bind_template_child (widget_class, GltViewRow, view_name_label);
}

static void
glt_view_row_init (GltViewRow *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}

GtkWidget*
glt_view_row_new (GltView *view)
{
  return g_object_new (GLT_TYPE_VIEW_ROW,
                       "view", view,
                       NULL);
}

GltView*
glt_view_row_get_view (GltViewRow *self)
{
  g_return_val_if_fail (GLT_IS_VIEW_ROW (self), NULL);

  return self->view;
}
