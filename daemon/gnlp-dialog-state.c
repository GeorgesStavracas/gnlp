/* gnlp-dialog-state.c
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

#define G_LOG_DOMAIN "DialogState"

#include "gnlp-dialog-state.h"
#include "gnlp-enum-types.h"

struct _GnlpDialogState
{
  GObject             parent;

  GnlpState           state;
  GMutex              lock;
};

G_DEFINE_TYPE (GnlpDialogState, gnlp_dialog_state, G_TYPE_OBJECT)

enum {
  PROP_0,
  PROP_STATE,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

static void
gnlp_dialog_state_finalize (GObject *object)
{
  GnlpDialogState *self = (GnlpDialogState *)object;

  self->state = GNLP_STATE_INVALID;

  G_OBJECT_CLASS (gnlp_dialog_state_parent_class)->finalize (object);
}

static void
gnlp_dialog_state_get_property (GObject    *object,
                                guint       prop_id,
                                GValue     *value,
                                GParamSpec *pspec)
{
  GnlpDialogState *self = GNLP_DIALOG_STATE (object);

  switch (prop_id)
    {
    case PROP_STATE:
      g_value_set_enum (value, self->state);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gnlp_dialog_state_set_property (GObject      *object,
                                guint         prop_id,
                                const GValue *value,
                                GParamSpec   *pspec)
{
  GnlpDialogState *self = GNLP_DIALOG_STATE (object);

  switch (prop_id)
    {
    case PROP_STATE:
      gnlp_dialog_state_set_state (self, g_value_get_enum (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gnlp_dialog_state_class_init (GnlpDialogStateClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gnlp_dialog_state_finalize;
  object_class->get_property = gnlp_dialog_state_get_property;
  object_class->set_property = gnlp_dialog_state_set_property;

  properties[PROP_STATE] = g_param_spec_enum ("state",
                                              "State",
                                              "State",
                                              GNLP_TYPE_STATE,
                                              GNLP_STATE_INVALID,
                                              G_PARAM_READWRITE);
}

static void
gnlp_dialog_state_init (GnlpDialogState *self)
{
  self->state = GNLP_STATE_INVALID;
}

GnlpDialogState*
gnlp_dialog_state_new (void)
{
  return g_object_new (GNLP_TYPE_DIALOG_STATE, NULL);
}

GnlpState
gnlp_dialog_state_get_state (GnlpDialogState *self)
{
  GnlpState state;

  g_return_val_if_fail (GNLP_IS_DIALOG_STATE (self), GNLP_STATE_INVALID);

  g_mutex_lock (&self->lock);
  state = self->state;
  g_mutex_unlock (&self->lock);

  return state;
}

void
gnlp_dialog_state_set_state (GnlpDialogState *self,
                             GnlpState        state)
{
  g_return_if_fail (GNLP_IS_DIALOG_STATE (self));

  g_mutex_lock (&self->lock);

  if (state != self->state)
    {
      GEnumClass *enum_class;
      GEnumValue *value;

      enum_class = g_type_class_ref (GNLP_TYPE_STATE);
      value = g_enum_get_value (enum_class, state);

      g_debug ("Setting state to %s", value->value_name);

      self->state = state;
      g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_STATE]);

      g_type_class_unref (enum_class);
    }

  g_mutex_unlock (&self->lock);
}
