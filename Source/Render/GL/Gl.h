/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef GEO_RENDER_GL_GL_H_INCLUDED
#define GEO_RENDER_GL_GL_H_INCLUDED

#include <string>

#include "Types.h"

namespace Geo {

    /// @addtogroup Render
    /// @{

    /// OpenGL API utilities.
    namespace Gl {

        // Uniform locations
        static inline constexpr GLuint ProjectionIdx = 0;
        static inline constexpr GLuint ObjectTransformIdx = 1;

        // Attribute locations
        static inline constexpr GLuint PositionIdx = 0;
        static inline constexpr GLuint Color_ColorIdx = 1;

        /// Reports and clears all set error flags.
        void FlushErrors();

        /// Gets a string describing an OpenGL error code.
        std::string GetErrorString(GLenum errnum);

    } // namespace Gl

    /// @}

} // namespace Geo

#endif // GEO_RENDER_GL_GL_H_INCLUDED
