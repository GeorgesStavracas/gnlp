/* gnlp-voice.c
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

#include "gnlp-voice.h"

#include <espeak-ng/espeak_ng.h>

struct _GnlpVoice
{
  guint               ref_count;

  gchar              *name;
  GnlpVoiceGender     gender;

  gint                age;
};

G_DEFINE_BOXED_TYPE (GnlpVoice, gnlp_voice, gnlp_voice_ref, gnlp_voice_unref)

static GPtrArray *cached_voices = NULL;

G_LOCK_DEFINE_STATIC (cache_lock);


static GnlpVoice*
gnlp_voice_new (const gchar     *name,
                GnlpVoiceGender  gender,
                gint             age)
{
  GnlpVoice *self;

  self = g_slice_new0 (GnlpVoice);
  self->ref_count = 1;
  self->name = g_strdup (name);
  self->gender = gender;
  self->age = age;

  return self;
}

static void
build_voice_cache (GCancellable  *cancellable,
                   GError       **error)
{
  const espeak_VOICE **espeak_voices;
  guint i;

  G_LOCK (cache_lock);

  if (!cached_voices)
    return;

  espeak_voices = espeak_ListVoices (NULL);
  cached_voices = g_ptr_array_new_with_free_func ((GDestroyNotify) gnlp_voice_unref);

  for (i = 0; espeak_voices[i]; i++)
    {
      const espeak_VOICE *espeak_voice;
      GnlpVoice *voice;

      if (g_cancellable_set_error_if_cancelled (cancellable, error))
        {
          g_ptr_array_free (cached_voices, TRUE);
          break;
        }

      espeak_voice = espeak_voices[i];
      voice = gnlp_voice_new (espeak_voice->name,
                              espeak_voice->gender,
                              espeak_voice->age);

      g_ptr_array_add (cached_voices, voice);
    }

  G_UNLOCK (cache_lock);
}

static void
list_available_voices_cb (GTask        *task,
                          gpointer      source_object,
                          gpointer      task_data,
                          GCancellable *cancellable)
{
  GPtrArray *voices;
  GError *error;

  error = NULL;
  voices = gnlp_voice_list_available_voices_sync (cancellable, &error);

  if (error)
    {
      g_task_return_error (task, error);
      return;
    }

  g_task_return_pointer (task,
                         g_ptr_array_ref (voices),
                         (GDestroyNotify) g_ptr_array_unref);
}

/**
 * gnlp_voice_copy:
 * @self: a #GnlpVoice
 *
 * Copies the #GnlpVoice.
 *
 * Returns: (transfer full): a newly-allocated #GnlpVoice
 *
 * Since: 0.1.0
 */
GnlpVoice *
gnlp_voice_copy (GnlpVoice *self)
{
  g_return_val_if_fail (self, NULL);
  g_return_val_if_fail (self->ref_count, NULL);

  return gnlp_voice_new (self->name, self->gender, self->age);
}

static void
gnlp_voice_free (GnlpVoice *self)
{
  g_assert (self);
  g_assert_cmpint (self->ref_count, ==, 0);

  g_clear_pointer (&self->name, g_free);
  g_slice_free (GnlpVoice, self);
}


/**
 * gnlp_voice_ref:
 * @self: a #GnlpVoice
 *
 * Increases the refcount of @self.
 *
 * Returns: (transfer full): @self
 *
 * Since: 0.1.0
 */
GnlpVoice*
gnlp_voice_ref (GnlpVoice *self)
{
  g_return_val_if_fail (self, NULL);
  g_return_val_if_fail (self->ref_count, NULL);

  g_atomic_int_inc (&self->ref_count);

  return self;
}


/**
 * gnlp_voice_unref:
 * @self: a #GnlpVoice
 *
 * Decreases the refcount of @self. Frees @self if the refcount is %0.
 *
 * Since: 0.1.0
 */
void
gnlp_voice_unref (GnlpVoice *self)
{
  g_return_if_fail (self);
  g_return_if_fail (self->ref_count);

  if (g_atomic_int_dec_and_test (&self->ref_count))
    gnlp_voice_free (self);
}


/**
 * gnlp_voice_get_name:
 * @self: a #GnlpVoice
 *
 * Retrieves the name of @self.
 *
 * Returns: (transfer none): a string. Do not free.
 *
 * Since: 0.1.0
 */
const gchar*
gnlp_voice_get_name (GnlpVoice *self)
{
  g_return_val_if_fail (self, NULL);

  return self->name;
}

/**
 * gnlp_voice_get_gender:
 * @self: a #GnlpVoice
 *
 * Retrieves the gender of @self.
 *
 * Returns: a @GnlpVoiceGender
 *
 * Since: 0.1.0
 */
GnlpVoiceGender
gnlp_voice_get_gender (GnlpVoice *self)
{
  g_return_val_if_fail (self, GNLP_VOICE_GENDER_NONE);

  return self->gender;
}

/**
 * gnlp_voice_get_age:
 * @self: a #GnlpVoice
 *
 * Retrieves the age of @self.
 *
 * Returns: the age of @self, or @GNLP_VOICE_AGE_NOT_SPECIFIED
 *
 * Since: 0.1.0
 */
gint
gnlp_voice_get_age (GnlpVoice *self)
{
  g_return_val_if_fail (self, GNLP_VOICE_AGE_NOT_SPECIFIED);

  return self->age;
}

/**
 * gnlp_voice_list_available_voices:
 * @cancellable: (nullable): a #GCancellable
 * @callback: (scope async): a #GAsyncReadyCallback to call when the initialization is done
 * @user_data: (closure): data to pass to the callback function
 *
 * List the available voices.
 *
 * Since: 0.1.0
 */
void
gnlp_voice_list_available_voices (GCancellable        *cancellable,
                                  GAsyncReadyCallback  callback,
                                  gpointer             user_data)
{
  GTask *task;

  task = g_task_new (NULL, cancellable, callback, user_data);
  g_task_run_in_thread (task, list_available_voices_cb);
}

/**
 * gnlp_voice_list_available_voices_finish:
 * @result: a #GAsyncResult
 * @error: (nullable): return location for a #GError, or %NULL
 *
 * Finishes the operation started by gnlp_voice_list_available_voices().
 *
 * Returns: (transfer full): a #GPtrArray. Free with g_ptr_array_unref() after use.
 *
 * Since: 0.1.0
 */
GPtrArray*
gnlp_voice_list_available_voices_finish (GAsyncResult  *result,
                                         GError       **error)
{
  g_return_val_if_fail (G_IS_TASK (result), NULL);
  g_return_val_if_fail (g_task_is_valid (result, NULL), NULL);

  return g_task_propagate_pointer (G_TASK (result), error);
}

/**
 * gnlp_voice_list_available_voices_sync:
 * @cancellable: (nullable): a #GCancellable
 * @error: (nullable): return location for a #GError, or %NULL
 *
 * Synchronously list the available voices. This function blocks the current thread.
 *
 * See gnlp_voice_list_available_voices() for the asynchronous version.
 *
 * Returns: (transfer full): a #GPtrArray. Free with g_ptr_array_unref() after use.
 *
 * Since: 0.1.0
 */
GPtrArray*
gnlp_voice_list_available_voices_sync (GCancellable  *cancellable,
                                       GError       **error)
{
  g_return_val_if_fail (!error || !*error, NULL);

  if (!cached_voices)
    build_voice_cache (cancellable, error);

  return cached_voices ? g_ptr_array_ref (cached_voices) : NULL;
}
