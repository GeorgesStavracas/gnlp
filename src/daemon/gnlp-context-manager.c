/* gnlp-context-manager.c
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

#include "gnlp-client-context.h"
#include "gnlp-context-manager.h"

struct _GnlpContextManager
{
  GObject            parent;

  GHashTable        *id_to_context;
  GnlpEngine        *engine;
};

G_DEFINE_TYPE (GnlpContextManager, gnlp_context_manager, G_TYPE_OBJECT)

enum {
  PROP_0,
  PROP_ENGINE,
  N_PROPS
};

static GParamSpec *properties [N_PROPS] = {NULL, };

static void
gnlp_context_manager_finalize (GObject *object)
{
  GnlpContextManager *self = (GnlpContextManager *)object;

  g_clear_object (&self->engine);

  G_OBJECT_CLASS (gnlp_context_manager_parent_class)->finalize (object);
}

static void
gnlp_context_manager_get_property (GObject    *object,
                                   guint       prop_id,
                                   GValue     *value,
                                   GParamSpec *pspec)
{
  GnlpContextManager *self = GNLP_CONTEXT_MANAGER (object);

  switch (prop_id)
    {
    case PROP_ENGINE:
      g_value_set_object (value, self->engine);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gnlp_context_manager_set_property (GObject      *object,
                                   guint         prop_id,
                                   const GValue *value,
                                   GParamSpec   *pspec)
{
  GnlpContextManager *self = GNLP_CONTEXT_MANAGER (object);

  switch (prop_id)
    {
    case PROP_ENGINE:
      self->engine = g_value_dup_object (value);
      g_object_notify_by_pspec (object, properties[PROP_ENGINE]);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gnlp_context_manager_class_init (GnlpContextManagerClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gnlp_context_manager_finalize;
  object_class->get_property = gnlp_context_manager_get_property;
  object_class->set_property = gnlp_context_manager_set_property;

  /**
   * GnlpContextManager::engine:
   *
   * The engine of the daemon.
   */
  properties[PROP_ENGINE] = g_param_spec_object ("engine",
                                                 "Engine",
                                                 "The engine of the daemon",
                                                 GNLP_TYPE_ENGINE,
                                                 G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
gnlp_context_manager_init (GnlpContextManager *self)
{
  self->id_to_context = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_object_unref);
}

/**
 * gnlp_context_manager_new:
 * @engine: a #GnlpEngine
 *
 * Creates a new context manager.
 *
 * Returns: (transfer full): a #GnlpContextManager
 */
GnlpContextManager *
gnlp_context_manager_new (GnlpEngine *engine)
{
  return g_object_new (GNLP_TYPE_CONTEXT_MANAGER,
                       "engine", engine,
                       NULL);
}

void
gnlp_context_manager_create_context (GnlpContextManager *self,
                                     const gchar        *application_id)
{
  g_return_if_fail (GNLP_IS_CONTEXT_MANAGER (self));

  if (!g_hash_table_contains (self->id_to_context, application_id))
    {
      GnlpClientContext *context;

      context = gnlp_client_context_new (application_id, self->engine);

      g_hash_table_insert (self->id_to_context, g_strdup (application_id), context);
    }
}

void
gnlp_context_manager_destroy_context (GnlpContextManager *self,
                                      const gchar        *application_id)
{
  g_return_if_fail (GNLP_IS_CONTEXT_MANAGER (self));

  if (g_hash_table_contains (self->id_to_context, application_id))
    g_hash_table_remove (self->id_to_context, application_id);
}

GnlpClientContext*
gnlp_context_manager_get_context (GnlpContextManager *self,
                                  const gchar        *application_id)
{
  g_return_val_if_fail (GNLP_IS_CONTEXT_MANAGER (self), NULL);
  g_return_val_if_fail (application_id != NULL, NULL);

  return g_hash_table_lookup (self->id_to_context, application_id);
}
