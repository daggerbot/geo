/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef CLIENT_DISPLAY_H_INCLUDED
#define CLIENT_DISPLAY_H_INCLUDED

#include <math/vector.h>

namespace geo {

    /// Functions for managing the game window.
    namespace display {

        void init();
        void shut_down();
        Vec2i get_window_size();
        void swap_buffers();

        void* gl_get_proc_address(const char* name);

    } // namespace display

} // namespace geo

#endif // CLIENT_DISPLAY_H_INCLUDED
