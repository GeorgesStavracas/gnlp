/* gnlp-dialog-state.h
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

#ifndef GNLP_DIALOG_STATE_H
#define GNLP_DIALOG_STATE_H

#include <glib-object.h>

G_BEGIN_DECLS

#define GNLP_TYPE_DIALOG_STATE (gnlp_dialog_state_get_type())

typedef enum
{
  GNLP_STATE_INVALID,
  GNLP_STATE_WAITING,
  GNLP_STATE_LISTENING,
  GNLP_STATE_PARSING,
  GNLP_STATE_SPEAKING,
  GNLP_STATE_WAITING_ANSWER
} GnlpState;

G_DECLARE_FINAL_TYPE (GnlpDialogState, gnlp_dialog_state, GNLP, DIALOG_STATE, GObject)

GnlpDialogState*     gnlp_dialog_state_new                       (void);

GnlpState            gnlp_dialog_state_get_state                 (GnlpDialogState    *self);

void                 gnlp_dialog_state_set_state                 (GnlpDialogState    *self,
                                                                  GnlpState           state);

G_END_DECLS

#endif /* GNLP_DIALOG_STATE_H */

