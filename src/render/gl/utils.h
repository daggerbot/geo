/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef RENDER_GL_UTILS_H_INCLUDED
#define RENDER_GL_UTILS_H_INCLUDED

#include <string>

#include "types.h"

namespace geo {

    /// OpenGL utility functions.
    namespace gl {

        void flush_errors();
        std::string strerror(GLenum errnum);

    } // namespace gl

} // namespace geo

#endif // RENDER_GL_UTILS_H_INCLUDED
