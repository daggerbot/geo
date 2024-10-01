/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef GEO_RENDER_SYSTEM_H_INCLUDED
#define GEO_RENDER_SYSTEM_H_INCLUDED

#include <geo/graphics/color.h>

namespace geo {

    class GlLoader;

    /// @addtogroup geo_render
    /// @{

    /// Global rendering subsystem.
    class RenderSystem {
    public:
        RenderSystem(const RenderSystem&) = delete;
        RenderSystem& operator=(const RenderSystem&) = delete;

        void initialize();
        void shut_down();

        /// Clears the color buffer to the specified color.
        void clear(Col4f color = 0);

        /// Gets the global render system instance.
        static RenderSystem& get();

        /// (OpenGL only) Sets the OpenGL symbol loader. This must be set before calling
        /// @ref initialize.
        void set_gl_loader(GlLoader* loader) { m_glLoader = loader; }

    private:
        GlLoader* m_glLoader = nullptr;

        RenderSystem();
        ~RenderSystem();
    };

    /// @}

} // namespace geo

#endif // GEO_RENDER_SYSTEM_H_INCLUDED
