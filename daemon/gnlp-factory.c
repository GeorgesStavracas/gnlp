/* gnlp-factory.c
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

#include "gnlp-factory.h"

struct _GnlpFactory
{
  GObject             parent;
};

G_DEFINE_TYPE (GnlpFactory, gnlp_factory, G_TYPE_OBJECT)

enum {
  PROP_0,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

GnlpFactory *
gnlp_factory_new (void)
{
  return g_object_new (GNLP_TYPE_FACTORY, NULL);
}

static void
gnlp_factory_finalize (GObject *object)
{
  GnlpFactory *self = (GnlpFactory *)object;

  G_OBJECT_CLASS (gnlp_factory_parent_class)->finalize (object);
}

static void
gnlp_factory_get_property (GObject    *object,
                           guint       prop_id,
                           GValue     *value,
                           GParamSpec *pspec)
{
  GnlpFactory *self = GNLP_FACTORY (object);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gnlp_factory_set_property (GObject      *object,
                           guint         prop_id,
                           const GValue *value,
                           GParamSpec   *pspec)
{
  GnlpFactory *self = GNLP_FACTORY (object);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gnlp_factory_class_init (GnlpFactoryClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gnlp_factory_finalize;
  object_class->get_property = gnlp_factory_get_property;
  object_class->set_property = gnlp_factory_set_property;
}

static void
gnlp_factory_init (GnlpFactory *self)
{
}
