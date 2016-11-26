/* test-speak.c
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

guint messages = 0;

static void
finished_reading_message_cb (GObject      *object,
                             GAsyncResult *result,
                             gpointer      user_data)
{
  GMainLoop *mainloop = user_data;

  if (--messages == 0)
    g_main_loop_quit (mainloop);
}

static void
speech_speaker_multiple_texts (void)
{
  GnlpClient *client;
  GMainLoop *mainloop;
  GError *error;

  error = NULL;
  client = gnlp_client_new_sync (NULL, NULL, &error);

  g_assert_null (error);
  g_assert_nonnull (client);

  mainloop = g_main_loop_new (NULL, FALSE);
  messages = 6;

  gnlp_client_read_text (client, "Hello world",   NULL, finished_reading_message_cb, mainloop);
  gnlp_client_read_text (client, "Hello world 2", NULL, finished_reading_message_cb, mainloop);
  gnlp_client_read_text (client, "Hello world 3", NULL, finished_reading_message_cb, mainloop);
  gnlp_client_read_text (client, "Hello world 4", NULL, finished_reading_message_cb, mainloop);
  gnlp_client_read_text (client, "Hello world 5", NULL, finished_reading_message_cb, mainloop);

  g_main_loop_run (mainloop);

  g_clear_object (&client);
}

static void
speech_speaker_hello_world (void)
{
  GnlpClient *client;
  GError *error;

  error = NULL;
  client = gnlp_client_new_sync (NULL, NULL, &error);

  g_assert_null (error);
  g_assert_nonnull (client);

  gnlp_client_read_text_sync (client, "Hello world", NULL, &error);

  g_clear_object (&client);
}

gint
main (gint   argc,
      gchar *argv[])
{
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/speech/speaker/hello_world", speech_speaker_hello_world);
  g_test_add_func ("/speech/speaker/multiple_texts", speech_speaker_multiple_texts);

  return g_test_run ();
}
