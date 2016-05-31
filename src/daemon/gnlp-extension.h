/* gnlp-extension.h
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

#ifndef GNLP_EXTENSION_H
#define GNLP_EXTENSION_H

#include <glib-object.h>

G_BEGIN_DECLS

#define GNLP_TYPE_EXTENSION (gnlp_extension_get_type ())

G_DECLARE_INTERFACE (GnlpExtension, gnlp_extension, GNLP, EXTENSION, GObject)

struct _GnlpExtensionInterface
{
  GTypeInterface       parent;

  gint                 status;

  const gchar*         name;

  gboolean           (*set_input)                                (GnlpExtension      *self,
                                                                  GVariant           *input);

  gboolean           (*run)                                      (GnlpExtension      *self);

  gboolean           (*cancel)                                   (GnlpExtension      *self);

  gboolean           (*get_output)                               (GnlpExtension      *self,
                                                                  GVariant          **output,
                                                                  GError            **error);
};

gint                 gnlp_extension_get_status                   (GnlpExtension      *self);

const gchar*         gnlp_extension_get_name                     (GnlpExtension      *self);

gboolean             gnlp_extension_set_input                    (GnlpExtension      *self,
                                                                  GVariant           *input);

gboolean             gnlp_extension_run                          (GnlpExtension      *self);

gboolean             gnlp_extension_cancel                       (GnlpExtension      *self);

gboolean             gnlp_extension_get_output                   (GnlpExtension      *self,
                                                                  GVariant          **output,
                                                                  GError            **error);

G_END_DECLS

#endif /* GNLP_EXTENSION_H */
