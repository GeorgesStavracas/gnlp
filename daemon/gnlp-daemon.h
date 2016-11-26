/* gnlp-daemon.h
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

#ifndef GNLP_DAEMON_H
#define GNLP_DAEMON_H

#include "gnlp-listener.h"

#include <gio/gio.h>

G_BEGIN_DECLS

#define GNLP_TYPE_DAEMON (gnlp_daemon_get_type())

G_DECLARE_FINAL_TYPE (GnlpDaemon, gnlp_daemon, GNLP, DAEMON, GApplication)

GnlpDaemon*          gnlp_daemon_new                             (void);

GnlpListener*        gnlp_daemon_get_listener                    (GnlpDaemon         *self);

G_END_DECLS

#endif /* GNLP_DAEMON_H */

