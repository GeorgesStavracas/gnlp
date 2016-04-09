/* gnlp-client-context.c
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

struct _GnlpClientContext
{
  GObject             parent_instance;

  GList              *operations;
  GnlpEngine         *engine;

  gchar              *application_id;
};

G_DEFINE_TYPE (GnlpClientContext, gnlp_client_context, G_TYPE_OBJECT)

enum
{
  PROP_0,
  PROP_APPLICATION_ID,
  PROP_ENGINE,
  N_PROPS
};

static GParamSpec *properties [N_PROPS] = {NULL, };

static void
gnlp_client_context_finalize (GObject *object)
{
  GnlpClientContext *self = (GnlpClientContext *)object;

  g_clear_pointer (&self->application_id, g_free);

  G_OBJECT_CLASS (gnlp_client_context_parent_class)->finalize (object);
}

static void
gnlp_client_context_get_property (GObject    *object,
                                  guint       prop_id,
                                  GValue     *value,
                                  GParamSpec *pspec)
{
  GnlpClientContext *self = GNLP_CLIENT_CONTEXT (object);

  switch (prop_id)
    {
    case PROP_APPLICATION_ID:
      g_value_set_string (value, self->application_id);
      break;

    case PROP_ENGINE:
      g_value_set_object (value, self->engine);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gnlp_client_context_set_property (GObject      *object,
                                  guint         prop_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
  GnlpClientContext *self = GNLP_CLIENT_CONTEXT (object);

  switch (prop_id)
    {
    case PROP_APPLICATION_ID:
      self->application_id = g_value_dup_string (value);
      g_object_notify_by_pspec (object, properties[PROP_APPLICATION_ID]);
      break;

    case PROP_ENGINE:
      self->engine = g_value_dup_object (value);
      g_object_notify_by_pspec (object, properties[PROP_ENGINE]);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gnlp_client_context_class_init (GnlpClientContextClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gnlp_client_context_finalize;
  object_class->get_property = gnlp_client_context_get_property;
  object_class->set_property = gnlp_client_context_set_property;

  properties[PROP_APPLICATION_ID] = g_param_spec_string ("application-id",
                                                         "Application ID",
                                                         "The application id",
                                                         NULL,
                                                         G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

  properties[PROP_ENGINE] = g_param_spec_object ("engine",
                                                 "Engine",
                                                 "The engine of the daemon",
                                                 GNLP_TYPE_ENGINE,
                                                 G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
gnlp_client_context_init (GnlpClientContext *self)
{
}

GnlpClientContext*
gnlp_client_context_new (const gchar *application_id,
                         GnlpEngine  *engine)
{
  return g_object_new (GNLP_TYPE_CLIENT_CONTEXT,
                       "application-id", application_id,
                       "engine", engine,
                       NULL);
}

gchar*
gnlp_client_context_create_operation (GnlpClientContext *self,
                                      const gchar       *operation,
                                      const gchar       *language)
{
  g_return_val_if_fail (GNLP_IS_CLIENT_CONTEXT (self), NULL);

  return g_strdup ("test");
}

//plugins/agenda-event-parser/gnlp-agenda-event-parser-plugin.c
