/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "Parse.h"

using namespace Geo;

//==================================================================================================
// Parse::Int() tests
//==================================================================================================

// Basic tests
static_assert(Parse::Int<int>("") == ParseResult<int>{ParseStatus::Empty, {}, 0, false});
static_assert(Parse::Int<int>("0") == ParseResult<int>{ParseStatus::Ok, 0, 1, false});
static_assert(Parse::Int<int>("123") == ParseResult<int>{ParseStatus::Ok, 123, 3, false});

// Invalid/trailing input tests
static_assert(Parse::Int<int>(" 123") == ParseResult<int>{ParseStatus::InvalidInput, {}, 0, false});
static_assert(Parse::Int<int>("a123") == ParseResult<int>{ParseStatus::InvalidInput, {}, 0, false});
static_assert(Parse::Int<int>("123 ") == ParseResult<int>{ParseStatus::Ok, 123, 3, true});
static_assert(Parse::Int<int>("123a") == ParseResult<int>{ParseStatus::Ok, 123, 3, true});

// Sign tests
static_assert(Parse::Int<int>("+123") == ParseResult<int>{ParseStatus::Ok, 123, 4, false});
static_assert(Parse::Int<int>("-123") == ParseResult<int>{ParseStatus::Ok, -123, 4, false});
static_assert(Parse::Int<int>("+123", ParseIntOptions{.allowSign = false})
              == ParseResult<int>{ParseStatus::InvalidInput, {}, 0, false});
static_assert(Parse::Int<int>("-123", ParseIntOptions{.allowSign = false})
              == ParseResult<int>{ParseStatus::InvalidInput, {}, 0, false});
static_assert(Parse::Int<int>("++123") == ParseResult<int>{ParseStatus::InvalidInput, {}, 0, false});

// Radix tests
static_assert(Parse::Int<int>("0123") == ParseResult<int>{ParseStatus::Ok, 123, 4, false});
static_assert(Parse::Int<int>("0b10101", ParseIntOptions{.radix = 0})
              == ParseResult<int>{ParseStatus::Ok, 0b10101, 7, false});
static_assert(Parse::Int<int>("01337", ParseIntOptions{.radix = 0})
              == ParseResult<int>{ParseStatus::Ok, 01337, 5, false});
static_assert(Parse::Int<uint32_t>("0x1BADD00D", ParseIntOptions{.radix = 0})
              == ParseResult<uint32_t>{ParseStatus::Ok, 0x1BADD00Dul, 10, false});
static_assert(Parse::Int<int>("Z0", ParseIntOptions{.radix = 36})
              == ParseResult<int>{ParseStatus::Ok, 1260, 2, false});
static_assert(Parse::Int<int>("00000", ParseIntOptions{.radix = 1})
              == ParseResult<int>{ParseStatus::InvalidOptions, {}, 0, false});
static_assert(Parse::Int<int>("Z0", ParseIntOptions{.radix = 37})
              == ParseResult<int>{ParseStatus::InvalidOptions, {}, 0, false});

// Digit separator tests
static_assert(Parse::Int<int>("1,23") == ParseResult<int>{ParseStatus::Ok, 1, 1, true});
static_assert(Parse::Int<int>("1,23", ParseIntOptions{.digitSeparator = ','})
              == ParseResult<int>{ParseStatus::Ok, 123, 4, false});
static_assert(Parse::Int<int>("123,", ParseIntOptions{.digitSeparator = ','})
              == ParseResult<int>{ParseStatus::Ok, 123, 3, true});
static_assert(Parse::Int<int>("1,,23", ParseIntOptions{.digitSeparator = ','})
              == ParseResult<int>{ParseStatus::Ok, 1, 1, true});
static_assert(Parse::Int<int>("0x,10", ParseIntOptions{.digitSeparator = ',', .radix = 0})
              == ParseResult<int>{ParseStatus::InvalidInput, {}, 0, false});

// Range tests
static_assert(Parse::Int<int8_t>("127") == ParseResult<int8_t>{ParseStatus::Ok, 127, 3, false});
static_assert(Parse::Int<int8_t>("128") == ParseResult<int8_t>{ParseStatus::Overflow, {}, 3, false});
static_assert(Parse::Int<int8_t>("200") == ParseResult<int8_t>{ParseStatus::Overflow, {}, 3, false});
static_assert(Parse::Int<int8_t>("-128") == ParseResult<int8_t>{ParseStatus::Ok, -128, 4, false});
static_assert(Parse::Int<int8_t>("-129") == ParseResult<int8_t>{ParseStatus::Underflow, {}, 4, false});
static_assert(Parse::Int<int8_t>("-200") == ParseResult<int8_t>{ParseStatus::Underflow, {}, 4, false});
