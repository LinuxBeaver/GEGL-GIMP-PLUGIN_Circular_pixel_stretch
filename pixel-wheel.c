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
 * Credit to Øyvind Kolås (pippin) for major GEGL contributions
 * Beaver Pixel Stretch
 */

/*
Recreation of GEGL Graph from late 2021 early 2022 graph. If you put this syntax
in Gimp's GEGL graph you can test the filter without installing it.

gegl:gaussian-blur std-dev-x=1500
gegl:opacity value=10.00
gegl:opacity value=10.00
polar-coordinates
 */

#include "config.h"
#include <glib/gi18n-lib.h>

#ifdef GEGL_PROPERTIES


#define TUTORIAL \
" gegl:gaussian-blur std-dev-x=1500 opacity value=10 opacity value=10  :\n"\



property_double (zoom, _("Zoom"), 0.0)
    description (_("Rescale overall image size"))
    value_range (-100, 100.0)

property_boolean (disablepolar, _("Normal Pixel Stretch"), FALSE)
  description    (_("Make a pixel stretch wheel or square"))


property_double (depth, _("0 for Square - 100 for Circle"), 100.0)
  description    (_("Square to circle transition"))
  value_range (0.0, 100.0)
  ui_meta     ("unit", "percent")
  ui_meta     ("sensitive", "! disablepolar")


property_int  (radius, _("Smooth to hide one pixel slit"), 1)
  value_range (0, 6)
  ui_range    (0, 6)
  ui_meta     ("unit", "pixel-distance")
  ui_meta     ("sensitive", "! disablepolar")
  description (_("Median blur takes care of the occasional one pixel slit"))



#else

#define GEGL_OP_META
#define GEGL_OP_NAME     pixel_wheel
#define GEGL_OP_C_SOURCE pixel-wheel.c

#include "gegl-op.h"


typedef struct
{
  GeglNode *input;
  GeglNode *zoom;
  GeglNode *stretch;
  GeglNode *polar;
  GeglNode *med;
  GeglNode *output;
} State;



static void attach (GeglOperation *operation)
{
  GeglNode *gegl = operation->node;
  GeglProperties *o = GEGL_PROPERTIES (operation);
  GeglNode *input, *output, *zoom, *stretch, *polar, *med;



  input    = gegl_node_get_input_proxy (gegl, "input");
  output   = gegl_node_get_output_proxy (gegl, "output");

  stretch    = gegl_node_new_child (gegl,
                                  "operation", "gegl:gegl", "string", TUTORIAL,
                                  NULL);


  zoom    = gegl_node_new_child (gegl,
                                  "operation", "gegl:lens-distortion",
                                  NULL);

   med    = gegl_node_new_child (gegl,
                                  "operation", "gegl:median-blur",
                                  NULL);

      polar = gegl_node_new_child (gegl,
                                  "operation", "gegl:polar-coordinates",
                                  NULL);

      gegl_operation_meta_redirect (operation, "zoom", zoom, "zoom");
      gegl_operation_meta_redirect (operation, "radius", med, "radius");
      gegl_operation_meta_redirect (operation, "depth", polar, "depth");


      gegl_node_link_many (input, zoom, stretch, /* polar, */ med, output, NULL);



 /* Now save points to the various gegl nodes so we can rewire them in
   * update_graph() later
   */
  State *state = g_malloc0 (sizeof (State));
  state->input = input;
  state->zoom = zoom;
  state->stretch = stretch;
  state->polar = polar;
  state->med = med;
  state->output = output;
  o->user_data = state;
}

static void
update_graph (GeglOperation *operation)
{
  GeglProperties *o = GEGL_PROPERTIES (operation);
  State *state = o->user_data;
  if (!state) return;

  if (o->disablepolar)
  {
    gegl_node_link_many (state->zoom, state->stretch, state->med, state->output, NULL);
  }
  else
  {
    gegl_node_link_many (state->zoom, state->stretch, state->polar, state->med, state->output, NULL);
  }
}


static void
gegl_op_class_init (GeglOpClass *klass)
{
  GeglOperationClass *operation_class = GEGL_OPERATION_CLASS (klass);
   GeglOperationMetaClass *operation_meta_class = GEGL_OPERATION_META_CLASS (klass);

  operation_class->attach = attach;
  operation_meta_class->update = update_graph;

  gegl_operation_class_set_keys (operation_class,
    "name",        "lb:pixel-wheel",
    "title",       _("Circular Pixel Stretch"),
    "categories",  "Artistic",
    "reference-hash", "2ah15656a238a5112010dc2544142af",
    "description", _("Make a circular pixel stretch effect "
                     ""),
    NULL);
}

#endif
