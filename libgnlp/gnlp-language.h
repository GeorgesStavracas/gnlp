/* gnlp-language.h
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

#ifndef GNLP_LANGUAGE_H
#define GNLP_LANGUAGE_H

#if !defined (GNLP_INSIDE) && !defined (GNLP_COMPILATION)
#error "Only <gnlp-0/gnlp.h> can be included directly."
#endif

#include <glib-object.h>

G_BEGIN_DECLS

#define GNLP_TYPE_LANGUAGE (gnlp_language_get_type())

typedef enum
{
  GNLP_QUIRK_QUALITY_NONE      = 0,
  GNLP_QUIRK_QUALITY_AVAILABLE = 1,
  GNLP_QUIRK_QUALITY_LOW       = 2,
  GNLP_QUIRK_QUALITY_AVERAGE   = 3,
  GNLP_QUIRK_QUALITY_HIGH      = 4
} GnlpQuirkQuality;

typedef enum
{
  GNLP_LANGUAGE_QUIRK_LISTEN,
  GNLP_LANGUAGE_QUIRK_SPEAK
} GnlpLanguageQuirk;

G_DECLARE_FINAL_TYPE (GnlpLanguage, gnlp_language, GNLP, LANGUAGE, GObject)

GnlpLanguage*        gnlp_language_new                           (const gchar        *locale);

const gchar*         gnlp_language_get_locale                    (GnlpLanguage       *self);

const gchar*         gnlp_language_get_name                      (GnlpLanguage       *self);

const gchar*         gnlp_language_get_path                      (GnlpLanguage       *self);

gboolean             gnlp_language_has_quirk                     (GnlpLanguage       *self,
                                                                  GnlpLanguageQuirk   quirk);

GnlpQuirkQuality     gnlp_language_get_quirk_quality             (GnlpLanguage       *self,
                                                                  GnlpLanguageQuirk   quirk);

void                 gnlp_language_add_quirk                     (GnlpLanguage       *self,
                                                                  GnlpLanguageQuirk   quirk,
                                                                  GnlpQuirkQuality    quality);

void                 gnlp_language_remove_quirk                  (GnlpLanguage       *self,
                                                                  GnlpLanguageQuirk   quirk);

gint                 gnlp_language_get_score                     (GnlpLanguage       *self);

gint                 gnlp_language_compare                       (GnlpLanguage      **a,
                                                                  GnlpLanguage      **b);

G_END_DECLS

#endif /* GNLP_LANGUAGE_H */

