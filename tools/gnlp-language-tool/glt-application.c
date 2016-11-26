/* glt-application.c
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
#include "glt-workbench.h"

struct _GltApplication
{
  GtkApplication      parent;

  GltWorkbench       *workbench;

  GnlpLanguage       *language;
};

G_DEFINE_TYPE (GltApplication, glt_application, GTK_TYPE_APPLICATION)

enum {
  PROP_0,
  PROP_LANGUAGE,
  N_PROPS
};

static GParamSpec *properties [N_PROPS] = { NULL, };

static GltApplication *instance = NULL;

static void
glt_application_finalize (GObject *object)
{
  GltApplication *self = (GltApplication *)object;

  G_OBJECT_CLASS (glt_application_parent_class)->finalize (object);
}

static void
glt_application_get_property (GObject    *object,
                              guint       prop_id,
                              GValue     *value,
                              GParamSpec *pspec)
{
  GltApplication *self = GLT_APPLICATION (object);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
glt_application_set_property (GObject      *object,
                              guint         prop_id,
                              const GValue *value,
                              GParamSpec   *pspec)
{
  GltApplication *self = GLT_APPLICATION (object);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
glt_application_startup (GApplication *application)
{
  GltApplication *self = GLT_APPLICATION (application);

  G_APPLICATION_CLASS (glt_application_parent_class)->startup (application);

  self->workbench = glt_workbench_new (self);
}

static void
glt_application_activate (GApplication *application)
{
  GltApplication *self = GLT_APPLICATION (application);

  G_APPLICATION_CLASS (glt_application_parent_class)->activate (application);

  gtk_window_present (GTK_WINDOW (self->workbench));
}

static void
glt_application_class_init (GltApplicationClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GApplicationClass *application_class = G_APPLICATION_CLASS (klass);

  object_class->finalize = glt_application_finalize;
  object_class->get_property = glt_application_get_property;
  object_class->set_property = glt_application_set_property;

  application_class->startup = glt_application_startup;
  application_class->activate = glt_application_activate;

  properties[PROP_LANGUAGE] = g_param_spec_object ("language",
                                                   "Language",
                                                   "The current language being edited",
                                                   GNLP_TYPE_LANGUAGE,
                                                   G_PARAM_READWRITE);

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
glt_application_init (GltApplication *self)
{
}

GltApplication*
glt_application_get_default (void)
{
  if (!instance)
    {
      instance = g_object_new (GLT_TYPE_APPLICATION,
                               "application-id", "org.gnome.Gnlp",
                               "flags", G_APPLICATION_FLAGS_NONE,
                               NULL);
    }

  return instance;
}

GltWorkbench*
glt_application_get_workbench (GltApplication *self)
{
  g_return_val_if_fail (GLT_IS_APPLICATION (self), NULL);

  return self->workbench;
}

GnlpLanguage*
glt_application_get_language (GltApplication *self)
{
  g_return_val_if_fail (GLT_IS_APPLICATION (self), NULL);

  return self->language;
}

void
glt_application_set_language (GltApplication *self,
                              GnlpLanguage   *language)
{
  g_return_if_fail (GLT_IS_APPLICATION (self));
  g_return_if_fail (GNLP_IS_LANGUAGE (language));

  if (g_set_object (&self->language, language))
    g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_LANGUAGE]);
}
