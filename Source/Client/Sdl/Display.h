/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef GEO_CLIENT_SDL_DISPLAY_H_INCLUDED
#define GEO_CLIENT_SDL_DISPLAY_H_INCLUDED

#include <Client/Display.h>

using SDL_GLContext = void*;
struct SDL_Window;

namespace Geo {

    /// @addtogroup Client
    /// @{

    /// SDL window wrapper.
    class SdlDisplay : public Display {
    public:
        void Initialize();
        void ShutDown();

        Vec2i GetSize() const override;
        void SwapBuffers() override;

    private:
        SDL_Window* m_window = nullptr;
        SDL_GLContext m_glContext = nullptr;
    };

    /// @}

} // namespace Geo

#endif // GEO_CLIENT_SDL_DISPLAY_H_INCLUDED
