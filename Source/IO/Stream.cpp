/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <System/Debug.h>

#include "Stream.h"

using namespace Geo;

Stream::~Stream()
{
}

void Stream::Close()
{
    Error error;

    Close(error);

    if (error)
        LOG_ERROR("Failed to close stream: {}", error);
}

bool Stream::Flush(Error&)
{
    return true;
}

i64 Stream::GetPosition(Error& outError) const
{
    outError = {.code = IoErrorCode::NotSeekable};
    return -1;
}

i64 Stream::GetSize(Error& outError) const
{
    outError = {.code = IoErrorCode::SizeUndefined};
    return -1;
}

size_t Stream::Read(void* dst, size_t size, Error& outError)
{
    Error localError;
    size_t total = 0;
    size_t result;

    while (size) {
        result = ReadPartial(dst, size, localError);
        ASSERT(result <= size);
        total += result;

        if (localError) {
            outError = std::move(localError);
            break;
        } else if (!result) {
            break;
        }

        dst = reinterpret_cast<char*>(dst) + result;
        size -= result;
    }

    return total;
}

size_t Stream::ReadExact(void* dst, size_t size, Error& outError)
{
    Error localError;
    size_t result;

    result = Read(dst, size, localError);

    if (localError)
        outError = std::move(localError);
    else if (result < size)
        outError = {.code = IoErrorCode::EndOfStream};

    return result;
}

size_t Stream::ReadPartial(void*, size_t, Error& outError)
{
    outError = {.code = IoErrorCode::NotReadable};
    return 0;
}

bool Stream::Seek(i64, SeekOrigin, Error& outError)
{
    outError = {.code = IoErrorCode::NotSeekable};
    return false;
}

size_t Stream::Write(const void* src, size_t size, Error& outError)
{
    Error localError;
    size_t total = 0;
    size_t result;

    while (size) {
        result = WritePartial(src, size, outError);
        ASSERT(result <= size);
        total += result;

        if (localError) {
            outError = std::move(localError);
            break;
        } else if (!result) {
            outError = {.code = IoErrorCode::EndOfStream};
            break;
        }

        src = reinterpret_cast<const char*>(src) + result;
        size -= result;
    }

    return total;
}

size_t Stream::WritePartial(const void*, size_t, Error& outError)
{
    outError = {.code = IoErrorCode::NotWritable};
    return 0;
}
