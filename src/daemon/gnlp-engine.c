/* gnlp-engine.c
 *
 * Copyright (C) 2016 Georges Basile Stavracas Neto <georges.stavracas@gmail.com>
 *
 * This file is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.
 *
 * This file is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "gnlp-engine.h"

#include <string.h>
#include <libpeas/peas.h>

struct _GnlpEngine
{
  GObject             parent;

  GHashTable         *name_to_type;
  PeasExtensionSet   *extension_set;
};

G_DEFINE_TYPE (GnlpEngine, gnlp_engine, G_TYPE_OBJECT)

static gpointer
string_copy (gconstpointer src,
             gpointer      data)
{
  return g_strdup (src);
}

static void
extension_added_cb (PeasExtensionSet *set,
                    PeasPluginInfo   *info,
                    PeasActivatable  *activatable)
{
  peas_activatable_activate (activatable);
}

static void
extension_removed_cb (PeasExtensionSet *set,
                      PeasPluginInfo   *info,
                      PeasActivatable  *activatable)
{
  peas_activatable_deactivate (activatable);
}

static void
gnlp_engine_finalize (GObject *object)
{
  GnlpEngine *self = GNLP_ENGINE (object);

  g_clear_object (&self->extension_set);
  g_clear_pointer (&self->name_to_type, g_hash_table_unref);

  G_OBJECT_CLASS (gnlp_engine_parent_class)->finalize (object);
}

static void
gnlp_engine_class_init (GnlpEngineClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gnlp_engine_finalize;
}

static void
gnlp_engine_init (GnlpEngine *self)
{
  PeasEngine *engine;
  const GList *list;
  gchar *plugin_dir;

  g_debug ("Initializing the engine...");

  /*
   * Setup the map of name -> type. A name is composed of the extension
   * operation and a language, in the form of <operation name>::<language>.
   * Therefore, the extension name should not contain '::' in it' name.
   */
  self->name_to_type = g_hash_table_new_full (g_str_hash,
                                              g_str_equal,
                                              g_free,
                                              NULL);

  /* Setup the extensions */
  engine = peas_engine_get_default ();

  /* Enable Python3 plugins */
  peas_engine_enable_loader (engine, "python3");

  /*
   * The extension set will assure we'll always have 1 instance
   * of each plugin loaded, freeing us from the burden to manage
   * that manually.
   */
  self->extension_set = peas_extension_set_new (engine,
                                                PEAS_TYPE_ACTIVATABLE,
                                                "object", self,
                                                NULL);

  peas_extension_set_foreach (self->extension_set,
                              (PeasExtensionSetForeachFunc) extension_added_cb,
                              NULL);

  g_signal_connect (self->extension_set,
                    "extension-added",
                    G_CALLBACK (extension_added_cb),
                    self);

  g_signal_connect (self->extension_set,
                    "extension-removed",
                    G_CALLBACK (extension_removed_cb),
                    self);

  /* Let Peas search for plugins in the specified directory */
  plugin_dir = g_build_filename (PACKAGE_LIB_DIR,
                                 "plugins",
                                 NULL);

  peas_engine_add_search_path (engine,
                               plugin_dir,
                               NULL);

  g_free (plugin_dir);

  /* User-installed plugins shall be detected too */
  plugin_dir = g_build_filename (g_get_home_dir (),
                                 ".local",
                                 "lib",
                                 "gnlp",
                                 "plugins",
                                 NULL);

  peas_engine_add_search_path (engine,
                               plugin_dir,
                               NULL);

  /* Embedded plugins */
  peas_engine_prepend_search_path (engine,
                                   "resource:///org/gnome/nlp/plugins",
                                   "resource:///org/gnome/nlp/plugins");

  peas_engine_rescan_plugins (engine);

  /* Load the discovered plugins */
  list = peas_engine_get_plugin_list (engine);

  for (; list != NULL; list = list->next)
    {
      g_debug ("Discovered plugin \"%s\"", peas_plugin_info_get_module_name (list->data));

      peas_engine_load_plugin (engine, list->data);
    }

  g_free (plugin_dir);
}

GnlpEngine *
gnlp_engine_new (void)
{
  return g_object_new (GNLP_TYPE_ENGINE, NULL);
}

/**
 * gnlp_engine_register_operation:
 * @self: a #GnlpEngine
 * @name: the name of the operation. It must not contain '::'.
 * @language: (nullable): the specialized language this operation applies.
 * @type: a #GType
 *
 * Registers an operation named @name for @language. When searching for
 * operations, #GnlpEngine first tries to match <operation name>::<language>,
 * then only <operation name>. Because of that, it is strictly forbiden to
 * pass '::' in @name.
 *
 * Since: 0.1.0
 */
void
gnlp_engine_register_operation (GnlpEngine  *self,
                                const gchar *name,
                                const gchar *language,
                                GType        type)
{
  gchar *extended_name;

  g_return_if_fail (GNLP_IS_ENGINE (self));
  g_return_if_fail (g_type_is_a (type, GNLP_TYPE_OPERATION));
  g_return_if_fail (strstr (name, "::") == NULL);

  extended_name = language ? g_strdup_printf ("%s::%s", name, language) : g_strdup (name);

  g_hash_table_insert (self->name_to_type, extended_name, GINT_TO_POINTER (type));

  g_free (extended_name);
}

/**
 * gnlp_engine_unregister_operation:
 * @self: a #GnlpEngine
 * @name: the name of the operation. It must not contain '::'.
 * @language: (nullable): the specialized language this operation applies.
 * @type: a #GType
 *
 * Registers an operation named @name for @language. When searching for
 * operations, #GnlpEngine first tries to match <operation name>::<language>,
 * then only <operation name>. Because of that, it is strictly forbiden to
 * pass '::' in @name.
 *
 * Since: 0.1.0
 */
void
gnlp_engine_unregister_operation (GnlpEngine *self,
                                  GType       type)
{
  GHashTableIter iter;
  gpointer key, value;

  g_return_if_fail (GNLP_IS_ENGINE (self));
  g_return_if_fail (g_type_is_a (type, GNLP_TYPE_OPERATION));

  g_hash_table_iter_init (&iter, self->name_to_type);

  while (g_hash_table_iter_next (&iter, &key, &value))
    {
      GType current_type = GPOINTER_TO_INT (value);

      /*
       * The plugin may have registered the same type for more
       * than one name or language. Thus, we cannot stop iterating
       * after we find an occurrence.
       */
      if (type == current_type)
        g_hash_table_iter_remove (&iter);
    }
}

/**
 * gnlp_engine_list_operations:
 * @self: a #GnlpEngine
 * @language: (nullable): the desired language.
 *
 * List the currently available operations. If @language is set, it fetches
 * the list of operations for that specific language, i.e. ignores generic
 * operations.
 *
 * Returns: (element-type utf8) (transfer full): the list of the currently
 * registered operations. Free the list with g_list_free_full() and pass
 * g_free() to free the strings.
 *
 * Since: 0.1.0
 */
GList*
gnlp_engine_list_operations (GnlpEngine  *self,
                             const gchar *language)
{
  GList *keys;

  g_return_val_if_fail (GNLP_IS_ENGINE (self), NULL);

  keys = g_hash_table_get_keys (self->name_to_type);

  if (language)
    {
      GList *aux, *retval;
      gchar *language_suffix;

      retval = NULL;
      language_suffix = g_strdup_printf ("::%s", language);

      for (aux = keys; aux != NULL; aux = aux->next)
        {
          if (g_str_has_suffix (aux->data, language_suffix))
            {
              gchar **split_name;

              split_name = g_strsplit (aux->data, "::", 2);
              retval = g_list_append (retval, g_strdup (split_name[0]));

              g_strfreev (split_name);
            }
        }

      g_list_free (keys);

      return retval;
    }
  else
    {
      return g_list_copy_deep (keys, string_copy, NULL);
    }
}

/**
 * gnlp_engine_create_operation:
 * @self: a #GnlpEngine
 * @name: the name of the extension
 * @language: (nullable): the language for the given extension
 *
 * Creates an instance of extension named @name for the @language
 * specified. If no extension is registered for this specific
 * language, create a generic extension.
 *
 * Returns: (transfer full): a #GnlpOperation
 */
GnlpOperation*
gnlp_engine_create_operation (GnlpEngine  *self,
                              const gchar *name,
                              const gchar *language)
{
  GnlpOperation *operation;
  GType type;
  gchar *extended_name;

  operation = NULL;
  extended_name = language ? g_strdup_printf ("%s::%s", name, language) : g_strdup (name);

  if (g_hash_table_contains (self->name_to_type, extended_name))
    {
      /* Search for the extension for the specific language first */
      type = GPOINTER_TO_INT (g_hash_table_lookup (self->name_to_type, extended_name));
      operation = g_object_new (type, NULL);
    }
  else if (language && g_hash_table_contains (self->name_to_type, name))
    {
      /* If we didn't find any extension for that specific language,
       * search for a generic extension
       */
      type = GPOINTER_TO_INT (g_hash_table_lookup (self->name_to_type, name));
      operation = g_object_new (type, NULL);
    }

  g_free (extended_name);

  return operation;
}
