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
 * Google Author(s): Behdad Esfahbod, Maysum Panju, Wojciech Baranowski
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "demo-glstate.h"

struct demo_glstate_t {
  unsigned int   refcount;

  GLuint program;
  demo_atlas_t *atlas;

  /* Uniforms */
  double u_debug;
  double u_contrast;
  double u_gamma_adjust;
  double u_outline;
  double u_outline_thickness;
  double u_boldness;
};

demo_glstate_t *
demo_glstate_create (QOpenGLExtraFunctions * gl)
{
  TRACE();

  demo_glstate_t *st = (demo_glstate_t *) calloc (1, sizeof (demo_glstate_t));
  st->refcount = 1;

  st->program = demo_shader_create_program (gl);
  st->atlas = demo_atlas_create (gl, 2048, 1024, 64, 8);

  st->u_debug = false;
  st->u_contrast = 1.0;
  st->u_gamma_adjust = 1.0;
  st->u_outline = false;
  st->u_outline_thickness = 1.0;
  st->u_boldness = 0.;

  return st;
}

demo_glstate_t *
demo_glstate_reference (demo_glstate_t *st)
{
  if (st) st->refcount++;
  return st;
}

void
demo_glstate_destroy (QOpenGLExtraFunctions * gl, demo_glstate_t *st)
{
  if (!st || --st->refcount)
    return;

  demo_atlas_destroy (gl, st->atlas);
  gl->glDeleteProgram (st->program);

  free (st);
}


static void
set_uniform (QOpenGLExtraFunctions * gl, GLuint program, const char *name, double *p, double value)
{
  *p = value;
  gl->glUniform1f (gl->glGetUniformLocation (program, name), value);
  LOGI ("Setting %s to %g", name + 2, value);
}

#define SET_UNIFORM(gl, name, value) set_uniform (gl, st->program, #name, &st->name, value)

void
demo_glstate_setup (QOpenGLExtraFunctions * gl, demo_glstate_t *st)
{
  gl->glUseProgram (st->program);

  demo_atlas_set_uniforms (gl, st->atlas);

  SET_UNIFORM (gl, u_debug, st->u_debug);
  SET_UNIFORM (gl, u_contrast, st->u_contrast);
  SET_UNIFORM (gl, u_gamma_adjust, st->u_gamma_adjust);
  SET_UNIFORM (gl, u_outline, st->u_outline);
  SET_UNIFORM (gl, u_outline_thickness, st->u_outline_thickness);
  SET_UNIFORM (gl, u_boldness, st->u_boldness);

  gl->glEnable (GL_BLEND);
  gl->glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

demo_atlas_t *
demo_glstate_get_atlas (demo_glstate_t *st)
{
  return st->atlas;
}

void
demo_glstate_scale_gamma_adjust (QOpenGLExtraFunctions * gl, demo_glstate_t *st, double factor)
{
  SET_UNIFORM (gl, u_gamma_adjust, clamp (st->u_gamma_adjust * factor, .1, 10.));
}

void
demo_glstate_scale_contrast (QOpenGLExtraFunctions * gl, demo_glstate_t *st, double factor)
{
  SET_UNIFORM (gl, u_contrast, clamp (st->u_contrast * factor, .1, 10.));
}

void
demo_glstate_toggle_debug (QOpenGLExtraFunctions * gl, demo_glstate_t *st)
{
  SET_UNIFORM (gl, u_debug, 1 - st->u_debug);
}

void
demo_glstate_set_matrix (QOpenGLExtraFunctions * gl, demo_glstate_t *st, float mat[16])
{
  gl->glUniformMatrix4fv (gl->glGetUniformLocation (st->program, "u_matViewProjection"), 1, GL_FALSE, mat);
}

void
demo_glstate_toggle_outline (QOpenGLExtraFunctions * gl, demo_glstate_t *st)
{
  SET_UNIFORM (gl, u_outline, 1 - st->u_outline);
}

void
demo_glstate_scale_outline_thickness (QOpenGLExtraFunctions * gl, demo_glstate_t *st, double factor)
{
  SET_UNIFORM (gl, u_outline_thickness, clamp (st->u_outline_thickness * factor, .5, 3.));
}

void
demo_glstate_adjust_boldness (QOpenGLExtraFunctions * gl, demo_glstate_t *st, double adjustment)
{
  SET_UNIFORM (gl, u_boldness, clamp (st->u_boldness + adjustment, -.2, .7));
}
