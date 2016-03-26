/* gnlp-engine.h
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

#ifndef GNLP_ENGINE_H
#define GNLP_ENGINE_H

#include <glib-object.h>

G_BEGIN_DECLS

#define GNLP_TYPE_ENGINE (gnlp_engine_get_type())

G_DECLARE_FINAL_TYPE (GnlpEngine, gnlp_engine, GNLP, ENGINE, GObject)

GnlpEngine*          gnlp_engine_new                             (void);

void                 gnlp_engine_register_operation              (GnlpEngine         *self,
                                                                  const gchar        *name,
                                                                  const gchar        *language,
                                                                  GType               type);

void                 gnlp_engine_unregister_operation            (GnlpEngine         *self,
                                                                  GType               type);

GList*               gnlp_engine_list_operations                 (GnlpEngine         *self,
                                                                  const gchar        *language);

//GnlpExtension*      gnlp_engine_create_operation                (GnlpEngine         *self,
//                                                                 const gchar        *name,
//                                                                 const gchar        *language);

G_END_DECLS

#endif /* GNLP_ENGINE_H */

