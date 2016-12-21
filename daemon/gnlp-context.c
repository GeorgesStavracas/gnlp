/* gnlp-context.c
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

#define G_LOG_DOMAIN "Context"

#include "gnlp-context.h"

#include <glib/gi18n.h>
#include <gio/gio.h>

#define DEFAULT_LOCALE "en_US.utf-8"

struct _GnlpContext
{
  GObject             parent;

  GDBusProxy         *localed;

  GnlpDialogState    *dialog_state;

  GnlpLanguage       *language;
  gchar              *language_name;
  gboolean            language_set : 1;
};

G_DEFINE_TYPE (GnlpContext, gnlp_context, G_TYPE_OBJECT)

enum {
  PROP_0,
  PROP_LANGUAGE,
  PROP_LANGUAGE_NAME,
  N_PROPS
};

static GParamSpec *properties [N_PROPS] = { NULL, };

static void
update_language (GnlpContext *self)
{
  GVariant *localed_props;
  const gchar **strv;
  gchar *language;
  gint i;

  language = NULL;
  localed_props = g_dbus_proxy_get_cached_property (self->localed, "Locale");

  if (!localed_props)
    return;

  strv = g_variant_get_strv (localed_props, NULL);

  for (i = 0; strv[i]; i++)
    {
      const gchar *lang;

      if (!g_str_has_prefix (strv[i], "LANG="))
        continue;

      lang = g_utf8_strlen ("LANG=", -1) + strv[i];
      language = g_strdup (lang);
    }

  /* Language changed */
  if (g_strcmp0 (language, self->language_name) != 0)
    {
      g_clear_pointer (&self->language_name, g_free);
      g_clear_object (&self->language);

      g_debug ("Current language is %s", language);

      self->language_name = language;
      self->language = gnlp_language_new (language);

      g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_LANGUAGE]);
      g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_LANGUAGE_NAME]);
    }

  g_clear_pointer (&localed_props, g_variant_unref);
}

static void
setup_language_proxy (GnlpContext *self)
{
  GDBusConnection *bus;
  GError *error;

  error = NULL;
  bus = g_bus_get_sync (G_BUS_TYPE_SYSTEM, NULL, NULL);

  self->localed = g_dbus_proxy_new_sync (bus,
                                         G_DBUS_PROXY_FLAGS_GET_INVALIDATED_PROPERTIES,
                                         NULL,
                                         "org.freedesktop.locale1",
                                         "/org/freedesktop/locale1",
                                         "org.freedesktop.locale1",
                                         NULL,
                                         &error);

  if (error)
    {
      g_critical ("Error loading language: %s", error->message);
      g_clear_error (&error);
      goto out;
    }

  /* Setup the language */
  update_language (self);

  g_signal_connect_swapped (self->localed,
                            "g-properties-changed",
                            G_CALLBACK (update_language),
                            self);

out:
  g_clear_object (&bus);
}

static void
gnlp_context_finalize (GObject *object)
{
  GnlpContext *self = (GnlpContext *)object;

  g_clear_pointer (&self->language, g_free);

  G_OBJECT_CLASS (gnlp_context_parent_class)->finalize (object);
}

static void
gnlp_context_get_property (GObject    *object,
                            guint       prop_id,
                            GValue     *value,
                            GParamSpec *pspec)
{
  GnlpContext *self = GNLP_CONTEXT (object);

  switch (prop_id)
    {
    case PROP_LANGUAGE:
      g_value_set_object (value, self->language);
      break;

    case PROP_LANGUAGE_NAME:
      g_value_set_string (value, self->language_name);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gnlp_context_set_property (GObject      *object,
                            guint         prop_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
  switch (prop_id)
    {
    case PROP_LANGUAGE_NAME:
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gnlp_context_class_init (GnlpContextClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gnlp_context_finalize;
  object_class->get_property = gnlp_context_get_property;
  object_class->set_property = gnlp_context_set_property;

  properties[PROP_LANGUAGE] = g_param_spec_object ("language",
                                                   "Language",
                                                   "The current language",
                                                   GNLP_TYPE_LANGUAGE,
                                                   G_PARAM_READABLE);

  properties[PROP_LANGUAGE_NAME] = g_param_spec_string ("language-name",
                                                        "Language name",
                                                        "The name of the current language",
                                                        DEFAULT_LOCALE,
                                                        G_PARAM_READWRITE);

  g_object_class_install_properties (object_class, G_N_ELEMENTS (properties), properties);
}

static void
gnlp_context_init (GnlpContext *self)
{
  self->dialog_state = gnlp_dialog_state_new ();

  self->language_name = g_strdup (DEFAULT_LOCALE);
  self->language = gnlp_language_new (DEFAULT_LOCALE);

  setup_language_proxy (self);
}

GnlpContext *
gnlp_context_new (void)
{
  return g_object_new (GNLP_TYPE_CONTEXT, NULL);
}

GnlpLanguage*
gnlp_context_get_language (GnlpContext *self)
{
  g_return_val_if_fail (GNLP_IS_CONTEXT (self), NULL);

  return self->language;
}

/**
 * gnlp_context_get_language_name:
 *
 * Sets the language name.
 */
void
gnlp_context_set_language (GnlpContext *self,
                            GnlpLanguage *language)
{
  g_return_if_fail (GNLP_IS_CONTEXT (self));

  if (g_set_object (&self->language, language))
    g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_LANGUAGE]);
}

GnlpDialogState*
gnlp_context_get_dialog_state (GnlpContext *self)
{
  g_return_val_if_fail (GNLP_IS_CONTEXT (self), NULL);

  return self->dialog_state;
}
