/* gnlp-operation-capsule.c
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

#include "gnlp-operation-capsule.h"

struct _GnlpOperationCapsule
{
  GObject             parent;

  GnlpExtension      *extension;
  GnlpOperation      *skeleton;
};

G_DEFINE_TYPE (GnlpOperationCapsule, gnlp_operation_capsule, G_TYPE_OBJECT)

enum {
  PROP_0,
  PROP_EXTENSION,
  PROP_SKELETON,
  N_PROPS
};

static GParamSpec *properties [N_PROPS] = { NULL, };

/*
 * Callbacks
 */
static void
handle_run (GnlpOperation         *operation,
            GDBusMethodInvocation *method,
            GnlpOperationCapsule  *self)
{

}

static void
gnlp_operation_capsule_finalize (GObject *object)
{
  GnlpOperationCapsule *self = (GnlpOperationCapsule *)object;

  g_clear_object (&self->extension);
  g_clear_object (&self->skeleton);

  G_OBJECT_CLASS (gnlp_operation_capsule_parent_class)->finalize (object);
}

static void
gnlp_operation_capsule_get_property (GObject    *object,
                                     guint       prop_id,
                                     GValue     *value,
                                     GParamSpec *pspec)
{
  GnlpOperationCapsule *self = GNLP_OPERATION_CAPSULE (object);

  switch (prop_id)
    {
    case PROP_EXTENSION:
      g_value_set_object (value, self->extension);
      break;

    case PROP_SKELETON:
      g_value_set_object (value, self->skeleton);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gnlp_operation_capsule_set_property (GObject      *object,
                                     guint         prop_id,
                                     const GValue *value,
                                     GParamSpec   *pspec)
{
  GnlpOperationCapsule *self = GNLP_OPERATION_CAPSULE (object);

  switch (prop_id)
    {
    case PROP_EXTENSION:
      self->extension = g_value_dup_object (value);
      break;

    case PROP_SKELETON:
      self->skeleton = g_value_dup_object (value);

      g_signal_connect (self->skeleton,
                        "handle-run",
                        G_CALLBACK (handle_run),
                        self);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gnlp_operation_capsule_class_init (GnlpOperationCapsuleClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gnlp_operation_capsule_finalize;
  object_class->get_property = gnlp_operation_capsule_get_property;
  object_class->set_property = gnlp_operation_capsule_set_property;

  properties[PROP_EXTENSION] = g_param_spec_object ("extension",
                                                    "Extension",
                                                    "The extension of this capsule",
                                                    GNLP_TYPE_EXTENSION,
                                                    G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);


  properties[PROP_SKELETON] = g_param_spec_object ("skeleton",
                                                   "Skeleton",
                                                   "The DBus-exported skeleton of this capsule",
                                                   GNLP_TYPE_OPERATION,
                                                   G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
}

static void
gnlp_operation_capsule_init (GnlpOperationCapsule *self)
{
}

GnlpOperationCapsule*
gnlp_operation_capsule_new (GnlpExtension *extension,
                            GnlpOperation *skeleton)
{
  return g_object_new (GNLP_TYPE_OPERATION_CAPSULE,
                       "extension", extension,
                       "skeleton", skeleton,
                       NULL);
}
