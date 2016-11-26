/* gnlp-language.c
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

#define G_LOG_DOMAIN "Language"

#include "gnlp.h"
#include "gnlp-language.h"
#include "gnome-languages.h"

#include <string.h>
#include <locale.h>

struct _GnlpLanguage
{
  GObject             parent;

  gchar              *name;
  gchar              *locale;
  gchar              *path;

  gint                score;

  GHashTable         *quirks;
};

G_DEFINE_TYPE (GnlpLanguage, gnlp_language, G_TYPE_OBJECT)

enum {
  PROP_0,
  PROP_LOCALE,
  PROP_NAME,
  PROP_PATH,
  N_PROPS
};

static GParamSpec *properties[N_PROPS] = { NULL, };

static void
check_supports_listening_at_path (GnlpLanguage *self,
                                  const gchar  *path,
                                  const gchar  *locale_name)
{
  g_autoptr(GFile) file = NULL;
  g_autofree gchar *built_path = NULL;
  g_autofree gchar *folder_name = NULL;
  gboolean valid;
  gsize i;

  const gchar* required_files[] = {
    "acoustic_model",
    "config",
    "dict",
    "tiedlist",
    "macros"
  };

  /* If we already have a language quirk, no need to check again */
  if (gnlp_language_has_quirk (self, GNLP_LANGUAGE_QUIRK_LISTEN))
    return;

  folder_name = g_strdup_printf ("gnlp-%d", GNLP_MAJOR_VERSION);
  built_path = g_build_filename (path,
                                 folder_name,
                                 locale_name,
                                 NULL);

  /* First check if the directory itself exists */
  file = g_file_new_for_path (built_path);
  valid = g_file_query_exists (file, NULL);

  if (!valid)
    return;

  /* The folder exists, now check if all the required files are there */
  for (i = 0; i < G_N_ELEMENTS (required_files); i++)
    {
      g_autofree gchar *filepath;

      filepath = g_build_filename (built_path, required_files[i], NULL);
      file = g_file_new_for_path (filepath);

      valid = valid && g_file_query_exists (file, NULL);

      if (!valid)
        return;
    }

  /* The path is valid, add the quirk */
  self->path = g_strdup (built_path);
  gnlp_language_add_quirk (self, GNLP_LANGUAGE_QUIRK_LISTEN, GNLP_QUIRK_QUALITY_AVAILABLE);
}

static void
check_language_quirks (GnlpLanguage *self)
{
  const gchar* const *system_data_dirs;
  GStrv split_name;
  gchar *locale;
  gint i;

  system_data_dirs = g_get_system_data_dirs ();
  split_name = g_strsplit (self->locale, ".", -1);
  locale = g_utf8_strdown (split_name[0], -1);

  check_supports_listening_at_path (self, g_get_user_data_dir (), split_name[0]);
  //check_supports_speaking_at_path (self, g_get_user_data_dir (), locale);

  /* Nothing found on user dir, let's try the system one */
  for (i = 0; system_data_dirs[i]; i++)
    {
      check_supports_listening_at_path (self, system_data_dirs[i], split_name[0]);
      //check_supports_speaking_at_path (self, system_data_dirs[i], locale);
    }

  g_clear_pointer (&split_name, g_strfreev);
  g_clear_pointer (&locale, g_free);
}

static void
gnlp_language_constructed (GObject *object)
{
  GnlpLanguage *self = GNLP_LANGUAGE (object);

  check_language_quirks (self);

  G_OBJECT_CLASS (gnlp_language_parent_class)->constructed (object);
}

static void
gnlp_language_finalize (GObject *object)
{
  GnlpLanguage *self = (GnlpLanguage *)object;

  g_clear_pointer (&self->quirks, g_hash_table_destroy);

  g_free (self->locale);
  g_free (self->name);
  g_free (self->path);

  G_OBJECT_CLASS (gnlp_language_parent_class)->finalize (object);
}

static void
gnlp_language_get_property (GObject    *object,
                            guint       prop_id,
                            GValue     *value,
                            GParamSpec *pspec)
{
  GnlpLanguage *self = GNLP_LANGUAGE (object);

  switch (prop_id)
    {
    case PROP_LOCALE:
      g_value_set_string (value, self->locale);
      break;

    case PROP_NAME:
      g_value_set_string (value, self->name);
      break;

    case PROP_PATH:
      g_value_set_string (value, self->path);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gnlp_language_set_property (GObject      *object,
                            guint         prop_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
  GnlpLanguage *self = GNLP_LANGUAGE (object);

  switch (prop_id)
    {
    case PROP_LOCALE:
      self->locale = g_value_dup_string (value);
      self->name = gnome_get_language_from_locale (self->locale, NULL);
      g_object_notify_by_pspec (object, properties[PROP_LOCALE]);
      g_object_notify_by_pspec (object, properties[PROP_NAME]);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gnlp_language_class_init (GnlpLanguageClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gnlp_language_finalize;
  object_class->constructed = gnlp_language_constructed;
  object_class->get_property = gnlp_language_get_property;
  object_class->set_property = gnlp_language_set_property;

  /**
   * GnlpLanguage:locale:
   *
   * The current locale id of the language Gnlp is working with.
   *
   * Since: 0.1.0
   */
  properties[PROP_LOCALE] = g_param_spec_string ("locale",
                                                 "Locale",
                                                 "The locale id of the language",
                                                 NULL,
                                                 G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

  /**
   * GnlpLanguage:name:
   *
   * The user-visible name of the language.
   *
   * Since: 0.1.0
   */
  properties[PROP_NAME] = g_param_spec_string ("name",
                                               "Name of the language",
                                               "The user-friendly name of the language",
                                               NULL,
                                               G_PARAM_READABLE);

  /**
   * GnlpLanguage:path:
   *
   * The path to the configuration files for Gnlp.
   *
   * Since: 0.1.0
   */
  properties[PROP_PATH] = g_param_spec_string ("path",
                                               "Path",
                                               "Path to the language files",
                                               NULL,
                                               G_PARAM_READABLE);

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
gnlp_language_init (GnlpLanguage *self)
{
  self->quirks = g_hash_table_new (g_direct_hash, g_direct_equal);
}

/**
 * gnlp_language_new:
 * @locale: the locale id of the language
 *
 * Creates a new language handler object.
 *
 * Returns: (transfer full): a #GnlpLanguage.
 *
 * Since: 0.1.0
 */
GnlpLanguage*
gnlp_language_new (const gchar *locale)
{
  return g_object_new (GNLP_TYPE_LANGUAGE,
                       "locale", locale,
                       NULL);
}

/**
 * gnlp_language_get_locale:
 * @self: a #GnlpLanguage
 *
 * Retrieves the locale of @self.
 *
 * Returns: (transfer none): the locale name.
 *
 * Since: 0.1.0
 */
const gchar*
gnlp_language_get_locale (GnlpLanguage *self)
{
  g_return_val_if_fail (GNLP_IS_LANGUAGE (self), NULL);

  return self->locale;
}

/**
 * gnlp_language_get_name:
 * @self: a #GnlpLanguage
 *
 * Retrieves the user-visible name of the language represented by @self.
 *
 * Returns: (transfer none)(nullable): a string.
 *
 * Since: 0.1.0
 */
const gchar*
gnlp_language_get_name (GnlpLanguage *self)
{
  g_return_val_if_fail (GNLP_IS_LANGUAGE (self), NULL);

  return self->name;
}

/**
 * gnlp_language_get_path:
 * @self: a #GnlpLanguage
 *
 * Retrieves the path to the Gnlp configuration files of @self.
 *
 * Returns: (transfer none)(nullable): the path to the configuration files,
 * or %NULL.
 *
 * Since: 0.1.0
 */
const gchar*
gnlp_language_get_path (GnlpLanguage *self)
{
  g_return_val_if_fail (GNLP_IS_LANGUAGE (self), NULL);

  return self->path;
}

/**
 * gnlp_language_has_quirk:
 * @self: a #GnlpLanguage
 * @quirk: a #GnlpLanguageQuirk
 *
 * Checks whether @self has a @quirk set.
 *
 * Returns: %TRUE is @self has @quirk, %FALSE otherwise.
 *
 * Since: 0.1.0
 */
gboolean
gnlp_language_has_quirk (GnlpLanguage      *self,
                         GnlpLanguageQuirk  quirk)
{
  g_return_val_if_fail (GNLP_IS_LANGUAGE (self), FALSE);

  return g_hash_table_contains (self->quirks, GINT_TO_POINTER (quirk));
}

/**
 * gnlp_language_get_quirk_quality:
 * @self: a #GnlpLanguage
 * @quirk: a #GnlpLanguageQuirk
 *
 * Retrieves the quality of the the language's @quirk.
 *
 * Returns: the #GnlpQuirkQuality of @quirk, or %GNLP_QUIRK_QUALITY_NONE if
 * it's not set.
 *
 * Since: 0.1.0
 */
GnlpQuirkQuality
gnlp_language_get_quirk_quality (GnlpLanguage      *self,
                                 GnlpLanguageQuirk  quirk)
{
  g_return_val_if_fail (GNLP_IS_LANGUAGE (self), GNLP_QUIRK_QUALITY_NONE);

  if (!gnlp_language_has_quirk (self, quirk))
    return GNLP_QUIRK_QUALITY_NONE;

  return (GnlpQuirkQuality) g_hash_table_lookup (self->quirks, GINT_TO_POINTER (quirk));
}

/**
 * gnlp_language_add_quirk:
 * @self: a #GnlpLanguage
 * @quirk: a #GnlpLanguageQuirk
 * @quality: the quality of @quirk
 *
 * Adds @quirk to the language. If there already is @quirk, the
 * value is updated.
 *
 * Since: 0.1.0
 */
void
gnlp_language_add_quirk (GnlpLanguage      *self,
                         GnlpLanguageQuirk  quirk,
                         GnlpQuirkQuality   quality)
{
  g_return_if_fail (GNLP_IS_LANGUAGE (self));

  self->score += quality;

  g_hash_table_insert (self->quirks, GINT_TO_POINTER (quirk), GINT_TO_POINTER (quality));
}

/**
 * gnlp_language_remove_quirk:
 * @self: a @GnlpLanguage
 * @quirk: a #GnlpLanguageQuirk
 *
 * Removes @quirk from @self. If @self doesn't have @quirk,
 * nothing happens.
 *
 * Since: 0.1.0
 */
void
gnlp_language_remove_quirk (GnlpLanguage      *self,
                            GnlpLanguageQuirk  quirk)
{
  g_return_if_fail (GNLP_IS_LANGUAGE (self));

  self->score -= gnlp_language_get_quirk_quality (self, quirk);

  g_hash_table_remove (self->quirks, GINT_TO_POINTER (quirk));
}

/**
 * gnlp_language_get_score:
 * @self: a #GnlpLanguage
 *
 * Retrieves the score of @self. The more feature-complete a language is,
 * the higher the score.
 *
 * Returns: the score of @self.
 *
 * Since: 0.1.0
 */
gint
gnlp_language_get_score (GnlpLanguage *self)
{
  g_return_val_if_fail (GNLP_IS_LANGUAGE (self), 0);

  return self->score;
}

/**
 * gnlp_language_compare:
 * @a: (nullable): a #GnlpLanguage
 * @b: (nullable): a #GnlpLanguage
 *
 * Compare @a and @b by score, then by language name.
 *
 * Returns: 1 if @a comes before @b, 0 if they're equal and -1 if
 * @b comes before @a.
 *
 * Since: 0.1.0
 */
gint
gnlp_language_compare (GnlpLanguage **a,
                       GnlpLanguage **b)
{
  if (!a || !*a)
    return -1;
  else if (!b || !*b)
    return 1;
  else if ((*a)->score != (*b)->score)
    return (*b)->score - (*a)->score;
  else
    return g_strcmp0 ((*a)->name, (*b)->name);
}
