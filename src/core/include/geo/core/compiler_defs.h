/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef GEO_CORE_COMPILER_DEFS_H_INCLUDED
#define GEO_CORE_COMPILER_DEFS_H_INCLUDED

/// @addtogroup geo_core
/// @{

/// @def FORCE_INLINE
/// Forces the compiler to inline a function.
#ifdef __GNUC__
# define FORCE_INLINE __attribute__((__always_inline__))
#elif defined(_MSC_VER)
# define FORCE_INLINE __forceinline
#else
# define FORCE_INLINE
#endif

/// @}

#endif // GEO_CORE_COMPILER_DEFS_H_INCLUDED
