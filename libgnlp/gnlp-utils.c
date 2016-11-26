/* gnlp-utils.c
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

#include "gnlp-utils.h"
#include "gnlp-language.h"
#include "gnome-languages.h"


static void
get_languages_in_thread_cb (GTask        *task,
                            gpointer      source_object,
                            gpointer      task_data,
                            GCancellable *cancellable)
{
  GPtrArray *result;
  GError *error;

  error = NULL;
  result = gnlp_get_available_languages_sync (cancellable, &error);

  if (error)
    {
      g_task_return_error (task, error);
      return;
    }

  g_task_return_pointer (task,
                         g_ptr_array_ref (result),
                         (GDestroyNotify) g_ptr_array_unref);
}

/**
 * gnlp_get_available_languages:
 * @cancellable: (nullable): a #GCancellable
 * @callback: (scope async): a #GAsyncReadyCallback to call when the initialization is done
 * @user_data: (closure): data to pass to the callback function
 *
 * Starts retrieving the list of languages
 *
 * Since: 0.1.0
 */
void
gnlp_get_available_languages (GCancellable        *cancellable,
                              GAsyncReadyCallback  callback,
                              gpointer             user_data)
{
  GTask *task;

  task = g_task_new (NULL, cancellable, callback, user_data);
  g_task_run_in_thread (task, get_languages_in_thread_cb);
}

/**
 * gnlp_get_available_languages_finish:
 * @result: a #GAsyncResult
 * @error: (nullable): return location for a #GError, or %NULL
 *
 * Finishes the operation started by @gnlp_get_available_languages.
 *
 * Returns: (transfer full)(nullable)(element-type #GnlpLanguage): a #GPtrArray, or %NULL
 *
 * Since: 0.1.0
 */
GPtrArray*
gnlp_get_available_languages_finish (GAsyncResult  *result,
                                     GError       **error)
{
  g_return_val_if_fail (G_IS_TASK (result), NULL);
  g_return_val_if_fail (g_task_is_valid (result, NULL), NULL);
  g_return_val_if_fail (!error || !*error, NULL);

  return g_task_propagate_pointer (G_TASK (result), error);
}

/**
 * gnlp_client_get_available_languages:
 * @cancellable: (nullable): a #GCancellable
 * @error: (nullable): return location for a #GError, or %NULL
 *
 * Retrieves the list of available languages.
 *
 * Returns: (transfer container): a #GPtrArray that may be empty or not.
 * Free with @g_ptr_array_free.
 *
 * Since: 0.1.0
 */
GPtrArray*
gnlp_get_available_languages_sync (GCancellable  *cancellable,
                                   GError       **error)
{
  GPtrArray *languages;
  gboolean cancelled;
  gchar **locales;
  guint i;

  cancelled = FALSE;
  languages = g_ptr_array_new_with_free_func (g_object_unref);
  locales = gnome_get_all_locales ();

  for (i = 0; locales[i]; i++)
    {
      if (g_cancellable_set_error_if_cancelled (cancellable, error))
        {
          cancelled = TRUE;
          break;
        }

      g_ptr_array_add (languages, gnlp_language_new (locales[i]));
    }

  g_strfreev (locales);

  if (cancelled)
    g_clear_pointer (&languages, g_ptr_array_unref);

  return languages;
}
