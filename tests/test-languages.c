/* test-languages.c
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

#include "gnlp.h"

/*************************************************************************************************/

static void
languages_list_avaliables (void)
{
  GPtrArray *languages;
  guint i;

  languages = gnlp_get_available_languages_sync (NULL, NULL);

  g_assert_nonnull (languages);

  g_clear_pointer (&languages, g_ptr_array_unref);
}

gint
main (gint   argc,
      gchar *argv[])
{
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/languages/list_available", languages_list_avaliables);

  return g_test_run ();
}
