/* test-context.c
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

#include <glib.h>

typedef enum
{
  ACQUIRE_CONTEXT,
  LIST_OPERATIONS
} TestOperation;

typedef struct
{
  GMainLoop     *mainloop;
  GnlpContext   *context;
  TestOperation  operation;
} Context;

static void
list_operations_cb (GObject      *source,
                    GAsyncResult *result,
                    gpointer      user_data)
{
  Context *data = user_data;
  GError *error = NULL;

  g_main_loop_quit (data->mainloop);
  g_clear_object (&data->context);
  g_clear_pointer (&data, g_free);
}

static void
context_acquire_cb (GObject      *source,
                    GAsyncResult *result,
                    gpointer      user_data)
{
  Context *data = user_data;
  GError *error = NULL;

  data->context = gnlp_context_new_finish (result, &error);

  g_assert (!error);

  if (error)
    {
      g_warning ("Error acquiring context: %s", error->message);
      g_clear_error (&error);
    }

  /* Run the desired operation here */
  switch (data->operation)
    {
    case ACQUIRE_CONTEXT:
      g_main_loop_quit (data->mainloop);
      g_clear_object (&data->context);
      g_clear_pointer (&data, g_free);
      break;

    case LIST_OPERATIONS:
      gnlp_context_list_operations (data->context,
                                    NULL,
                                    NULL,
                                    list_operations_cb,
                                    data);

      gnlp_context_list_operations (data->context,
                                    "en_US",
                                    NULL,
                                    list_operations_cb,
                                    data);
      break;

    default:
      g_assert_not_reached ();
    }
}

static void
run_operation (TestOperation operation)
{
  Context *data;
  GMainLoop *mainloop;

  mainloop = g_main_loop_new (NULL, FALSE);

  data = g_new0 (Context, 1);
  data->mainloop = mainloop;
  data->operation = operation;

  /* Starts retrieving the GnlpContext */
  gnlp_context_new ("test-context", NULL, context_acquire_cb, data);

  g_main_loop_run (mainloop);

  g_clear_pointer (&mainloop, g_main_loop_unref);
}

static void
acquire_context (void)
{
  run_operation (ACQUIRE_CONTEXT);
}

static void
list_operations (void)
{
  run_operation (LIST_OPERATIONS);
}

gint
main (gint   argc,
      gchar *argv[])
{
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/context/acquire_context", acquire_context);
  g_test_add_func ("/context/list_operations", list_operations);

  return g_test_run ();
}
