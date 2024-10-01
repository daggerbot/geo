/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <atomic>

#include <geo/system/system.h>

using namespace geo;

namespace {

    std::atomic<ErrorDialogHandler> s_errorDialogHandler = nullptr;

} // namespace

void system::set_error_dialog_handler(ErrorDialogHandler handler)
{
    s_errorDialogHandler.store(handler, std::memory_order::relaxed);
}

void system::show_error_dialog(const oschar_t* message)
{
    auto handler = s_errorDialogHandler.load(std::memory_order::relaxed);

    if (handler)
        handler(message);
}
