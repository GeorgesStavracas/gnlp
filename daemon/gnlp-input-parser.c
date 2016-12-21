/* gnlp-input-parser.c
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

#include "gnlp-context.h"

#include "gnlp-input-parser.h"

G_DEFINE_INTERFACE (GnlpInputParser, gnlp_input_parser, G_TYPE_OBJECT)

static void
gnlp_input_parser_default_init (GnlpInputParserInterface *iface)
{
  g_object_interface_install_property (iface,
                                       g_param_spec_object ("context",
                                                            "Context",
                                                            "Context",
                                                            GNLP_TYPE_CONTEXT,
                                                            G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
}

/**
 * gnlp_input_parser_parse:
 * @self: a #GnlpInputParser
 * @words: (nullable): the segmented list of words
 * @word_confidence: (nullable): array of confidence level for each word
 * @sentence: (nullable): the built sentence
 * @total_confidence: the general confidence level
 *
 * Parses the given sentence.
 *
 * Returns: %TRUE on success, %FALSE otherwise
 */
gboolean
gnlp_input_parser_parse (GnlpInputParser *self,
                         const GStrv      words,
                         gdouble         *word_confidence,
                         const gchar     *sentence,
                         gdouble          total_confidence)
{
  g_return_val_if_fail (GNLP_IS_INPUT_PARSER (self), FALSE);
  g_return_val_if_fail (GNLP_INPUT_PARSER_GET_IFACE (self)->parse, FALSE);

  return GNLP_INPUT_PARSER_GET_IFACE (self)->parse (self,
                                                    words,
                                                    word_confidence,
                                                    sentence,
                                                    total_confidence);
}
