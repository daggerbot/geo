/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <geo/core/parse.h>

using namespace geo;

//==================================================================================================
// parse::integer() tests

// Basic tests
static_assert(parse::integer<int>("") == ParseResult<int>{ParseStatus::empty, {}, 0, false});
static_assert(parse::integer<int>("0") == ParseResult<int>{ParseStatus::ok, 0, 1, false});
static_assert(parse::integer<int>("123") == ParseResult<int>{ParseStatus::ok, 123, 3, false});

// Invalid/trailing input tests
static_assert(parse::integer<int>(" 123") == ParseResult<int>{ParseStatus::invalid_input, {}, 0, false});
static_assert(parse::integer<int>("a123") == ParseResult<int>{ParseStatus::invalid_input, {}, 0, false});
static_assert(parse::integer<int>("123 ") == ParseResult<int>{ParseStatus::ok, 123, 3, true});
static_assert(parse::integer<int>("123a") == ParseResult<int>{ParseStatus::ok, 123, 3, true});

// Sign tests
static_assert(parse::integer<int>("+123") == ParseResult<int>{ParseStatus::ok, 123, 4, false});
static_assert(parse::integer<int>("-123") == ParseResult<int>{ParseStatus::ok, -123, 4, false});
static_assert(parse::integer<int>("+123", ParseIntOptions{.allow_sign = false})
              == ParseResult<int>{ParseStatus::invalid_input, {}, 0, false});
static_assert(parse::integer<int>("-123", ParseIntOptions{.allow_sign = false})
              == ParseResult<int>{ParseStatus::invalid_input, {}, 0, false});
static_assert(parse::integer<int>("++123") == ParseResult<int>{ParseStatus::invalid_input, {}, 0, false});

// Radix tests
static_assert(parse::integer<int>("0123") == ParseResult<int>{ParseStatus::ok, 123, 4, false});
static_assert(parse::integer<int>("0b10101", ParseIntOptions{.radix = 0})
              == ParseResult<int>{ParseStatus::ok, 0b10101, 7, false});
static_assert(parse::integer<int>("01337", ParseIntOptions{.radix = 0})
              == ParseResult<int>{ParseStatus::ok, 01337, 5, false});
static_assert(parse::integer<uint32_t>("0x1BADD00D", ParseIntOptions{.radix = 0})
              == ParseResult<uint32_t>{ParseStatus::ok, 0x1BADD00Dul, 10, false});
static_assert(parse::integer<int>("Z0", ParseIntOptions{.radix = 36})
              == ParseResult<int>{ParseStatus::ok, 1260, 2, false});
static_assert(parse::integer<int>("00000", ParseIntOptions{.radix = 1})
              == ParseResult<int>{ParseStatus::invalid_options, {}, 0, false});
static_assert(parse::integer<int>("Z0", ParseIntOptions{.radix = 37})
              == ParseResult<int>{ParseStatus::invalid_options, {}, 0, false});

// Digit separator tests
static_assert(parse::integer<int>("1,23") == ParseResult<int>{ParseStatus::ok, 1, 1, true});
static_assert(parse::integer<int>("1,23", ParseIntOptions{.separator = ','})
              == ParseResult<int>{ParseStatus::ok, 123, 4, false});
static_assert(parse::integer<int>("123,", ParseIntOptions{.separator = ','})
              == ParseResult<int>{ParseStatus::ok, 123, 3, true});
static_assert(parse::integer<int>("1,,23", ParseIntOptions{.separator = ','})
              == ParseResult<int>{ParseStatus::ok, 1, 1, true});
static_assert(parse::integer<int>("0x,10", ParseIntOptions{.radix = 0, .separator = ','})
              == ParseResult<int>{ParseStatus::invalid_input, {}, 0, false});

// Range tests
static_assert(parse::integer<int8_t>("127") == ParseResult<int8_t>{ParseStatus::ok, 127, 3, false});
static_assert(parse::integer<int8_t>("128") == ParseResult<int8_t>{ParseStatus::overflow, {}, 3, false});
static_assert(parse::integer<int8_t>("200") == ParseResult<int8_t>{ParseStatus::overflow, {}, 3, false});
static_assert(parse::integer<int8_t>("-128") == ParseResult<int8_t>{ParseStatus::ok, -128, 4, false});
static_assert(parse::integer<int8_t>("-129") == ParseResult<int8_t>{ParseStatus::underflow, {}, 4, false});
static_assert(parse::integer<int8_t>("-200") == ParseResult<int8_t>{ParseStatus::underflow, {}, 4, false});
