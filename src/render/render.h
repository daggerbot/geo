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

#include "gl/types.h"

namespace geo {

    class StreamProvider;

    /// Rendering subsystem.
    namespace render {

        void init(StreamProvider& data_source);
        void shut_down();

        void begin_draw();
        void end_draw();
        void present();

        void clear_color_buffer(Rgbaf color = {});
        void clear_depth_buffer(f32 depth = 0.0f);

    } // namespace render

} // namespace geo

#endif // RENDER_RENDER_H_INCLUDED
