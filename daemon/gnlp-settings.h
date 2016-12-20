/* gnlp-settings.h
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

#ifndef GNLP_SETTINGS_H
#define GNLP_SETTINGS_H

#include "gnlp.h"
#include "gnlp-dialog-state.h"

#include <glib-object.h>

G_BEGIN_DECLS

#define GNLP_TYPE_SETTINGS (gnlp_settings_get_type())

G_DECLARE_FINAL_TYPE (GnlpSettings, gnlp_settings, GNLP, SETTINGS, GObject)

GnlpSettings*        gnlp_settings_new                           (void);

GnlpLanguage*        gnlp_settings_get_language                  (GnlpSettings       *self);

void                 gnlp_settings_set_language                  (GnlpSettings       *self,
                                                                  GnlpLanguage       *language);

GnlpDialogState*     gnlp_settings_get_dialog_state              (GnlpSettings       *self);

G_END_DECLS

#endif /* GNLP_SETTINGS_H */

