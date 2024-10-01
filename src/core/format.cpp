/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <geo/core/format.h>

using namespace geo;

//==================================================================================================
// Basic format tests

// Verbatim text tests
static_assert(format::static_string<"">() == "");
static_assert(format::static_string<"abc">() == "abc");

// Escape sequence test
static_assert(format::static_string<"}}{{}}{{">() == "}{}{");

// Value index test
static_assert(format::static_string<"{}{2}{:}{1:}{}{0}", 'a', 'b', 'c'>() == "acbbca");

// Boolean test
static_assert(format::static_string<"{}|{}", true, false>() == "true|false");

//==================================================================================================
// Integer format tests

static_assert(format::static_string<"{}", 0>() == "0");
static_assert(format::static_string<"{}", 123>() == "123");

// Sign mode tests
static_assert(format::static_string<"{:+}", 123>() == "+123");
static_assert(format::static_string<"{:-}", 123>() == "123");
static_assert(format::static_string<"{: }", 123>() == " 123");
static_assert(format::static_string<"{:+}", -123>() == "-123");
static_assert(format::static_string<"{:-}", -123>() == "-123");
static_assert(format::static_string<"{: }", -123>() == "-123");

// Fill char test
static_assert(format::static_string<"{:_>4}", 123>() == "_123");

// Alignment tests
static_assert(format::static_string<"{:<5}", 123>() == "123  ");
static_assert(format::static_string<"{:^5}", 123>() == " 123 ");
static_assert(format::static_string<"{:>5}", 123>() == "  123");
static_assert(format::static_string<"{:<5}", -123>() == "-123 ");
static_assert(format::static_string<"{:^5}", -123>() == "-123 ");
static_assert(format::static_string<"{:>5}", -123>() == " -123");
static_assert(format::static_string<"{:< 5}", 123>() == " 123 ");
static_assert(format::static_string<"{:^ 5}", 123>() == " 123 ");
static_assert(format::static_string<"{:^6}", 123>() == "  123 ");
static_assert(format::static_string<"{:^6}", -123>() == " -123 ");
static_assert(format::static_string<"{:^ 6}", 123>() == "  123 ");

// Radix tests
static_assert(format::static_string<"{:b}", 0b10101>() == "10101");
static_assert(format::static_string<"{:o}", 01337>() == "1337");
static_assert(format::static_string<"{:x}", 0x1BADD00Dul>() == "1badd00d");
static_assert(format::static_string<"{:X}", 0x1BADD00Dul>() == "1BADD00D");

// Digit separator tests
static_assert(format::static_string<"{:H}", 123>() == "123");
static_assert(format::static_string<"{:H}", 1234>() == "1'234");
static_assert(format::static_string<"{:H:_}", 1234>() == "1_234");
static_assert(format::static_string<"{:Hb}", 0b10101>() == "1'0101");
static_assert(format::static_string<"{:Ho}", 0113377>() == "11'3377");
static_assert(format::static_string<"{:HX}", 0x1BADD00Dul>() == "1BAD'D00D");

// Prefix tests
static_assert(format::static_string<"{:#}", 123>() == "123");
static_assert(format::static_string<"{:#b}", 0b10101>() == "0b10101");
static_assert(format::static_string<"{:#B}", 0b10101>() == "0b10101");
static_assert(format::static_string<"{:#o}", 0>() == "0");
static_assert(format::static_string<"{:#o}", 01337>() == "01337");
static_assert(format::static_string<"{:#x}", 0x1BADD00Dul>() == "0x1badd00d");
static_assert(format::static_string<"{:#X}", 0x1BADD00Dul>() == "0x1BADD00D");
static_assert(format::static_string<"{:#HX}", 0x1BADD00Dul>() == "0x1BAD'D00D");

// Zero-fill tests
static_assert(format::static_string<"{:04}", 123>() == "0123");
static_assert(format::static_string<"{:04H}", 123>() == " 123");
static_assert(format::static_string<"{:05H}", 123>() == "0'123");
static_assert(format::static_string<"{:#05Ho}", 01337>() == "0'1337");
static_assert(format::static_string<"{:#06Ho}", 01337>() == "0'1337");
static_assert(format::static_string<"{:#012HX}", 0x1BADD00Dul>() == " 0x1BAD'D00D");
static_assert(format::static_string<"{:#013HX}", 0x1BADD00Dul>() == "0x0'1BAD'D00D");

// Everything test
static_assert(format::static_string<"{:!^+#013H:?X}", 0x1BADD00Dul>() == "+0x1BAD?D00D!");
