/* glt-workbench.c
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

#include "glt-language-view.h"
#include "glt-selector-view.h"

#include "glt-view-selector-popover.h"
#include "glt-workbench.h"

struct _GltWorkbench
{
  GtkApplicationWindow parent;

  GtkWidget          *main_stack;
  GtkWidget          *view_icon;
  GtkWidget          *views_popover;

  GltView            *current_view;
  GBinding           *icon_binding;

  GHashTable         *name_to_view;
  GPtrArray          *views;
};

G_DEFINE_TYPE (GltWorkbench, glt_workbench, GTK_TYPE_APPLICATION_WINDOW)

enum {
  PROP_0,
  PROP_CURRENT_VIEW,
  N_PROPS
};

static GParamSpec *properties [N_PROPS] = { NULL, };

static void
initialize_views (GltWorkbench *self)
{
  gint i;

  GType views[] = {
    GLT_TYPE_SELECTOR_VIEW,
    GLT_TYPE_LANGUAGE_VIEW
  };

  for (i = 0; i < G_N_ELEMENTS (views); i++)
    {
      GltView *view;

      view = g_object_new (views[i], NULL);

      gtk_stack_add_titled (GTK_STACK (self->main_stack),
                            GTK_WIDGET (view),
                            glt_view_get_name (view),
                            glt_view_get_title (view));

      glt_view_selector_popover_add_view (GLT_VIEW_SELECTOR_POPOVER (self->views_popover), view);

      g_hash_table_insert (self->name_to_view,
                           (gchar*) glt_view_get_name (view),
                           view);

      g_ptr_array_add (self->views, view);
    }
}

static void
glt_workbench_finalize (GObject *object)
{
  GltWorkbench *self = (GltWorkbench *)object;

  g_clear_pointer (&self->name_to_view, g_hash_table_destroy);
  g_clear_pointer (&self->views, g_ptr_array_unref);

  G_OBJECT_CLASS (glt_workbench_parent_class)->finalize (object);
}

static void
glt_workbench_constructed (GObject *object)
{
  GltWorkbench *self = GLT_WORKBENCH (object);

  initialize_views (self);

  G_OBJECT_CLASS (glt_workbench_parent_class)->constructed (object);
}

static void
glt_workbench_get_property (GObject    *object,
                            guint       prop_id,
                            GValue     *value,
                            GParamSpec *pspec)
{
  GltWorkbench *self = GLT_WORKBENCH (object);

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
glt_workbench_set_property (GObject      *object,
                            guint         prop_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
  GltWorkbench *self = GLT_WORKBENCH (object);

  switch (prop_id)
    {
    case PROP_CURRENT_VIEW:
      glt_workbench_show_view (self, g_value_get_object (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
glt_workbench_class_init (GltWorkbenchClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->finalize = glt_workbench_finalize;
  object_class->constructed = glt_workbench_constructed;
  object_class->get_property = glt_workbench_get_property;
  object_class->set_property = glt_workbench_set_property;

  g_type_ensure (GLT_TYPE_VIEW_SELECTOR_POPOVER);

  properties[PROP_CURRENT_VIEW] = g_param_spec_object ("current-view",
                                                       "",
                                                       "",
                                                       GLT_TYPE_VIEW,
                                                       G_PARAM_READWRITE);

  g_object_class_install_properties (object_class, N_PROPS, properties);

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/glt/ui/workbench.ui");

  gtk_widget_class_bind_template_child (widget_class, GltWorkbench, main_stack);
  gtk_widget_class_bind_template_child (widget_class, GltWorkbench, view_icon);
  gtk_widget_class_bind_template_child (widget_class, GltWorkbench, views_popover);
}

static void
glt_workbench_init (GltWorkbench *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));

  self->views = g_ptr_array_new_with_free_func (g_object_unref);
  self->name_to_view = g_hash_table_new (g_str_hash, g_str_equal);

  g_object_bind_property (self->views_popover,
                          "current-view",
                          self,
                          "current-view",
                          G_BINDING_BIDIRECTIONAL);
}

GltWorkbench *
glt_workbench_new (GltApplication *application)
{
  return g_object_new (GLT_TYPE_WORKBENCH,
                       "application", GTK_APPLICATION (application),
                       NULL);
}


void
glt_workbench_show_view (GltWorkbench *self,
                         GltView      *view)
{
  g_return_if_fail (GLT_IS_WORKBENCH (self));
  g_return_if_fail (GLT_IS_VIEW (view));

  if (g_set_object (&self->current_view, view))
    {
      g_clear_pointer (&self->icon_binding, g_binding_unbind);
      self->icon_binding = g_object_bind_property (view,
                                                   "icon",
                                                   self->view_icon,
                                                   "gicon",
                                                   G_BINDING_DEFAULT | G_BINDING_SYNC_CREATE);

      gtk_stack_set_visible_child (GTK_STACK (self->main_stack), GTK_WIDGET (view));

      g_message ("showing view %s", glt_view_get_title (view));

      g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_CURRENT_VIEW]);
    }
}

void
glt_workbench_show_view_from_name (GltWorkbench *self,
                                   const gchar  *name)
{
  g_return_if_fail (GLT_IS_WORKBENCH (self));

  if (!g_hash_table_contains (self->name_to_view, name))
    return;

  glt_workbench_show_view (self, g_hash_table_lookup (self->name_to_view, name));
}
