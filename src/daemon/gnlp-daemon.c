/* gnlp-daemon.c
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

#define G_LOG_DOMAIN "Daemon"

#include "gnlp-context-manager.h"
#include "gnlp-daemon.h"
#include "gnlp-dbus-code.h"
#include "gnlp-engine.h"

struct _GnlpDaemon
{
  GApplication        parent;

  GnlpEngine         *engine;
  GnlpManager        *manager;
  GnlpContextManager *context_manager;
  GDBusObjectManagerServer *object_manager;
};

G_DEFINE_TYPE (GnlpDaemon, gnlp_daemon, G_TYPE_APPLICATION)

enum {
  PROP_0,
  N_PROPS
};

/*
 * Context management
 */
static gboolean
handle_create_context (GnlpManager           *manager,
                       GDBusMethodInvocation *invocation,
                       const gchar           *application_id,
                       GnlpDaemon            *daemon)
{
  g_debug ("Creating server-side context for application '%s'", application_id);

  gnlp_context_manager_create_context (daemon->context_manager, application_id);

  gnlp_manager_complete_create_context (manager, invocation);

  return TRUE;
}

static gboolean
handle_destroy_context (GnlpManager           *manager,
                        GDBusMethodInvocation *invocation,
                        const gchar           *application_id,
                        GnlpDaemon            *daemon)
{
  g_debug ("Destroying server-side context for application '%s'", application_id);

  gnlp_context_manager_destroy_context (daemon->context_manager, application_id);

  gnlp_manager_complete_destroy_context (manager, invocation);

  return TRUE;
}

static gboolean
handle_list_operations (GnlpManager           *manager,
                        GDBusMethodInvocation *invocation,
                        const gchar           *language,
                        GnlpDaemon            *daemon)
{
  GList *operations, *aux;
  gchar **array;
  gint index;

  /* Since we can't pass NULL pointers to DBus, we NULLify it here */
  if (g_utf8_strlen (language, -1) == 0)
    language = NULL;

  g_debug ("Looking up operations for language '%s'", language);

  operations = gnlp_engine_list_operations (daemon->engine, language);

  /* Setup the array from the GList */
  array = g_new0 (gchar*, g_list_length (operations) + 1);
  index = 0;

  for (aux = operations; aux != NULL; aux = aux->next)
    {
      array[index] = g_strdup (aux->data);
      index++;
    }

  gnlp_manager_complete_list_operations (manager,
                                         invocation,
                                         (const gchar* const*) array);

  g_list_free_full (operations, g_free);
  g_strfreev (array);

  return TRUE;
}

static gboolean
handle_create_operation (GnlpManager           *manager,
                         GDBusMethodInvocation *invocation,
                         const gchar           *name,
                         const gchar           *language,
                         const gchar           *application_id,
                         GnlpDaemon            *daemon)
{
  GnlpClientContext *client_context;
  GnlpObjectSkeleton *skeleton;
  GnlpOperation *operation;
  gchar *operation_id;
  gchar *object_path;

  client_context = gnlp_context_manager_get_context (daemon->context_manager,
                                                     application_id);

  /* Trying to create an operation for a dead context? Something is wrong */
  if (!client_context)
    {
      gnlp_manager_complete_create_operation (manager,
                                              invocation,
                                              NULL,
                                              FALSE);

      return TRUE;
    }

  operation_id = gnlp_client_context_create_operation (client_context,
                                                       name,
                                                       language);

  object_path = g_strdup_printf ("/org/gnome/Nlp/Operation/%s/%s",
                                 application_id,
                                 operation_id);

  /* Export the operation */
  operation = gnlp_operation_skeleton_new ();

  skeleton = gnlp_object_skeleton_new (object_path);
  gnlp_object_skeleton_set_operation (skeleton, operation);

  g_dbus_object_manager_server_export (daemon->object_manager, G_DBUS_OBJECT_SKELETON (skeleton));

  gnlp_manager_complete_create_operation (manager,
                                          invocation,
                                          operation_id,
                                          TRUE);

  return TRUE;
}

static void
gnlp_daemon_finalize (GObject *object)
{
  GnlpDaemon *self = (GnlpDaemon*) object;

  g_clear_object (&self->engine);

  G_OBJECT_CLASS (gnlp_daemon_parent_class)->finalize (object);
}

/*
 * DBus methods
 */
static gboolean
gnlp_daemon_dbus_register (GApplication     *application,
                           GDBusConnection  *connection,
                           const gchar      *object_path,
                           GError          **error)
{
  GnlpObjectSkeleton *object;
  GnlpDaemon *self;

  self = GNLP_DAEMON (application);

  /* Chain up before anything else */
  if (!G_APPLICATION_CLASS (gnlp_daemon_parent_class)->dbus_register (application,
                                                                      connection,
                                                                      object_path,
                                                                      error))
    {
      return FALSE;
    }

  g_debug ("DBus name registered: \"%s\"", object_path);

  /* Create object manager */
  self->object_manager = g_dbus_object_manager_server_new ("/org/gnome/Nlp");

  /* Context manager */
  self->manager = gnlp_manager_skeleton_new ();

  g_signal_connect (self->manager,
                    "handle-create-context",
                    G_CALLBACK (handle_create_context),
                    self);

  g_signal_connect (self->manager,
                    "handle-destroy-context",
                    G_CALLBACK (handle_destroy_context),
                    self);

  g_signal_connect (self->manager,
                    "handle-list-operations",
                    G_CALLBACK (handle_list_operations),
                    self);

  g_signal_connect (self->manager,
                    "handle-create-operation",
                    G_CALLBACK (handle_create_operation),
                    self);

  /* Export the context manager */
  object = gnlp_object_skeleton_new ("/org/gnome/Nlp/Manager");
  gnlp_object_skeleton_set_manager (object, self->manager);

  g_dbus_object_manager_server_export (self->object_manager, G_DBUS_OBJECT_SKELETON (object));

  /* Setup the connection */
  g_dbus_object_manager_server_set_connection (self->object_manager, connection);

  g_clear_object (&object);

  return TRUE;
}

static void
gnlp_daemon_startup (GApplication *application)
{
  g_debug ("Starting up daemon...");

  /*
   * We don't want the daemon to die after 10s, which is the
   * default behavior for service applications. To avoid that,
   * hold a user count.
   */
  g_application_hold (application);

  /* Hook up to the parent class */
  G_APPLICATION_CLASS (gnlp_daemon_parent_class)->startup (application);
}

static void
gnlp_daemon_class_init (GnlpDaemonClass *klass)
{
  GApplicationClass *application_class = G_APPLICATION_CLASS (klass);
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  application_class->startup = gnlp_daemon_startup;
  application_class->dbus_register = gnlp_daemon_dbus_register;

  object_class->finalize = gnlp_daemon_finalize;
}

static void
gnlp_daemon_init (GnlpDaemon *self)
{
  self->engine = gnlp_engine_new ();
  self->context_manager = gnlp_context_manager_new (self->engine);
}

GnlpDaemon*
gnlp_daemon_new (void)
{
  return g_object_new (GNLP_TYPE_DAEMON,
                       "flags", G_APPLICATION_IS_SERVICE,
                       "application-id", "org.gnome.Nlp",
                       NULL);
}
