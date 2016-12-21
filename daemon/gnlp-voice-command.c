/* gnlp-voice-command.c
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

#include "gnlp-voice-command.h"

struct _GnlpVoiceCommand
{
  guint               ref_count;

  gchar              *target;
  gchar              *action;
  GPtrArray          *parameters;
};

G_DEFINE_BOXED_TYPE (GnlpVoiceCommand, gnlp_voice_command, gnlp_voice_command_ref, gnlp_voice_command_unref)

GnlpVoiceCommand*
gnlp_voice_command_new (void)
{
  GnlpVoiceCommand *self;

  self = g_slice_new0 (GnlpVoiceCommand);
  self->ref_count = 1;

  return self;
}

GnlpVoiceCommand*
gnlp_voice_command_copy (GnlpVoiceCommand *self)
{
  GnlpVoiceCommand *copy;

  g_return_val_if_fail (self, NULL);
  g_return_val_if_fail (self->ref_count, NULL);

  copy = gnlp_voice_command_new ();

  return copy;
}

static void
gnlp_voice_command_free (GnlpVoiceCommand *self)
{
  g_assert (self);
  g_assert_cmpint (self->ref_count, ==, 0);

  g_free (self->target);
  g_free (self->action);

  g_ptr_array_set_free_func (self->parameters, g_free);
  g_ptr_array_free (self->parameters, TRUE);

  g_slice_free (GnlpVoiceCommand, self);
}

GnlpVoiceCommand*
gnlp_voice_command_ref (GnlpVoiceCommand *self)
{
  g_return_val_if_fail (self, NULL);
  g_return_val_if_fail (self->ref_count, NULL);

  g_atomic_int_inc (&self->ref_count);

  return self;
}

void
gnlp_voice_command_unref (GnlpVoiceCommand *self)
{
  g_return_if_fail (self);
  g_return_if_fail (self->ref_count);

  if (g_atomic_int_dec_and_test (&self->ref_count))
    gnlp_voice_command_free (self);
}

const gchar*
gnlp_voice_command_get_target (GnlpVoiceCommand *self)
{
  g_return_val_if_fail (self, NULL);

  return self->target;
}

void
gnlp_voice_command_set_target (GnlpVoiceCommand *self,
                               const gchar      *target)
{
  g_return_if_fail (self);
  g_return_if_fail (target);

  g_clear_pointer (&self->target, g_free);
  self->target = g_strdup (target);
}

const gchar*
gnlp_voice_command_get_action (GnlpVoiceCommand *self)
{
  g_return_val_if_fail (self, NULL);

  return self->action;
}

void
gnlp_voice_command_set_action (GnlpVoiceCommand *self,
                               const gchar      *action)
{
  g_return_if_fail (self);
  g_return_if_fail (action);

  g_clear_pointer (&self->action, g_free);
  self->action = g_strdup (action);
}

GPtrArray*
gnlp_voice_command_get_parameters (GnlpVoiceCommand *self)
{
  g_return_val_if_fail (self, NULL);

  return self->parameters;
}

void
gnlp_voice_command_push_parameter (GnlpVoiceCommand *self,
                                   const gchar      *parameter)
{
  g_return_if_fail (self);
  g_return_if_fail (parameter);

  if (!self->parameters)
    {
      self->parameters = g_ptr_array_new ();
      g_ptr_array_add (self->parameters, NULL);
    }

  g_ptr_array_insert (self->parameters, self->parameters->len - 1, g_strdup (parameter));
}

gchar*
gnlp_voice_command_pop_parameter (GnlpVoiceCommand *self)
{
  gchar *data;
  guint idx;

  g_return_val_if_fail (self, NULL);

  if (!self->parameters)
    return NULL;

  idx = self->parameters->len - 1;
  data = g_ptr_array_index (self->parameters, idx);

  g_ptr_array_remove_index (self->parameters, idx);

  if (idx == 1)
    g_clear_pointer (&self->parameters, g_ptr_array_unref);

  return data;
}
