/* gnlp-agenda-event-parser-plugin.c
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

#define G_LOG_DOMAIN "Plugin"

#include <glib-object.h>
#include <libpeas/peas.h>
#include "gnlp.h"
#include "gnlp-engine.h"
#include "gnlp-agenda-event-parser-generic.h"

#define GNLP_TYPE_AGENDA_EVENT_PARSER_PLUGIN (gnlp_agenda_event_parser_plugin_get_type())

struct _GnlpAgendaEventParserPlugin
{
  GObject             parent;

  GnlpEngine         *engine;
};

static void          peas_activatable_iface_init                    (PeasActivatableInterface *iface);

void                 gnlp_agenda_event_parser_plugin_register_types (PeasObjectModule *module);

G_DECLARE_FINAL_TYPE (GnlpAgendaEventParserPlugin,
                      gnlp_agenda_event_parser_plugin,
                      GNLP,
                      AGENDA_EVENT_PARSER_PLUGIN,
                      GObject)

G_DEFINE_TYPE_WITH_CODE (GnlpAgendaEventParserPlugin,
                         gnlp_agenda_event_parser_plugin,
                         G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (PEAS_TYPE_ACTIVATABLE,
                                                peas_activatable_iface_init))

enum
{
  PROP_0,
  PROP_OBJECT,
  N_PROPS
};

/*
 * PeasActivatable
 */
static void
gnlp_agenda_event_parser_plugin_activate (PeasActivatable *activatable)
{
  GnlpAgendaEventParserPlugin *self = GNLP_AGENDA_EVENT_PARSER_PLUGIN (activatable);

  gnlp_engine_register_operation (self->engine,
                                  GNLP_BUILTIN_AGENDA_EVENT_PARSER,
                                  NULL,
                                  GNLP_TYPE_AGENDA_EVENT_PARSER_GENERIC);
}

static void
gnlp_agenda_event_parser_plugin_deactivate (PeasActivatable *activatable)
{
  GnlpAgendaEventParserPlugin *self = GNLP_AGENDA_EVENT_PARSER_PLUGIN (activatable);

  gnlp_engine_unregister_operation (self->engine, GNLP_TYPE_AGENDA_EVENT_PARSER_GENERIC);
}

static void
peas_activatable_iface_init (PeasActivatableInterface *iface)
{
  iface->activate = gnlp_agenda_event_parser_plugin_activate;
  iface->deactivate = gnlp_agenda_event_parser_plugin_deactivate;
}

/*
 * GObject methods
 */
static void
gnlp_agenda_event_parser_plugin_get_property (GObject    *object,
                                              guint       prop_id,
                                              GValue     *value,
                                              GParamSpec *pspec)
{
  GnlpAgendaEventParserPlugin *self = GNLP_AGENDA_EVENT_PARSER_PLUGIN (object);

  switch (prop_id)
    {
    case PROP_OBJECT:
      g_value_set_object (value, self->engine);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gnlp_agenda_event_parser_plugin_set_property (GObject      *object,
                                              guint         prop_id,
                                              const GValue *value,
                                              GParamSpec   *pspec)
{
  GnlpAgendaEventParserPlugin *self = GNLP_AGENDA_EVENT_PARSER_PLUGIN (object);

  switch (prop_id)
    {
    case PROP_OBJECT:
      self->engine = g_value_get_object (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gnlp_agenda_event_parser_plugin_class_init (GnlpAgendaEventParserPluginClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->get_property = gnlp_agenda_event_parser_plugin_get_property;
  object_class->set_property = gnlp_agenda_event_parser_plugin_set_property;

  g_object_class_override_property (object_class, PROP_OBJECT, "object");
}

static void
gnlp_agenda_event_parser_plugin_init (GnlpAgendaEventParserPlugin *self)
{
  g_debug ("Initializing plugin 'Agenda Event Parser'...");
}

void
gnlp_agenda_event_parser_plugin_register_types (PeasObjectModule *module)
{
  peas_object_module_register_extension_type (module,
                                              PEAS_TYPE_ACTIVATABLE,
                                              GNLP_TYPE_AGENDA_EVENT_PARSER_PLUGIN);
}
