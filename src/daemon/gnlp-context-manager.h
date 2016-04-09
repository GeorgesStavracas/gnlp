/* gnlp-context-manager.h
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

#ifndef GNLP_CONTEXT_MANAGER_H
#define GNLP_CONTEXT_MANAGER_H

#include "gnlp-client-context.h"
#include "gnlp-engine.h"

#include <glib-object.h>

G_BEGIN_DECLS

#define GNLP_TYPE_CONTEXT_MANAGER (gnlp_context_manager_get_type())

G_DECLARE_FINAL_TYPE (GnlpContextManager, gnlp_context_manager, GNLP, CONTEXT_MANAGER, GObject)

GnlpContextManager*  gnlp_context_manager_new                    (GnlpEngine         *engine);

void                 gnlp_context_manager_create_context         (GnlpContextManager *self,
                                                                  const gchar        *application_id);

void                 gnlp_context_manager_destroy_context        (GnlpContextManager *self,
                                                                  const gchar        *application_id);

GnlpClientContext*   gnlp_context_manager_get_context            (GnlpContextManager *self,
                                                                  const gchar        *application_id);

G_END_DECLS

#endif /* GNLP_CONTEXT_MANAGER_H */

