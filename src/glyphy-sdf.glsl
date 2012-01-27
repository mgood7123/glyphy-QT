/*
 * Copyright 2012 Google, Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Google Author(s): Behdad Esfahbod, Maysum Panju
 */

#ifndef GLYPHY_TEXTURE1D_FUNC
#define GLYPHY_TEXTURE1D_FUNC glyphy_texture1D_func
#endif
#ifndef GLYPHY_TEXTURE1D_EXTRA_DECLS
#define GLYPHY_TEXTURE1D_EXTRA_DECLS
#endif
#ifndef GLYPHY_TEXTURE1D_EXTRA_ARGS
#define GLYPHY_TEXTURE1D_EXTRA_ARGS
#endif

#ifndef GLYPHY_SDF_TEXTURE1D_FUNC
#define GLYPHY_SDF_TEXTURE1D_FUNC GLYPHY_TEXTURE1D_FUNC
#endif
#ifndef GLYPHY_SDF_TEXTURE1D_EXTRA_DECLS
#define GLYPHY_SDF_TEXTURE1D_EXTRA_DECLS GLYPHY_TEXTURE1D_EXTRA_DECLS
#endif
#ifndef GLYPHY_SDF_TEXTURE1D_EXTRA_ARGS
#define GLYPHY_SDF_TEXTURE1D_EXTRA_ARGS GLYPHY_TEXTURE1D_EXTRA_ARGS
#endif
#ifndef GLYPHY_SDF_TEXTURE1D(offset)
#define GLYPHY_SDF_TEXTURE1D(offset) \
	GLYPHY_SDF_TEXTURE1D_FUNC (offset GLYPHY_TEXTURE1D_EXTRA_ARGS)
#endif


float
glyphy(sdf) (vec2 p, int glyph_layout GLYPHY_SDF_TEXTURE1D_EXTRA_DECLS)
{
  ivec2 grid_size = ivec2 (mod (glyph_layout, 256), glyph_layout / 256); // width, height
  ivec2 p_cell = ivec2 (clamp (p, vec2 (0,0), vec2(1,1) * (1.-GLYPHY_EPSILON)) * vec2 (grid_size));

  vec4 arc_list_data = GLYPHY_SDF_TEXTURE1D (p_cell.y * grid_size.x + p_cell.x);
  glyphy(arc_list_t) arc_list = glyphy(arc_list_decode) (arc_list_data);
  float side = arc_list.side;

  int i;
  float min_dist = GLYPHY_INFINITY;
  float min_extended_dist = GLYPHY_INFINITY;

  glyphy(arc_t) closest_arc;

  glyphy(arc_endpoint_t) endpoint_prev, endpoint;
  endpoint_prev = glyphy(arc_endpoint_decode) (GLYPHY_SDF_TEXTURE1D (arc_list.offset));
  for (i = 1; i < arc_list.num_endpoints; i++)
  {
    endpoint = glyphy(arc_endpoint_decode) (GLYPHY_SDF_TEXTURE1D (arc_list.offset + i));
    glyphy(arc_t) a = glyphy(arc_t) (endpoint_prev.p, endpoint.p, endpoint.d);
    endpoint_prev = endpoint;
    if (glyphy(isinf) (a.d)) continue;

    // unsigned distance
    float d2 = glyphy(tan2atan) (a.d);
    if (dot (p - a.p0, (a.p1 - a.p0) * mat2(1,  d2, -d2, 1)) >= 0 &&
	dot (p - a.p1, (a.p1 - a.p0) * mat2(1, -d2,  d2, 1)) <= 0)
    {
      vec2 c = glyphy(arc_center) (a);
      float signed_dist = (distance (p, c) - distance (a.p0, c));
      float dist = abs (signed_dist);
      if (dist <= min_dist) {
	min_dist = dist;
	side = sign (a.d) * (signed_dist >= 0 ? -1 : +1);
      }
    } else {
      float dist = min (distance (p, a.p0), distance (p, a.p1));
      if (dist < min_dist) {
	min_dist = dist;
	side = 0; /* unsure */
	closest_arc = glyphy(arc_t) (a.p0, a.p1, a.d);
      } else if (dist == min_dist && side == 0) {
	// If this new distance is the same as the current minimum, compare extended distances.
	// Take the sign from the arc with larger extended distance.
	float new_extended_dist = glyphy(arc_extended_dist) (p, a);
	float old_extended_dist = glyphy(arc_extended_dist) (p, closest_arc);

	float extended_dist = abs (new_extended_dist) <= abs (old_extended_dist) ?
			      old_extended_dist : new_extended_dist;

	/* For emboldening and stuff: */
	// min_dist = abs (extended_dist);
	side = sign (extended_dist);
      }
    }
  }

  if (side == 0) {
    // Technically speaking this should not happen, but it does.  So try to fix it.
    float extended_dist = glyphy(arc_extended_dist) (p, closest_arc);
    side = sign (extended_dist);
  }

  return min_dist * side;
}