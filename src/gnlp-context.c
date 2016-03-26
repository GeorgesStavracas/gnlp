/* gnlp-context.c
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

#include "gnlp-context.h"
#include "daemon/gnlp-dbus-code.h"

#include <glib/gi18n.h>

#define              GNLP_MANAGER_DBUS_PATH                      "/org/gnome/Nlp/Manager"

struct _GnlpContext
{
  GObject             parent;

  gchar              *application_id;

  GDBusObjectManager *object_manager;

  /* Internal data */
  gboolean            is_initialized;
  GError             *initialization_error;
};

static void          gnlp_context_initable_iface_init            (GInitableIface     *iface);

G_DEFINE_TYPE_WITH_CODE (GnlpContext, gnlp_context, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (G_TYPE_INITABLE, gnlp_context_initable_iface_init)
                         G_IMPLEMENT_INTERFACE (G_TYPE_ASYNC_INITABLE, NULL))

enum {
  PROP_0,
  PROP_APPLICATION_ID,
  N_PROPS
};

static GParamSpec *properties [N_PROPS] = {NULL, };

static GnlpManager*
get_manager (GnlpContext *self)
{
  GnlpManager *manager;
  GDBusObject *object;

  object = g_dbus_object_manager_get_object (self->object_manager, GNLP_MANAGER_DBUS_PATH);

  if (!object)
    return NULL;

  manager = gnlp_object_peek_manager (GNLP_OBJECT (object));

  g_clear_object (&object);

  return manager;
}

static GList*
strv_to_glist (gchar **strv)
{
  GList *list;
  gint index;

  list = NULL;

  if (!strv)
    return NULL;

  for (index = 0; strv[index]; index++)
    list = g_list_prepend (list, g_strdup (strv[index]));

  return list;
}

/*
 * GInitable iface implementation
 */
G_LOCK_DEFINE_STATIC (init_lock);

static gboolean
gnlp_context_initable_init (GInitable     *initable,
                            GCancellable  *cancellable,
                            GError       **error)
{
  GnlpContext *self;
  gboolean valid;

  self = GNLP_CONTEXT (initable);
  valid = FALSE;

  G_LOCK (init_lock);

  if (self->is_initialized)
    {
      /*
       * If we already initialized the object, it MUST have the
       * object manager, otherwise it's invalid and should return
       * NULL.
       */
      valid = self->object_manager != NULL;
    }
  else
    {
      self->object_manager = gnlp_object_manager_client_new_for_bus_sync (G_BUS_TYPE_SESSION,
                                                                          G_DBUS_OBJECT_MANAGER_CLIENT_FLAGS_NONE,
                                                                          "org.gnome.Nlp",
                                                                          "/org/gnome/Nlp",
                                                                          cancellable,
                                                                          &self->initialization_error);

      /*
       * If something happened and the object returned NULL, we simply
       * don't set valid to TRUE.
       */
      if (self->object_manager)
        {
          GnlpManager *manager;

          manager = get_manager (self);

          if (manager)
            {
              g_debug ("Registering the context for application '%s'", self->application_id);

              valid = gnlp_manager_call_create_context_sync (manager,
                                                             self->application_id,
                                                             cancellable,
                                                             &self->initialization_error);
            }
          else
            {
              /*
               * Is the previous call returned NULL, almost certainly the
               * gnlp daemon is not running.
               */
              g_set_error (&self->initialization_error,
                           G_DBUS_ERROR,
                           G_DBUS_ERROR_FAILED,
                           "%s", _("The manager object is unavailable, check if Gnlp daemon is running"));
            }
        }
    }

  self->is_initialized = TRUE;

  if (!valid)
    g_propagate_error (error, g_error_copy (self->initialization_error));

  G_UNLOCK (init_lock);

  return valid;
}

static void
gnlp_context_initable_iface_init (GInitableIface *iface)
{
  iface->init = gnlp_context_initable_init;
}

static void
gnlp_context_finalize (GObject *object)
{
  GnlpContext *self = GNLP_CONTEXT (object);
  GnlpManager *manager = get_manager (self);

  gnlp_manager_call_destroy_context (manager,
                                     self->application_id,
                                     NULL,
                                     NULL,
                                     NULL);

  g_clear_object (&self->object_manager);
  g_clear_pointer (&self->application_id, g_free);

  G_OBJECT_CLASS (gnlp_context_parent_class)->finalize (object);
}

static void
gnlp_context_get_property (GObject    *object,
                           guint       prop_id,
                           GValue     *value,
                           GParamSpec *pspec)
{
  GnlpContext *self = GNLP_CONTEXT (object);

  switch (prop_id)
    {
    case PROP_APPLICATION_ID:
      g_value_set_string (value, self->application_id);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gnlp_context_set_property (GObject      *object,
                           guint         prop_id,
                           const GValue *value,
                           GParamSpec   *pspec)
{
  GnlpContext *self = GNLP_CONTEXT (object);

  switch (prop_id)
    {
    case PROP_APPLICATION_ID:
      self->application_id = g_value_dup_string (value);
      g_object_notify_by_pspec (object, properties[PROP_APPLICATION_ID]);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gnlp_context_class_init (GnlpContextClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gnlp_context_finalize;
  object_class->get_property = gnlp_context_get_property;
  object_class->set_property = gnlp_context_set_property;

  properties[PROP_APPLICATION_ID] = g_param_spec_string ("application-id",
                                                         "Application identifier",
                                                         "The unique application identifier",
                                                         NULL,
                                                         G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
gnlp_context_init (GnlpContext *self)
{
}

/**
 * gnlp_context_new:
 * @application_id: the application id for this context
 * @cancellable: (nullable): optional #GCancellable object
 * @callback: (scope async): a #GAsyncReadyCallback to call when the initialization is done
 * @user_data: (closure): data to pass to the callback function
 *
 * Asynchronously creates a new #GnlpContext. When it finishes, @callback will
 * be called.
 *
 * Since: 0.1.0
 */
void
gnlp_context_new (const gchar         *application_id,
                  GCancellable        *cancellable,
                  GAsyncReadyCallback  callback,
                  gpointer             user_data)
{
  g_return_if_fail (g_utf8_strlen (application_id, -1) > 0);

  g_async_initable_new_async (GNLP_TYPE_CONTEXT,
                              G_PRIORITY_DEFAULT,
                              cancellable,
                              callback,
                              user_data,
                              "application-id", application_id,
                              NULL);
}

/**
 * gnlp_context_new_finish:
 * @result: a #GAsyncResult
 * @error: (nullable): return location for a #GError
 *
 * Finishes an asynchronous initialization started in gnlp_context_new(). If
 * an error occurs, the functions sets @error and returns %NULL.
 *
 * Returns: (transfer full) (nullable): a #GnlpContext
 *
 * Since: 0.1.0
 */
GnlpContext*
gnlp_context_new_finish (GAsyncResult  *result,
                         GError       **error)
{
  GObject *context;
  GObject *source;

  source = g_async_result_get_source_object (result);
  context = g_async_initable_new_finish (G_ASYNC_INITABLE (source), result, error);

  g_clear_object (&source);

  return context ? GNLP_CONTEXT (context) : NULL;
}

/**
 * gnlp_context_new_sync:
 * @application_id: the application id for this context
 * @cancellable: (nullable): optional #GCancellable object
 * @error: (nullable): return location for a #GError
 *
 * Initializes a #GnlpContext exactly like the asynchronous gnlp_context_new(),
 * but in a synchronous way. This version will block the running thread.
 *
 * Returns: (transfer full) (nullable): a #GnlpContext
 *
 * Since: 0.1.0
 */
GnlpContext*
gnlp_context_new_sync (const gchar   *application_id,
                       GCancellable  *cancellable,
                       GError       **error)
{
  GInitable *context;

  g_return_val_if_fail (g_utf8_strlen (application_id, -1) > 0, NULL);

  context = g_initable_new (GNLP_TYPE_CONTEXT,
                            cancellable,
                            error,
                            "application-id", application_id,
                            NULL);

  return context ? GNLP_CONTEXT (context) : NULL;
}

/**
 * gnlp_context_list_operations:
 * @self: a #GnlpContext
 * @language: (nullable): the desired language
 * @cancellable: (nullable): optional #GCancellable object
 * @callback: (scope async): a #GAsyncReadyCallback to call when the initialization is done
 * @user_data: (closure): data to pass to the callback function
 *
 * List all the operations available. An operation is anything that receives
 * input and generates meaningful output.
 *
 * The list is not sorted.
 *
 * Since: 0.1.0
 */
void
gnlp_context_list_operations (GnlpContext         *self,
                              const gchar         *language,
                              GCancellable        *cancellable,
                              GAsyncReadyCallback  callback,
                              gpointer             user_data)
{
  GnlpManager *manager;

  g_return_if_fail (GNLP_IS_CONTEXT (self));

  manager = get_manager (self);

  if (manager)
    {
      gnlp_manager_call_list_operations (manager,
                                         language ? language : "",
                                         cancellable,
                                         callback,
                                         user_data);
    }
}

/**
 * gnlp_context_list_operations_finish:
 * @result: a #GAsyncResult
 * @error: (nullable): return location for a #GError
 *
 * Finishes an asynchronous initialization started in gnlp_context_list_operations(). If
 * an error occurs, the functions sets @error and returns %NULL.
 *
 * Returns: (element-type utf8) (transfer full) (nullable): a #GnlpContext. The strings
 * themselves are duplicated, free the return value with g_list_free_full().
 *
 * Since: 0.1.0
 */
GList*
gnlp_context_list_operations_finish (GAsyncResult  *result,
                                     GError       **error)
{
  GnlpManager *manager;
  gboolean success;
  GList *list;
  gchar **operations;

  manager = GNLP_MANAGER (g_async_result_get_source_object (result));
  success = gnlp_manager_call_list_operations_finish (manager, &operations, result, error);

  if (!success)
    return NULL;

  list = strv_to_glist (operations);

  g_clear_pointer (&operations, g_strfreev);

  return list;
}

/**
 * gnlp_context_list_operations_sync:
 * @self: a #GnlpContext
 * @language: the desired language override
 * @cancellable: (nullable): optional #GCancellable object
 * @error: (nullable): return location for a #GError
 *
 * List all the operations available exactly like gnlp_context_list_operations(), but
 * synchronously. This version will block the running thread.
 *
 * Returns: (element-type utf8) (transfer full) (nullable): a #GnlpContext. The strings
 * themselves are duplicated, free the return value with g_list_free_full().
 *
 * Since: 0.1.0
 */
GList*
gnlp_context_list_operations_sync (GnlpContext   *self,
                                   const gchar   *language,
                                   GCancellable  *cancellable,
                                   GError       **error)
{
  GnlpManager *manager;

  g_return_val_if_fail (GNLP_IS_CONTEXT (self), NULL);

  manager = get_manager (self);

  if (manager)
    {
      gboolean success;
      gchar **operations;
      GList *list;

      success = gnlp_manager_call_list_operations_sync (manager,
                                                        language ? language : "",
                                                        &operations,
                                                        cancellable,
                                                        error);

      if (!success)
        return NULL;

      list = strv_to_glist (operations);

      g_clear_pointer (&operations, g_strfreev);

      return list;
    }

  return NULL;
}
