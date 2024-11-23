/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <errno.h>
#include <stdio.h>

#include <limits>

#include <zip.h>

#include <core/finally.h>
#include <system/debug.h>

#include "zip.h"

using namespace geo;

//==================================================================================================
// libzip_error_category
//==================================================================================================

namespace {

    class LibzipErrorCategory : public std::error_category {
        std::string message(int value) const override
        {
            zip_error_t zerror;
            std::string msg;

            zip_error_init_with_code(&zerror, value);
            msg = zip_error_strerror(&zerror);
            zip_error_fini(&zerror);
            return msg;
        }

        const char* name() const noexcept override { return "libzip"; }
    } constinit const libzip_error_category_instance;

    Error make_libzip_error(const char* description, int zerr, int cerr)
    {
        Error error{.description = description};

        if (zerr) {
            error.code = {zerr, libzip_error_category};
            if (cerr)
                error.cause.reset(new Error{.code = {cerr, std::generic_category()}});
        } else if (cerr) {
            error.code = {cerr, std::generic_category()};
        }

        return error;
    }

    Error make_libzip_error(const char* description, const zip_error_t* zerror)
    {
        return make_libzip_error(description, zip_error_code_zip(zerror), zip_error_code_system(zerror));
    }

} // namespace

constinit const std::error_category& geo::libzip_error_category = libzip_error_category_instance;

//==================================================================================================
// ZipArchive
//==================================================================================================

ZipArchive::ZipArchive(ZipArchive&& other)
    : zip_{other.zip_}
{
    other.zip_ = nullptr;
}

ZipArchive::ZipArchive(const oschar_t* path, Error& out_error)
{
    open(path, out_error);
}

ZipArchive::~ZipArchive()
{
    close();
}

ZipArchive& ZipArchive::operator=(ZipArchive&& other)
{
    if (&other != this) {
        close();
        zip_ = other.zip_;
        other.zip_ = nullptr;
    }
    return *this;
}

void ZipArchive::close()
{
    Error error;

    close(error);

    if (error)
        LOG_ERROR("Failed to close zip archive: {}", error);
}

void ZipArchive::close(Error& out_error)
{
    if (!zip_)
        return;

    if (zip_close(zip_))
        out_error = make_libzip_error("zip_close failed", zip_get_error(zip_));

    zip_ = nullptr;
}

bool ZipArchive::open(const oschar_t* path, Error& out_error)
{
    close();

    // Open the underlying stream for reading.
    std::unique_ptr<FILE, decltype(&fclose)> fp{nullptr, &fclose};

#ifdef _WIN32
    fp.reset(_wfopen(path, L"rb"));
#else
    fp.reset(fopen(path, "rb"));
#endif

    if (!fp) {
        out_error = {.description = "fopen failed", .code = {errno, std::generic_category()}};
        return false;
    }

    // Capture libzip errors.
    zip_error_t zerror;
    zip_error_init(&zerror);
    Finally free_zerror = [&] { zip_error_fini(&zerror); };

    // Create a zip source from the underlying stream.
    std::unique_ptr<zip_source_t, decltype(&zip_source_free)> source{nullptr, &zip_source_free};
    source.reset(zip_source_filep_create(fp.get(), 0, ZIP_LENGTH_TO_END, &zerror));

    if (!source) {
        out_error = make_libzip_error("zip_source_filep_create failed", &zerror);
        return false;
    }

    fp.release(); // fp is now owned by source

    // Open the zip archive from the source.
    zip_ = zip_open_from_source(source.get(), ZIP_RDONLY, &zerror);

    if (!zip_) {
        out_error = make_libzip_error("zip_open_from_source failed", &zerror);
        return false;
    }

    source.release(); // source is now owned by zip_
    return true;
}

std::unique_ptr<Stream> ZipArchive::open_stream(const char* name, Error& out_error)
{
    ZipStream stream{*this, name, out_error};

    if (stream.is_open())
        return std::make_unique<ZipStream>(std::move(stream));
    else
        return {};
}

//==================================================================================================
// ZipStream
//==================================================================================================

ZipStream::ZipStream(ZipStream&& other)
    : zfp_{other.zfp_}
    , size_{other.size_}
{
    other.zfp_ = nullptr;
    other.size_ = -1;
}

ZipStream::ZipStream(ZipArchive& archive, const char* name, Error& out_error)
{
    open(archive, name, out_error);
}

ZipStream::~ZipStream()
{
    close();
}

ZipStream& ZipStream::operator=(ZipStream&& other)
{
    if (&other != this) {
        close();
        std::swap(zfp_, other.zfp_);
        std::swap(size_, other.size_);
    }
    return *this;
}

bool ZipStream::open(ZipArchive& archive, const char* name, Error& out_error)
{
    close();

    if (!archive.is_open()) {
        out_error = {.code = IoErrorCode::archive_closed};
        return false;
    }

    // Get the entry index.
    i64 index = zip_name_locate(archive.zip_, name, 0);

    if (index < 0) {
        out_error = {.code = IoErrorCode::not_found};
        return false;
    }

    // Open the underlying stream.
    zfp_ = zip_fopen_index(archive.zip_, u64(index), 0);

    if (!zfp_) {
        out_error = make_libzip_error("zip_fopen_index failed", zip_get_error(archive.zip_));
        return false;
    }

    // Try to get the stream size.
    zip_stat_t stat;

    if (zip_stat_index(archive.zip_, u64(index), ZIP_STAT_SIZE, &stat) == 0
        && stat.size <= u64(std::numeric_limits<i64>::max()))
    {
        size_ = i64(stat.size);
    }

    return true;
}

void ZipStream::close(Error& out_error)
{
    int zerr;

    if (!zfp_)
        return;

    errno = 0;
    zerr = zip_fclose(zfp_);

    zfp_ = nullptr;
    size_ = -1;

    if (zerr)
        out_error = make_libzip_error("zip_fclose failed", zerr, errno);
}

i64 ZipStream::get_size(Error& out_error) const
{
    if (size_ >= 0) {
        return size_;
    } else {
        out_error = {.code = IoErrorCode::stream_size_undefined};
        return -1;
    }
}

size_t ZipStream::read_partial(void* dst, size_t size, Error& out_error)
{
    if (!zfp_) {
        out_error = {.code = IoErrorCode::stream_closed};
        return 0;
    }

    i64 result = zip_fread(zfp_, dst, size);

    if (result < 0) {
        out_error = make_libzip_error("zip_fread failed", zip_file_get_error(zfp_));
        return 0;
    }

    return size_t(result);
}
