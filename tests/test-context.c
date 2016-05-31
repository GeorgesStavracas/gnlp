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
  LIST_OPERATIONS,
  EVENT_PARSER
} TestOperation;

typedef struct
{
  GnlpContext   *context;
  TestOperation  operation;
} Context;

static void
real_event_parser (Context *data)
{
  GnlpOperation *operation;
  GError *error = NULL;

  operation = gnlp_context_create_operation_sync (data->context,
                                                  GNLP_BUILTIN_AGENDA_EVENT_PARSER,
                                                  "en_US",
                                                  NULL,
                                                  &error);

  g_assert (!error);
}

static void
real_list_operations (Context *data)
{
  GError *error = NULL;
  GList *operations, *l;

  gnlp_context_list_operations_sync (data->context,
                                     NULL,
                                     NULL,
                                     &error);
  g_assert (!error);
}

static void
run_operation (TestOperation operation)
{
  Context *data;
  GError *error = NULL;

  data = g_new0 (Context, 1);
  data->operation = operation;

  /* Starts retrieving the GnlpContext */
  data->context = gnlp_context_new_sync ("test-context", NULL, &error);
  g_assert (!error);

  /* Run the desired operation here */
  switch (data->operation)
    {
    case ACQUIRE_CONTEXT:
      break;

    case LIST_OPERATIONS:
      real_list_operations (data);
      break;

    case EVENT_PARSER:
      real_event_parser (data);
      break;

    default:
      g_assert_not_reached ();
    }

  g_clear_object (&data->context);
  g_clear_pointer (&data, g_free);
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

static void
event_parser (void)
{
  run_operation (EVENT_PARSER);
}

gint
main (gint   argc,
      gchar *argv[])
{
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/context/acquire_context", acquire_context);
  g_test_add_func ("/context/list_operations", list_operations);
  g_test_add_func ("/context/event_parser", event_parser);

  return g_test_run ();
}
