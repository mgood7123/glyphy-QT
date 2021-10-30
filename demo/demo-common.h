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

#ifndef DEMO_COMMON_H
#define DEMO_COMMON_H

#include <glyphy.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

#include <algorithm>
#include <vector>

#include <QtOpenGL>
#include <QOpenGLExtraFunctions>
#include <QKeyEvent>
#include <QMouseEvent>

/* Logging. */
#define LOGI(...) ((void) qDebug (__VA_ARGS__))
#define LOGW(...) ((void) qDebug (__VA_ARGS__))
#define LOGE(...) ((void) qDebug (__VA_ARGS__), abort ())

#define STRINGIZE1(Src) #Src
#define STRINGIZE(Src) STRINGIZE1(Src)

#define ARRAY_LEN(Array) (sizeof (Array) / sizeof (*Array))

#define MIN_FONT_SIZE 10
#define TOLERANCE (1./2048)

#define gl(glPtr, name) \
	for (GLint __ee, __ii = 0; \
	     __ii < 1; \
	     (__ii++, \
	      (__ee = glGetError()) && \
          (qDebug("gl" #name " failed with error %04X on line %d", __ee, __LINE__), abort (), 0))) \
      glPtr->gl##name


static inline void
die (const char *msg)
{
  qFatal("%s", msg);
}

template <typename T>
T clamp (T v, T m, T M)
{
  return v < m ? m : v > M ? M : v;
}


#if defined(_MSC_VER)
#define DEMO_FUNC __FUNCSIG__
#else
#define DEMO_FUNC __func__
#endif

struct auto_trace_t
{
  auto_trace_t (const char *func_) : func (func_)
  { qDebug() << "Enter:" << func; }

  ~auto_trace_t (void)
  { qDebug() << "Leave:" << func; }

  private:
  const char * const func;
};

#define TRACE() auto_trace_t trace(DEMO_FUNC)

#endif /* DEMO_COMMON_H */
