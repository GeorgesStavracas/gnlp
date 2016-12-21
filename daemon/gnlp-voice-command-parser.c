/* gnlp-voice-command-parser.c
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

#define G_LOG_DOMAIN "VoiceCommandParser"

#include "gnlp-context.h"
#include "gnlp-input-parser.h"
#include "gnlp-voice-command-parser.h"

struct _GnlpVoiceCommandParser
{
  GObject             parent;

  GnlpContext        *context;
};

static void          gnlp_input_parser_iface_init                (GnlpInputParserInterface *iface);

G_DEFINE_TYPE_WITH_CODE (GnlpVoiceCommandParser, gnlp_voice_command_parser, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (GNLP_TYPE_INPUT_PARSER, gnlp_input_parser_iface_init))

enum {
  PROP_0,
  PROP_CONTEXT,
  N_PROPS
};

static gboolean
gnlp_voice_command_parser_parse (GnlpInputParser *parser,
                                 const GStrv      words,
                                 gdouble         *word_confidence,
                                 const gchar     *sentence,
                                 gdouble          total_confidence)
{
  GnlpVoiceCommandParser *self;
  GnlpCommandDispatcher *dispatcher;
  GnlpVoiceCommand *command;
  gboolean valid;
  guint len;

  self = GNLP_VOICE_COMMAND_PARSER (parser);
  len = g_strv_length (words);
  dispatcher = gnlp_context_get_command_dispatcher (self->context);

  /* Build the command */
  command = gnlp_voice_command_new ();

  if (len == 4)
    {
      gnlp_voice_command_set_target (command, words[1]);
      gnlp_voice_command_set_action (command, words[2]);
      gnlp_voice_command_push_parameter (command, words[3]);
    }
  else if (len == 2)
    {
      gnlp_voice_command_push_parameter (command, words[0]);
      gnlp_voice_command_push_parameter (command, words[1]);
    }
  else
    {
      return GNLP_PARSE_FAIL;
    }

  valid = gnlp_command_dispatcher_dispatch_command (dispatcher, command);

  return valid;
}

static void
gnlp_input_parser_iface_init (GnlpInputParserInterface *iface)
{
  iface->parse = gnlp_voice_command_parser_parse;
}

static void
gnlp_voice_command_parser_finalize (GObject *object)
{
  GnlpVoiceCommandParser *self = (GnlpVoiceCommandParser *)object;

  g_clear_object (&self->context);

  G_OBJECT_CLASS (gnlp_voice_command_parser_parent_class)->finalize (object);
}

static void
gnlp_voice_command_parser_get_property (GObject    *object,
                                        guint       prop_id,
                                        GValue     *value,
                                        GParamSpec *pspec)
{
  GnlpVoiceCommandParser *self = GNLP_VOICE_COMMAND_PARSER (object);

  switch (prop_id)
    {
    case PROP_CONTEXT:
      g_value_set_object (value, self->context);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gnlp_voice_command_parser_set_property (GObject      *object,
                                        guint         prop_id,
                                        const GValue *value,
                                        GParamSpec   *pspec)
{
  GnlpVoiceCommandParser *self = GNLP_VOICE_COMMAND_PARSER (object);

  switch (prop_id)
    {
    case PROP_CONTEXT:
      self->context = g_value_dup_object (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gnlp_voice_command_parser_class_init (GnlpVoiceCommandParserClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gnlp_voice_command_parser_finalize;
  object_class->get_property = gnlp_voice_command_parser_get_property;
  object_class->set_property = gnlp_voice_command_parser_set_property;

  g_object_class_override_property (object_class, PROP_CONTEXT, "context");
}

static void
gnlp_voice_command_parser_init (GnlpVoiceCommandParser *self)
{
}

GnlpVoiceCommandParser*
gnlp_voice_command_parser_new (void)
{
  return g_object_new (GNLP_TYPE_VOICE_COMMAND_PARSER, NULL);
}
