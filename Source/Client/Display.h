/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef GEO_CLIENT_DISPLAY_H_INCLUDED
#define GEO_CLIENT_DISPLAY_H_INCLUDED

#include <Math/Vector.h>

namespace Geo {

    /// @addtogroup Client
    /// @{

    /// Interface for managing the render window.
    class Display {
    public:
        virtual ~Display() = 0;

        Display& operator=(const Display&) = delete;

        /// Returns the width and height of the render window's client area in pixels.
        virtual Vec2i GetSize() const = 0;

        /// Swaps the front and back buffers, making the rendered frame visible.
        virtual void SwapBuffers() = 0;

        /// (OpenGL only) Gets a named OpenGL API function pointer.
        static void* gl_GetProcAddress(const char* name);
    };

    inline Display::~Display() = default;

    /// @}

} // namespace Geo

#endif // GEO_CLIENT_DISPLAY_H_INCLUDED
