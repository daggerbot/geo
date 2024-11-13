/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef GEO_CLIENT_PLAYGROUND_H_INCLUDED
#define GEO_CLIENT_PLAYGROUND_H_INCLUDED

#include <memory>

#include "Main.h"

namespace Geo {

    class RenderBuffer;
    class RenderGeometry;

    class Playground : public ClientState {
    public:
        void BeginState(Client& client) override;
        void Render(Client& client, RenderSystem& render, u32 deltaMs) override;

    private:
        std::shared_ptr<RenderGeometry> m_geometry;
    };

} // namespace Geo

#endif // GEO_CLIENT_PLAYGROUND_H_INCLUDED
