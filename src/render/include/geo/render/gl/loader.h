/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef GEO_RENDER_GL_LOADER_H_INCLUDED
#define GEO_RENDER_GL_LOADER_H_INCLUDED

#include <geo/core/types.h>

namespace geo {

    /// @addtogroup geo_render
    /// @{

    /// Interface for loading OpenGL symbols.
    class GlLoader {
    public:
        virtual ~GlLoader() = 0;
        GlLoader& operator=(const GlLoader&) = delete;
        virtual void* gl_get_proc_address(const char* name) = 0;
    };

    inline GlLoader::~GlLoader() = default;

    /// @}

} // namespace geo

#endif // GEO_RENDER_GL_LOADER_H_INCLUDED
