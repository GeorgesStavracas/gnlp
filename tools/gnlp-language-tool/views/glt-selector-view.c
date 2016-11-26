/* glt-selector-view.c
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

#include "glt-application.h"
#include "glt-selector-row.h"
#include "glt-selector-view.h"
#include "glt-view.h"
#include "glt-workbench.h"
#include "gnlp.h"

#include <glib/gi18n.h>

#define GLT_SELECTOR_VIEW_NAME  "selector"
#define GLT_SELECTOR_VIEW_TITLE _("Language Selector")


struct _GltSelectorView
{
  GtkBox              parent;

  GtkWidget          *listbox;

  GCancellable       *cancellable;

  GIcon              *icon;
};

static void          glt_view_iface_init                         (GltViewInterface   *iface);

G_DEFINE_TYPE_WITH_CODE (GltSelectorView, glt_selector_view, GTK_TYPE_BOX,
                         G_IMPLEMENT_INTERFACE (GLT_TYPE_VIEW, glt_view_iface_init))

enum {
  PROP_0,
  PROP_ICON,
  PROP_NAME,
  PROP_TITLE,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

/*
 * Auxiliary methods
 */
static void
header_func (GtkListBoxRow *row,
             GtkListBoxRow *before,
             gpointer       user_data)
{
  GtkWidget *separator;

  if (!before)
    return;

  separator = gtk_separator_new (GTK_ORIENTATION_HORIZONTAL);
  gtk_widget_show (separator);

  gtk_list_box_row_set_header (row, separator);
}

/*
 * Callbacks
 */
static void
languages_ready_cb (GObject      *source_object,
                    GAsyncResult *result,
                    gpointer      user_data)
{
  GltSelectorView *self;
  GPtrArray *languages;
  GError *error;
  guint i;

  error = NULL;
  self = user_data;
  languages = gnlp_get_available_languages_finish (result, &error);

  if (error)
    {
      g_warning ("Error fetching available languages: %s", error->message);
      g_clear_error (&error);
      return;
    }

  g_ptr_array_sort (languages, (GCompareFunc) gnlp_language_compare);

  for (i = 0; i < languages->len; i++)
    {
      GnlpLanguage *language;
      GtkWidget *row;

      language = g_ptr_array_index (languages, i);
      row = glt_selector_row_new (language);

      gtk_container_add (GTK_CONTAINER (self->listbox), row);
    }
}

static void
language_row_activated (GltSelectorView *self,
                        GltSelectorRow  *row)
{
  GltApplication *application;
  GltWorkbench *workbench;
  GnlpLanguage *language;

  application = glt_application_get_default ();
  workbench = glt_application_get_workbench (application);
  language = glt_selector_row_get_language (row);

  glt_application_set_language (application, language);
  glt_workbench_show_view_from_name (workbench, "language");
}

static const GIcon*
glt_selector_view_get_icon (GltView *view)
{
  return GLT_SELECTOR_VIEW (view)->icon;
}

static const gchar*
glt_selector_view_get_name (GltView *view)
{
  return GLT_SELECTOR_VIEW_NAME;
}

static const gchar*
glt_selector_view_get_title (GltView *view)
{
  return GLT_SELECTOR_VIEW_TITLE;
}

static void
glt_view_iface_init (GltViewInterface *iface)
{
  iface->get_icon = glt_selector_view_get_icon;
  iface->get_name = glt_selector_view_get_name;
  iface->get_title = glt_selector_view_get_title;
}

static void
glt_selector_view_finalize (GObject *object)
{
  GltSelectorView *self = (GltSelectorView *)object;

  g_cancellable_cancel (self->cancellable);

  g_clear_object (&self->cancellable);
  g_clear_object (&self->icon);

  G_OBJECT_CLASS (glt_selector_view_parent_class)->finalize (object);
}

static void
glt_selector_view_get_property (GObject    *object,
                                guint       prop_id,
                                GValue     *value,
                                GParamSpec *pspec)
{
  GltSelectorView *self = GLT_SELECTOR_VIEW (object);

  switch (prop_id)
    {
    case PROP_ICON:
      g_value_set_object (value, self->icon);
      break;

    case PROP_NAME:
      g_value_set_string (value, GLT_SELECTOR_VIEW_NAME);
      break;

    case PROP_TITLE:
      g_value_set_string (value, GLT_SELECTOR_VIEW_TITLE);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
glt_selector_view_set_property (GObject      *object,
                                guint         prop_id,
                                const GValue *value,
                                GParamSpec   *pspec)
{
  G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
}

static void
glt_selector_view_class_init (GltSelectorViewClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->finalize = glt_selector_view_finalize;
  object_class->get_property = glt_selector_view_get_property;
  object_class->set_property = glt_selector_view_set_property;

  g_object_class_override_property (object_class, PROP_ICON, "icon");
  g_object_class_override_property (object_class, PROP_NAME, "name");
  g_object_class_override_property (object_class, PROP_TITLE, "title");

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/glt/ui/selector-view.ui");

  gtk_widget_class_bind_template_child (widget_class, GltSelectorView, listbox);

  gtk_widget_class_bind_template_callback (widget_class, language_row_activated);
}

static void
glt_selector_view_init (GltSelectorView *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));

  self->icon = g_themed_icon_new ("view-list-symbolic");
  self->cancellable = g_cancellable_new ();

  /* Listbox functions */
  gtk_list_box_set_header_func (GTK_LIST_BOX (self->listbox), header_func, NULL, NULL);

  /* Start fetching the languages */
  gnlp_get_available_languages (self->cancellable, languages_ready_cb, self);
}
