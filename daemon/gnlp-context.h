/* gnlp-context.h
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

#ifndef GNLP_CONTEXT_H
#define GNLP_CONTEXT_H

#include "gnlp.h"
#include "gnlp-command-dispatcher.h"
#include "gnlp-dialog-state.h"

#include <glib-object.h>

G_BEGIN_DECLS

#define GNLP_TYPE_CONTEXT (gnlp_context_get_type())

G_DECLARE_FINAL_TYPE (GnlpContext, gnlp_context, GNLP, CONTEXT, GObject)

GnlpContext*         gnlp_context_new                            (void);

GnlpLanguage*        gnlp_context_get_language                   (GnlpContext        *self);

void                 gnlp_context_set_language                   (GnlpContext        *self,
                                                                  GnlpLanguage       *language);

GnlpDialogState*     gnlp_context_get_dialog_state               (GnlpContext        *self);

GnlpCommandDispatcher* gnlp_context_get_command_dispatcher       (GnlpContext        *self);

G_END_DECLS

#endif /* GNLP_CONTEXT_H */

