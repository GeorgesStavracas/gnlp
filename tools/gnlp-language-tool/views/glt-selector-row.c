/* glt-selector-row.c
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

#include "glt-selector-row.h"

struct _GltSelectorRow
{
  GtkListBoxRow       parent;

  GtkWidget          *language_name_label;
  GtkWidget          *listen_support_label;
  GtkWidget          *locale_label;
  GtkWidget          *speak_support_label;

  GnlpLanguage       *language;
};

G_DEFINE_TYPE (GltSelectorRow, glt_selector_row, GTK_TYPE_LIST_BOX_ROW)

enum {
  PROP_0,
  PROP_LANGUAGE,
  N_PROPS
};

static GParamSpec *properties [N_PROPS] = { NULL, };

static void
glt_selector_row_finalize (GObject *object)
{
  GltSelectorRow *self = (GltSelectorRow *)object;

  g_clear_object (&self->language);

  G_OBJECT_CLASS (glt_selector_row_parent_class)->finalize (object);
}

static void
glt_selector_row_get_property (GObject    *object,
                               guint       prop_id,
                               GValue     *value,
                               GParamSpec *pspec)
{
  GltSelectorRow *self = GLT_SELECTOR_ROW (object);

  switch (prop_id)
    {
    case PROP_LANGUAGE:
      g_value_set_object (value, self->language);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
glt_selector_row_set_property (GObject      *object,
                               guint         prop_id,
                               const GValue *value,
                               GParamSpec   *pspec)
{
  GltSelectorRow *self = GLT_SELECTOR_ROW (object);

  switch (prop_id)
    {
    case PROP_LANGUAGE:
      self->language = g_value_dup_object (value);

      gtk_label_set_label (GTK_LABEL (self->language_name_label), gnlp_language_get_name (self->language));
      gtk_label_set_label (GTK_LABEL (self->locale_label), gnlp_language_get_locale (self->language));
      gtk_widget_set_visible (self->listen_support_label, gnlp_language_has_quirk (self->language, GNLP_LANGUAGE_QUIRK_LISTEN));
      gtk_widget_set_visible (self->speak_support_label, gnlp_language_has_quirk (self->language, GNLP_LANGUAGE_QUIRK_SPEAK));

      g_object_notify_by_pspec (object, properties[PROP_LANGUAGE]);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
glt_selector_row_class_init (GltSelectorRowClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->finalize = glt_selector_row_finalize;
  object_class->get_property = glt_selector_row_get_property;
  object_class->set_property = glt_selector_row_set_property;

  properties[PROP_LANGUAGE] = g_param_spec_object ("language",
                                                   "",
                                                   "",
                                                   GNLP_TYPE_LANGUAGE,
                                                   G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

  g_object_class_install_properties (object_class, N_PROPS, properties);

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/glt/ui/selector-row.ui");

  gtk_widget_class_bind_template_child (widget_class, GltSelectorRow, language_name_label);
  gtk_widget_class_bind_template_child (widget_class, GltSelectorRow, listen_support_label);
  gtk_widget_class_bind_template_child (widget_class, GltSelectorRow, locale_label);
  gtk_widget_class_bind_template_child (widget_class, GltSelectorRow, speak_support_label);
}

static void
glt_selector_row_init (GltSelectorRow *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}


GtkWidget*
glt_selector_row_new (GnlpLanguage *language)
{
  return g_object_new (GLT_TYPE_SELECTOR_ROW,
                       "language", language,
                       NULL);
}

GnlpLanguage*
glt_selector_row_get_language (GltSelectorRow *self)
{
  g_return_val_if_fail (GLT_IS_SELECTOR_ROW (self), NULL);

  return self->language;
}
