/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef CLIENT_PLAYGROUND_H_INCLUDED
#define CLIENT_PLAYGROUND_H_INCLUDED

#include <memory>

#include <render/geometry.h>

#include "main.h"

namespace geo {

    /// Temporary testing state.
    class Playground : public ClientState {
    public:
        void begin_state() override;
        void render(u32 delta_ms) override;

    private:
        std::shared_ptr<render::Geometry> geometry_;
    };

} // namespace geo

#endif // CLIENT_PLAYGROUND_H_INCLUDED
