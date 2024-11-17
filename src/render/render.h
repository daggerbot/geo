/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef RENDER_RENDER_H_INCLUDED
#define RENDER_RENDER_H_INCLUDED

#include <memory>

#include <graphics/rgb.h>
#include <math/matrix.h>
#include <math/rect.h>

#include "types.h"

namespace geo {

    class StreamProvider;

    /// Rendering subsystem.
    namespace render {

        class GeometryBase;

        void init(StreamProvider& data_source);
        void shut_down();

        void begin_draw();
        void end_draw();

        void clear_color_buffer(Rgbaf color = {});
        void clear_depth_buffer(f32 depth = 0.0f);

        void set_viewport(Recti viewport);
        void set_full_viewport();

        void select_shader(Shader shader);
        void select_geometry(const std::shared_ptr<GeometryBase>& geometry);
        void set_projection(const Mat4f& matrix);
        void set_object_transform(const Mat4f& matrix);

        void draw_triangles(size_t first_vertex_index, size_t vertex_count);

    } // namespace render

} // namespace geo

#endif // RENDER_RENDER_H_INCLUDED
