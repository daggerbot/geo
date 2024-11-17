/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <math/math.h>
#include <system/debug.h>

#include "stream.h"

using namespace geo;

//==================================================================================================
// Stream
//==================================================================================================

Stream::~Stream()
{
}

void Stream::close()
{
    Error error;

    close(error);

    if (error)
        LOG_ERROR("Failed to close stream: {}", error);
}

bool Stream::flush(Error&)
{
    return true;
}

i64 Stream::get_position(Error& out_error) const
{
    out_error = {.code = IoErrorCode::not_seekable};
    return -1;
}

i64 Stream::get_size(Error& out_error) const
{
    out_error = {.code = IoErrorCode::stream_size_undefined};
    return -1;
}

size_t Stream::read(void* dst, size_t size, Error& out_error)
{
    Error local_error;
    size_t pass_result;
    size_t total_result = 0;

    while (size) {
        pass_result = read_partial(dst, size, local_error);
        total_result += pass_result;
        ASSERT(pass_result <= size);

        if (local_error) {
            out_error = std::move(local_error);
            break;
        } else if (!pass_result) {
            break;
        }

        dst = reinterpret_cast<char*>(dst) + pass_result;
        size -= pass_result;
    }

    return total_result;
}

size_t Stream::read_exact(void* dst, size_t size, Error& out_error)
{
    Error local_error;
    size_t result;

    result = read(dst, size, local_error);

    if (local_error)
        out_error = std::move(local_error);
    else if (result < size)
        out_error = {.code = IoErrorCode::end_of_stream};

    return result;
}

size_t Stream::read_partial(void*, size_t, Error& out_error)
{
    out_error = {.code = IoErrorCode::not_readable};
    return 0;
}

size_t Stream::write(const void* src, size_t size, Error& out_error)
{
    Error local_error;
    size_t pass_result;
    size_t total_result = 0;

    while (size) {
        pass_result = write_partial(src, size, local_error);
        total_result += pass_result;
        ASSERT(pass_result <= size);

        if (local_error) {
            out_error = std::move(local_error);
            break;
        } else if (!pass_result) {
            out_error = {.code = IoErrorCode::end_of_stream};
            break;
        }

        src = reinterpret_cast<const char*>(src) + pass_result;
        size -= pass_result;
    }

    return total_result;
}

size_t Stream::write_partial(const void*, size_t, Error& out_error)
{
    out_error = {.code = IoErrorCode::not_writable};
    return 0;
}

//==================================================================================================
// StreamProvider
//==================================================================================================

StreamProvider::~StreamProvider()
{
}

std::vector<u8> StreamProvider::read_stream_bytes(const char* name, size_t max_size, Error& out_error)
{
    Error local_error;

    // Open the stream for reading.
    auto stream = open_stream(name, local_error);

    if (local_error) {
        out_error = std::move(local_error);
        return {};
    } else if (!stream || !stream->is_open()) {
        out_error = {.code = IoErrorCode::not_found};
        return {};
    }

    // Allocate the output buffer.
    std::vector<u8> data;
    i64 stream_size = stream->get_size(local_error);

    local_error.clear();

    if (stream_size > 0) {
        if (u64(stream_size) > max_size) {
            out_error = {.code = IoErrorCode::stream_too_long};
            return {};
        }
        data.reserve(math::min(size_t(stream_size), max_size));
    }

    // Read the data from the stream.
    char buf[4096];
    size_t pass_size;
    size_t pass_result;

    while (data.size() < max_size) {
        pass_size = math::min(max_size - data.size(), sizeof(buf));
        pass_result = stream->read_partial(buf, pass_size, local_error);
        ASSERT(pass_result <= pass_size);

        if (pass_result)
            data.insert(data.end(), buf, buf + pass_result);

        if (local_error) {
            out_error = std::move(local_error);
            return data;
        } else if (!pass_result) {
            return data;
        }
    }

    // Make sure the stream does not continue beyond max_size.
    u8 excess_byte;

    if (data.size() == max_size && stream->read_partial(&excess_byte, 1, local_error))
        out_error = {.code = IoErrorCode::stream_too_long};

    return data;
}
