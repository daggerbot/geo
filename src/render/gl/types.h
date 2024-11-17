/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef RENDER_GL_TYPES_H_INCLUDED
#define RENDER_GL_TYPES_H_INCLUDED

#include <render/types.h>

using GLbitfield = unsigned;
using GLboolean = unsigned char;
using GLbyte = signed char;
using GLchar = char;
using GLclampd = double;
using GLclampf = float;
using GLclampx = int;
using GLdouble = double;
using GLenum = unsigned;
using GLfixed = int;
using GLfloat = float;
using GLhalf = unsigned short;
using GLint = int;
using GLint64 = geo::i64;
using GLintptr = geo::iptr;
using GLshort = short;
using GLsizeiptr = geo::ssize_t;
using GLsizei = int;
using GLubyte = unsigned char;
using GLuint = unsigned;
using GLuint64 = geo::u64;
using GLushort = unsigned short;
using GLvoid = void;

#endif // RENDER_GL_TYPES_H_INCLUDED
