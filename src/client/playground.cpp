/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <render/buffer.h>
#include <render/geometry.h>
#include <render/render.h>
#include <system/debug.h>

#include "playground.h"

using namespace geo;

void Playground::begin_state()
{
    static const render::ColorVertex triangle[] = {
        {{-0.9f, -0.9f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
        {{0.9f, -0.9f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
        {{0.0f, 0.9f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},
    };

    auto vertex_buffer = std::make_shared<render::Buffer>(sizeof(triangle),
                                                          render::BufferType::vertex,
                                                          render::BufferUsage::static_draw);

    if (vertex_buffer->is_empty())
        FATAL("Not enough VRAM");

    vertex_buffer->write(triangle, 0, sizeof(triangle));

    geometry_ = std::make_shared<render::Geometry>(render::VertexLayout::color);
    geometry_->set_vertex_buffer(0, vertex_buffer);
}

void Playground::render(u32)
{
    render::clear_color_buffer();
    render::set_full_viewport();
    render::select_shader(render::Shader::color);
    render::select_geometry(geometry_);
    render::draw_triangles(0, 3);
}
