/* glt-view-selector-popover.c
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
#include "glt-view-selector-popover.h"

struct _GltViewSelectorPopover
{
  GtkPopover          parent;

  GtkWidget          *listbox;

  GltView            *current_view;
};

G_DEFINE_TYPE (GltViewSelectorPopover, glt_view_selector_popover, GTK_TYPE_POPOVER)

enum {
  PROP_0,
  PROP_CURRENT_VIEW,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

static void
set_current_view (GltViewSelectorPopover *self,
                  GltViewRow             *row)
{
  if (g_set_object (&self->current_view, glt_view_row_get_view (row)))
    g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_CURRENT_VIEW]);

  gtk_widget_hide (GTK_WIDGET (self));
}

static GtkWidget*
get_view_row (GltViewSelectorPopover *self,
              GltView                *view)
{
  GtkWidget *row;
  GList *children, *l;

  row = NULL;
  children = gtk_container_get_children (GTK_CONTAINER (self->listbox));

  for (l = children; l != NULL; l = l->next)
    {
      if (view == glt_view_row_get_view (l->data))
        {
          row = l->data;
          break;
        }
    }

  g_list_free (children);

  return row;

}

static void
glt_view_selector_popover_finalize (GObject *object)
{
  GltViewSelectorPopover *self = (GltViewSelectorPopover *)object;

  G_OBJECT_CLASS (glt_view_selector_popover_parent_class)->finalize (object);
}

static void
glt_view_selector_popover_get_property (GObject    *object,
                                        guint       prop_id,
                                        GValue     *value,
                                        GParamSpec *pspec)
{
  GltViewSelectorPopover *self = GLT_VIEW_SELECTOR_POPOVER (object);

  switch (prop_id)
    {
    case PROP_CURRENT_VIEW:
      g_value_set_object (value, self->current_view);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
glt_view_selector_popover_set_property (GObject      *object,
                                        guint         prop_id,
                                        const GValue *value,
                                        GParamSpec   *pspec)
{
  GltViewSelectorPopover *self = GLT_VIEW_SELECTOR_POPOVER (object);

  switch (prop_id)
    {
    case PROP_CURRENT_VIEW:
      glt_view_selector_popover_set_view (self, g_value_get_object (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
glt_view_selector_popover_class_init (GltViewSelectorPopoverClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->finalize = glt_view_selector_popover_finalize;
  object_class->get_property = glt_view_selector_popover_get_property;
  object_class->set_property = glt_view_selector_popover_set_property;

  properties[PROP_CURRENT_VIEW] = g_param_spec_object ("current-view",
                                                       "",
                                                       "",
                                                       GLT_TYPE_VIEW,
                                                       G_PARAM_READWRITE);

  g_object_class_install_properties (object_class, N_PROPS, properties);

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/glt/ui/view-selector-popover.ui");

  gtk_widget_class_bind_template_child (widget_class, GltViewSelectorPopover, listbox);
  gtk_widget_class_bind_template_callback (widget_class, set_current_view);
}

static void
glt_view_selector_popover_init (GltViewSelectorPopover *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}


void
glt_view_selector_popover_add_view (GltViewSelectorPopover *self,
                                    GltView                *view)
{
  GtkWidget *row;

  g_return_if_fail (GLT_IS_VIEW_SELECTOR_POPOVER (self));

  row = glt_view_row_new (view);
  gtk_container_add (GTK_CONTAINER (self->listbox), row);

  if (!self->current_view)
    set_current_view (self, GLT_VIEW_ROW (row));
}

GltView*
glt_view_selector_popover_get_current_view (GltViewSelectorPopover *self)
{
  g_return_val_if_fail (GLT_IS_VIEW_SELECTOR_POPOVER (self), NULL);

  return self->current_view;
}

void
glt_view_selector_popover_set_view (GltViewSelectorPopover *self,
                                    GltView                *view)
{
  GtkWidget *row;

  g_return_if_fail (GLT_IS_VIEW_SELECTOR_POPOVER (self));
  g_return_if_fail (GLT_IS_VIEW (view));

  row = get_view_row (self, view);

  if (row)
    g_signal_emit_by_name (row, "activate");
}
