/* test-client.c
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
speech_listener_state (void)
{
  GnlpClient *client;
  GMainLoop *mainloop;
  GError *error;

  error = NULL;
  client = gnlp_client_new_sync ("GNOME", NULL, &error);

  g_assert_null (error);
  g_assert_nonnull (client);

  mainloop = g_main_loop_new (NULL, FALSE);

  g_signal_connect_swapped (client,
                            "notify::is-listening",
                            G_CALLBACK (g_main_loop_quit),
                            mainloop);

  /* Run the mainloop until we get a message */
  g_main_loop_run (mainloop);

  g_clear_object (&client);
}


/*************************************************************************************************/

static void
speech_listener_listen_named (void)
{
  GnlpClient *client;
  GMainLoop *mainloop;
  GError *error;

  client = gnlp_client_new_sync ("GNOME", NULL, &error);

  g_assert_null (error);
  g_assert_nonnull (client);

  error = NULL;
  mainloop = g_main_loop_new (NULL, FALSE);

  g_signal_connect_swapped (client,
                            "command_received",
                            G_CALLBACK (g_main_loop_quit),
                            mainloop);

  /* Run the mainloop until we get a message */
  g_main_loop_run (mainloop);

  g_clear_object (&client);
}

static void
speech_listener_listen_command (void)
{
  GnlpClient *client;
  GMainLoop *mainloop;
  GError *error;

  error = NULL;
  client = gnlp_client_new_sync (NULL, NULL, &error);

  g_assert_null (error);
  g_assert_nonnull (client);

  mainloop = g_main_loop_new (NULL, FALSE);

  g_signal_connect_swapped (client,
                            "command_received",
                            G_CALLBACK (g_main_loop_quit),
                            mainloop);

  /* Run the mainloop until we get a message */
  g_main_loop_run (mainloop);

  g_clear_object (&client);
}

static void
speech_listener_listen_speech (void)
{
  GnlpClient *client;
  GMainLoop *mainloop;
  GError *error;

  error = NULL;
  client = gnlp_client_new_sync (NULL, NULL, &error);

  g_assert_null (error);
  g_assert_nonnull (client);

  mainloop = g_main_loop_new (NULL, FALSE);

  g_signal_connect_swapped (client,
                            "speech_recognized",
                            G_CALLBACK (g_main_loop_quit),
                            mainloop);

  /* Run the mainloop until we get a message */
  g_main_loop_run (mainloop);

  g_clear_object (&client);
}

/*************************************************************************************************/

static void
client_init_named (void)
{
  GnlpClient *client;
  GError *error;

  error = NULL;
  client = gnlp_client_new_sync ("GNOME", NULL, &error);

  g_assert_null (error);
  g_assert_nonnull (client);

  g_clear_object (&client);
}

static void
client_init (void)
{
  GnlpClient *client;
  GError *error;

  error = NULL;
  client = gnlp_client_new_sync (NULL, NULL, &error);

  g_assert_null (error);
  g_assert_nonnull (client);

  g_clear_object (&client);
}

gint
main (gint   argc,
      gchar *argv[])
{
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/client/init", client_init);
  g_test_add_func ("/client/init_named", client_init_named);
  g_test_add_func ("/speech/listener/listen_command", speech_listener_listen_command);
  g_test_add_func ("/speech/listener/listen_speech", speech_listener_listen_speech);
  g_test_add_func ("/speech/listener/listen_named", speech_listener_listen_named);
  g_test_add_func ("/speech/listener/state", speech_listener_state);

  return g_test_run ();
}
