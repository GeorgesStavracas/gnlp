/* gnlp-extension.c
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

#include "gnlp-extension.h"

G_DEFINE_INTERFACE (GnlpExtension, gnlp_extension, G_TYPE_OBJECT)

static void
gnlp_extension_default_init (GnlpExtensionInterface *iface)
{
  /**
   * GnlpExtension::status
   *
   * The status of the operation.
   */
  g_object_interface_install_property (iface,
                                       g_param_spec_int ("status",
                                                         "Status",
                                                         "The status of the operation",
                                                         0,
                                                         4,
                                                         0,
                                                         G_PARAM_READABLE));

  /**
   * GnlpExtension::name
   *
   * The name of the operation.
   */
  g_object_interface_install_property (iface,
                                       g_param_spec_string ("extension-name",
                                                            "Extension name",
                                                            "The name of the extension",
                                                            "",
                                                            G_PARAM_READABLE));
}

/**
 * gnlp_extension_get_status:
 * @self: a #GnlpExtension
 *
 * Retrieves the ongoing status of @self.
 *
 * Returns: an integer ranging from 0 to 4, inclusively.
 */
gint
gnlp_extension_get_status (GnlpExtension *self)
{
  g_return_val_if_fail (GNLP_IS_EXTENSION (self), FALSE);

  return GNLP_EXTENSION_GET_IFACE (self)->status;
}

/**
 * gnlp_extension_get_name:
 * @self: a #GnlpExtension
 *
 * Retrieves the name of @self.
 *
 * Returns: (nullable): a string.
 */
const gchar*
gnlp_extension_get_name (GnlpExtension *self)
{
  g_return_val_if_fail (GNLP_IS_EXTENSION (self), FALSE);

  return GNLP_EXTENSION_GET_IFACE (self)->name;
}

/**
 * gnlp_extension_set_input:
 * @self: a #GnlpExtension
 * @input: (nullable): a #GVariant
 *
 * Sets the input of @self to @input. Any validation should be done
 * here.
 *
 * Returns: %TRUE if the input is value, %FALSE otherwise.
 */
gboolean
gnlp_extension_set_input (GnlpExtension *self,
                          GVariant      *input)
{
  g_return_val_if_fail (GNLP_IS_EXTENSION (self), FALSE);
  g_return_val_if_fail (GNLP_EXTENSION_GET_IFACE (self)->set_input, FALSE);

  return GNLP_EXTENSION_GET_IFACE (self)->set_input (self, input);
}

/**
 * gnlp_extension_run:
 * @self: a #GnlpExtension
 *
 * Run the operation.
 *
 * Returns: %TRUE if the run was successful, %FALSE otherwise.
 */
gboolean
gnlp_extension_run (GnlpExtension *self)
{
  g_return_val_if_fail (GNLP_IS_EXTENSION (self), FALSE);
  g_return_val_if_fail (GNLP_EXTENSION_GET_IFACE (self)->run, FALSE);

  return GNLP_EXTENSION_GET_IFACE (self)->run (self);
}

/**
 * gnlp_extension_cancel:
 * @self: a #GnlpExtension
 *
 * Cancel the ongoing operation.
 *
 * Returns: %TRUE if the canceling was successful, %FALSE otherwise.
 */
gboolean
gnlp_extension_cancel (GnlpExtension *self)
{
  g_return_val_if_fail (GNLP_IS_EXTENSION (self), FALSE);
  g_return_val_if_fail (GNLP_EXTENSION_GET_IFACE (self)->cancel, FALSE);

  return GNLP_EXTENSION_GET_IFACE (self)->cancel (self);
}

/**
 * gnlp_extension_get_output:
 * @self: a #GnlpExtension
 * @output: a return location for the generated output
 * @error: a return location for an eventual error
 *
 * Retrieve the output of the finished operation.
 *
 * Returns: %TRUE if the retrieving the output was successful, %FALSE otherwise.
 */
gboolean
gnlp_extension_get_output(GnlpExtension  *self,
                          GVariant      **output,
                          GError        **error)
{
  g_return_val_if_fail (GNLP_IS_EXTENSION (self), FALSE);
  g_return_val_if_fail (GNLP_EXTENSION_GET_IFACE (self)->get_output, FALSE);

  return GNLP_EXTENSION_GET_IFACE (self)->get_output (self, output, error);
}
