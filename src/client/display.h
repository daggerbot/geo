/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef GEO_CLIENT_DISPLAY_H_INCLUDED
#define GEO_CLIENT_DISPLAY_H_INCLUDED

#include <memory>

#include <geo/math/vector.h>
#include <geo/render/gl/loader.h>

using SDL_GLContext = void*;
struct SDL_Window;

namespace geo {

    /// @addtogroup geo_client
    /// @{

    /// Game rendering surface.
    class Display : public GlLoader {
    public:
        Display(const Display&) = delete;
        Display& operator=(const Display&) = delete;

        void initialize();
        void shut_down();

        /// Gets the global display instance.
        static Display& get();

        /// Loads an OpenGL symbol.
        void* gl_get_proc_address(const char* name) override;

        /// Returns the size of the rendering surface in pixels.
        Vec2i size() const;

        /// Swaps the front and back buffers.
        void swap_buffers();

    private:
        std::unique_ptr<SDL_Window, void (*)(SDL_Window*)> m_window;
        std::unique_ptr<void, void (*)(SDL_GLContext)> m_glContext;

        Display();
        ~Display();
    };

    /// @}

} // namespace geo

#endif // GEO_CLIENT_DISPLAY_H_INCLUDED
