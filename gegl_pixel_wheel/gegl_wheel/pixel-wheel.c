/* This file is an image processing operation for GEGL
 *
 * GEGL is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * GEGL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEGL; if not, see <https://www.gnu.org/licenses/>.
 *
 * Copyright 2006 Øyvind Kolås <pippin@gimp.org>
 * Beaver & Liam 2022 - Pixel Stretch
 */

#include "config.h"
#include <glib/gi18n-lib.h>

#ifdef GEGL_PROPERTIES


property_double (zoom, _("Zoom"), 0.0)
    description (_("Rescale overall image size"))
    value_range (-100, 100.0)

property_boolean (polar, _("Circular"), TRUE)
  description    (_("Make a pixel stretch wheel"))


#else

#define GEGL_OP_META
#define GEGL_OP_NAME     pixel_wheel
#define GEGL_OP_C_SOURCE pixel-wheel.c

#include "gegl-op.h"

static void attach (GeglOperation *operation)
{
  GeglNode *gegl = operation->node;
  GeglNode *input, *output, *zoom, *stretch, *polar;



  input    = gegl_node_get_input_proxy (gegl, "input");
  output   = gegl_node_get_output_proxy (gegl, "output");

  stretch    = gegl_node_new_child (gegl,
                                  "operation", "gegl:Zps",
                                  NULL);


  zoom    = gegl_node_new_child (gegl,
                                  "operation", "gegl:lens-distortion",
                                  NULL);



      polar = gegl_node_new_child (gegl,
                                  "operation", "gegl:zpolar-coordinates",
                                  NULL);

      gegl_operation_meta_redirect (operation, "zoom", zoom, "zoom");
      gegl_operation_meta_redirect (operation, "polar", polar, "activate");


      gegl_node_link_many (input, zoom, stretch, polar, output, NULL);


}

static void
gegl_op_class_init (GeglOpClass *klass)
{
  GeglOperationClass *operation_class;

  operation_class = GEGL_OPERATION_CLASS (klass);

  operation_class->attach = attach;

  gegl_operation_class_set_keys (operation_class,
    "name",        "gegl:pixel-wheel",
    "title",       _("Circular Pixel Stretch"),
    "categories",  "Artistic",
    "reference-hash", "2ah15656a238a5112010dc2544142af",
    "description", _("GEGL makes a circular pixel stretch effect "
                     ""),
    NULL);
}

#endif
