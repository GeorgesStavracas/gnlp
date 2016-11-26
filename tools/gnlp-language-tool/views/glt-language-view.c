/* glt-language-view.c
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
#include "glt-view.h"

#include <glib/gi18n.h>

#define GLT_LANGUAGE_VIEW_NAME  "language"
#define GLT_LANGUAGE_VIEW_TITLE _("Language")

struct _GltLanguageView
{
  GtkBox              parent;

  GIcon              *icon;
};

static void          glt_view_iface_init                         (GltViewInterface   *iface);

G_DEFINE_TYPE_WITH_CODE (GltLanguageView, glt_language_view, GTK_TYPE_BOX,
                         G_IMPLEMENT_INTERFACE (GLT_TYPE_VIEW, glt_view_iface_init));

enum {
  PROP_0,
  PROP_ICON,
  PROP_NAME,
  PROP_TITLE,
  N_PROPS
};

static GParamSpec *properties [N_PROPS] = { NULL, };

static const GIcon*
glt_language_view_get_icon (GltView *view)
{
  return GLT_LANGUAGE_VIEW (view)->icon;
}

static const gchar*
glt_language_view_get_name (GltView *view)
{
  return GLT_LANGUAGE_VIEW_NAME;
}

static const gchar*
glt_language_view_get_title (GltView *view)
{
  return GLT_LANGUAGE_VIEW_TITLE;
}

static void
glt_view_iface_init (GltViewInterface *iface)
{
  iface->get_icon = glt_language_view_get_icon;
  iface->get_name = glt_language_view_get_name;
  iface->get_title = glt_language_view_get_title;
}

static void
glt_language_view_finalize (GObject *object)
{
  GltLanguageView *self = (GltLanguageView *)object;

  G_OBJECT_CLASS (glt_language_view_parent_class)->finalize (object);
}

static void
glt_language_view_get_property (GObject    *object,
                                 guint       prop_id,
                                 GValue     *value,
                                 GParamSpec *pspec)
{
  GltLanguageView *self = GLT_LANGUAGE_VIEW (object);

  switch (prop_id)
    {
    case PROP_ICON:
      g_value_set_object (value, self->icon);
      break;

    case PROP_NAME:
      g_value_set_string (value, GLT_LANGUAGE_VIEW_NAME);
      break;

    case PROP_TITLE:
      g_value_set_string (value, GLT_LANGUAGE_VIEW_TITLE);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
glt_language_view_set_property (GObject      *object,
                                 guint         prop_id,
                                 const GValue *value,
                                 GParamSpec   *pspec)
{
  G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
}

static void
glt_language_view_class_init (GltLanguageViewClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->finalize = glt_language_view_finalize;
  object_class->get_property = glt_language_view_get_property;
  object_class->set_property = glt_language_view_set_property;

  g_object_class_override_property (object_class, PROP_ICON, "icon");
  g_object_class_override_property (object_class, PROP_NAME, "name");
  g_object_class_override_property (object_class, PROP_TITLE, "title");

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/glt/ui/language-view.ui");
}

static void
glt_language_view_init (GltLanguageView *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));

  self->icon = g_themed_icon_new ("tools-check-spelling-symbolic");
}
