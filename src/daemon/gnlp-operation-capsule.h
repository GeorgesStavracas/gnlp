/* gnlp-operation-capsule.h
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

#ifndef GNLP_OPERATION_CAPSULE_H
#define GNLP_OPERATION_CAPSULE_H

#include "gnlp-dbus-code.h"
#include "gnlp-extension.h"

#include <glib-object.h>

G_BEGIN_DECLS

#define GNLP_TYPE_OPERATION_CAPSULE (gnlp_operation_capsule_get_type())

G_DECLARE_FINAL_TYPE (GnlpOperationCapsule, gnlp_operation_capsule, GNLP, OPERATION_CAPSULE, GObject)

GnlpOperationCapsule* gnlp_operation_capsule_new                 (GnlpExtension      *extension,
                                                                  GnlpOperation      *skeleton);

G_END_DECLS

#endif /* GNLP_OPERATION_CAPSULE_H */

