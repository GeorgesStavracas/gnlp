/* gnlp-listener.h
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

#ifndef GNLP_LISTENER_H
#define GNLP_LISTENER_H

#include "gnlp-context.h"

#include <glib-object.h>

G_BEGIN_DECLS

typedef enum
{
  GNLP_LISTENER_MODE_NONE,
  GNLP_LISTENER_MODE_COMMAND,
  GNLP_LISTENER_MODE_FREE_SPEECH,
  GNLP_LISTENER_MODE_QUESTION
} GnlpListenerMode;

#define GNLP_TYPE_LISTENER (gnlp_listener_get_type())

G_DECLARE_FINAL_TYPE (GnlpListener, gnlp_listener, GNLP, LISTENER, GObject)

GnlpListener*        gnlp_listener_new                           (GnlpContext        *context);

void                 gnlp_listener_run                           (GnlpListener       *self);

GnlpListenerMode     gnlp_listener_get_mode                      (GnlpListener       *self);

void                 gnlp_listener_set_mode                      (GnlpListener       *self,
                                                                  GnlpListenerMode    mode);

G_END_DECLS

#endif /* GNLP_LISTENER_H */

