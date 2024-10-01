/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <limits>
#include <type_traits>

#include <geo/core/types.h>

using namespace geo;

static_assert(static_cast<unsigned char>(-1) == 255);
static_assert(std::numeric_limits<unsigned char>::digits == 8);

static_assert(sizeof(char) == 1);
static_assert(sizeof(char8_t) == 1);
static_assert(sizeof(char16_t) == 2);
static_assert(sizeof(char32_t) == 4);

static_assert(sizeof(short) == 2);
static_assert(sizeof(int) == 4);
static_assert(sizeof(long) == 4 || sizeof(long) == 8);
static_assert(sizeof(long long) == 8);

static_assert(sizeof(float) == 4);
static_assert(sizeof(double) == 8);
static_assert(sizeof(long double) >= 8);

static_assert(std::numeric_limits<float>::is_iec559);
static_assert(std::numeric_limits<double>::is_iec559);

static_assert(sizeof(void(*)()) == sizeof(void*));

static_assert(sizeof(max_align_t) >= sizeof(void*));
static_assert(sizeof(max_align_t) >= sizeof(std::intmax_t));
static_assert(std::is_same_v<nullptr_t, decltype(nullptr)>);
static_assert(std::is_same_v<ptrdiff_t, decltype(static_cast<int*>(0) - static_cast<int*>(0))>);
static_assert(sizeof(size_t) >= sizeof(int));
static_assert(std::is_same_v<size_t, decltype(sizeof(int))>);

static_assert(std::is_same_v<i8, signed char>);
static_assert(std::is_same_v<u8, unsigned char>);

static_assert(sizeof(i8) == 1);
static_assert(sizeof(i16) == 2);
static_assert(sizeof(i32) == 4);
static_assert(sizeof(i64) == 8);
static_assert(sizeof(iptr) == sizeof(void*));

static_assert(sizeof(u8) == 1);
static_assert(sizeof(u16) == 2);
static_assert(sizeof(u32) == 4);
static_assert(sizeof(u64) == 8);
static_assert(sizeof(uptr) == sizeof(void*));

static_assert(std::numeric_limits<i8>::min() == -0x80);
static_assert(std::numeric_limits<i8>::max() == 0x7F);
static_assert(std::numeric_limits<i16>::min() == -0x7FFF-1);
static_assert(std::numeric_limits<i16>::max() == 0x7FFF);
static_assert(std::numeric_limits<i32>::min() == -0x7FFF'FFFFl-1);
static_assert(std::numeric_limits<i32>::max() == 0x7FFF'FFFF);
static_assert(std::numeric_limits<i64>::min() == -0x7FFF'FFFF'FFFF'FFFFll-1);
static_assert(std::numeric_limits<i64>::max() == 0x7FFF'FFFF'FFFF'FFFFll);

static_assert(std::numeric_limits<u8>::min() == 0);
static_assert(std::numeric_limits<u8>::max() == 0xFF);
static_assert(std::numeric_limits<u16>::min() == 0);
static_assert(std::numeric_limits<u16>::max() == 0xFFFFu);
static_assert(std::numeric_limits<u32>::min() == 0);
static_assert(std::numeric_limits<u32>::max() == 0xFFFF'FFFFul);
static_assert(std::numeric_limits<u64>::min() == 0);
static_assert(std::numeric_limits<u64>::max() == 0xFFFF'FFFF'FFFF'FFFFull);
