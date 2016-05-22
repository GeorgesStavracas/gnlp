/* gnlp-agenda-event-parser-generic.c
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

#include "gnlp-agenda-event-parser-generic.h"

struct _GnlpAgendaEventParserGeneric
{
  GObject             parent;

  GCancellable       *cancellable;

  GRegex             *regex;
  gchar              *text;
};

static void          gnlp_agenda_event_parser_generic_operation_init (GnlpOperationIface *iface);

G_DEFINE_TYPE_WITH_CODE (GnlpAgendaEventParserGeneric, gnlp_agenda_event_parser_generic, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (GNLP_TYPE_OPERATION,
                                                gnlp_agenda_event_parser_generic_operation_init))

enum
{
  PROP_0,
  PROP_TEXT,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

static gboolean
gnlp_agenda_event_parser_generic_handle_cancel (GnlpOperation         *operation,
                                                GDBusMethodInvocation *method)
{
  GnlpAgendaEventParserGeneric *self = GNLP_AGENDA_EVENT_PARSER_GENERIC (operation);

  g_cancellable_cancel (self->cancellable);

  return TRUE;
}

static gboolean
gnlp_agenda_event_parser_generic_handle_get_output (GnlpOperation         *operation,
                                                    GDBusMethodInvocation *method)
{
  GnlpAgendaEventParserGeneric *self = GNLP_AGENDA_EVENT_PARSER_GENERIC (operation);

  g_cancellable_cancel (self->cancellable);

  return TRUE;
}

static void
gnlp_agenda_event_parser_generic_operation_init (GnlpOperationIface *iface)
{
  iface->handle_cancel = gnlp_agenda_event_parser_generic_handle_cancel;
  // TODO
}

static void
gnlp_agenda_event_parser_generic_finalize (GObject *object)
{
  GnlpAgendaEventParserGeneric *self = (GnlpAgendaEventParserGeneric *)object;

  g_cancellable_cancel (self->cancellable);

  g_clear_pointer (&self->text, g_free);
  g_clear_object (&self->cancellable);

  G_OBJECT_CLASS (gnlp_agenda_event_parser_generic_parent_class)->finalize (object);
}

static void
gnlp_agenda_event_parser_generic_get_property (GObject    *object,
                                               guint       prop_id,
                                               GValue     *value,
                                               GParamSpec *pspec)
{
  GnlpAgendaEventParserGeneric *self = GNLP_AGENDA_EVENT_PARSER_GENERIC (object);

  switch (prop_id)
    {
    case PROP_TEXT:
      g_value_set_string (value, self->text);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gnlp_agenda_event_parser_generic_set_property (GObject      *object,
                                               guint         prop_id,
                                               const GValue *value,
                                               GParamSpec   *pspec)
{
  GnlpAgendaEventParserGeneric *self = GNLP_AGENDA_EVENT_PARSER_GENERIC (object);

  switch (prop_id)
    {
    case PROP_TEXT:
      if (g_strcmp0 (self->text, g_value_get_string (value)) != 0)
        {
          g_clear_pointer (&self->text, g_free);
          self->text = g_value_dup_string (value);

          g_object_notify_by_pspec (object, properties[PROP_TEXT]);
        }
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gnlp_agenda_event_parser_generic_class_init (GnlpAgendaEventParserGenericClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gnlp_agenda_event_parser_generic_finalize;
  object_class->get_property = gnlp_agenda_event_parser_generic_get_property;
  object_class->set_property = gnlp_agenda_event_parser_generic_set_property;


  properties[PROP_TEXT] = g_param_spec_string ("text",
                                               "Text",
                                               "The text being parsed",
                                               NULL,
                                               G_PARAM_READWRITE);

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
gnlp_agenda_event_parser_generic_init (GnlpAgendaEventParserGeneric *self)
{
  self->cancellable = g_cancellable_new ();
}

GnlpOperation*
gnlp_agenda_event_parser_generic_new (void)
{
  return g_object_new (GNLP_TYPE_AGENDA_EVENT_PARSER_GENERIC, NULL);
}

