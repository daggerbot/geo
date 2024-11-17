/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef RENDER_GL_RENDER_H_INCLUDED
#define RENDER_GL_RENDER_H_INCLUDED

#include <render/render.h>

#include "shaders.h"

namespace geo::render::state {

    extern Shader current_shader;
    extern GlProgram* current_program;
    extern std::shared_ptr<GeometryBase> current_geometry;

    extern Mat4f projection;
    extern Mat4f object_transform;

} // namespace geo::render::state

#endif // RENDER_GL_RENDER_H_INCLUDED
