/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef GEO_SYSTEM_DEBUG_H_INCLUDED
#define GEO_SYSTEM_DEBUG_H_INCLUDED

#include <mutex>

#include <geo/core/format.h>
#include <geo/core/string_types.h>

/// @addtogroup geo_system
/// @{

/// @def FATAL
/// Shows a fatal error message and terminates the process with an error code.
#ifdef LOG_NO_LOCATION
# define FATAL(FMT, ...) ::geo::debug::internal::fatal<OSSTR FMT>(nullptr, 0, __VA_ARGS__)
#else
# define FATAL(FMT, ...) ::geo::debug::internal::fatal<OSSTR FMT>(::geo::debug::internal::get_short_name<OSSTR __FILE__>().data, __LINE__ __VA_OPT__(,) __VA_ARGS__)
#endif

/// @def LOG_ERROR
/// Logs an error that may be of consequence to the user.
#ifdef LOG_NO_LOCATION
# define LOG_ERROR(FMT, ...) ::geo::debug::internal::log<OSSTR FMT>(::geo::LogLevel::error __VA_OPT__(,) __VA_ARGS__)
#else
# define LOG_ERROR(FMT, ...) ::geo::debug::internal::log<OSSTR FMT>(::geo::debug::internal::get_short_name<OSSTR __FILE__>().data, __LINE__, ::geo::LogLevel::error __VA_OPT__(,) __VA_ARGS__)
#endif

/// @def LOG_WARNING
/// Logs a potential issue or unexpected behavior that warrants attention.
#ifdef LOG_NO_LOCATION
# define LOG_WARNING(FMT, ...) ::geo::debug::internal::log<OSSTR FMT>(::geo::LogLevel::warning __VA_OPT__(,) __VA_ARGS__)
#else
# define LOG_WARNING(FMT, ...) ::geo::debug::internal::log<OSSTR FMT>(::geo::debug::internal::get_short_name<OSSTR __FILE__>().data, __LINE__, ::geo::LogLevel::warning __VA_OPT__(,) __VA_ARGS__)
#endif

/// @def LOG_NOTICE
/// Logs an important status, typically non-critical but noteworthy.
#ifdef LOG_NO_LOCATION
# define LOG_NOTICE(FMT, ...) ::geo::debug::internal::log<OSSTR FMT>(::geo::LogLevel::notice __VA_OPT__(,) __VA_ARGS__)
#else
# define LOG_NOTICE(FMT, ...) ::geo::debug::internal::log<OSSTR FMT>(::geo::debug::internal::get_short_name<OSSTR __FILE__>().data, __LINE__, ::geo::LogLevel::notice __VA_OPT__(,) __VA_ARGS__)
#endif

/// @def LOG_INFO
/// Logs a general informational message about application state or progress.
#ifdef LOG_NO_LOCATION
# define LOG_INFO(FMT, ...) ::geo::debug::internal::log<OSSTR FMT>(::geo::LogLevel::info __VA_OPT__(,) __VA_ARGS__)
#else
# define LOG_INFO(FMT, ...) ::geo::debug::internal::log<OSSTR FMT>(::geo::debug::internal::get_short_name<OSSTR __FILE__>().data, __LINE__, ::geo::LogLevel::info __VA_OPT__(,) __VA_ARGS__)
#endif

/// @def LOG_DEBUG
/// Logs a message primarily for debugging purposes, only visible on debug builds.
#ifdef NDEBUG
# define LOG_DEBUG(FMT, ...) ::geo::debug::internal::discard<OSSTR FMT>(__VA_ARGS__)
#elif defined(LOG_NO_LOCATION)
# define LOG_DEBUG(FMT, ...) ::geo::debug::internal::log<OSSTR FMT>(::geo::LogLevel::debug __VA_OPT__(,) __VA_ARGS__)
#else
# define LOG_DEBUG(FMT, ...) ::geo::debug::internal::log<OSSTR FMT>(::geo::debug::internal::get_short_name<OSSTR __FILE__>().data, __LINE__, ::geo::LogLevel::debug __VA_OPT__(,) __VA_ARGS__)
#endif

/// @def LOG_TRACE
/// Logs highly detailed tracing information, usually enabled at runtime via a command line option,
/// and visible only in debug builds.
#ifdef NDEBUG
# define LOG_TRACE(FMT, ...) ::geo::debug::internal::discard<OSSTR FMT>(__VA_ARGS__)
#elif defined(LOG_NO_LOCATION)
# define LOG_TRACE(FMT, ...) ::geo::debug::internal::log<OSSTR FMT>(::geo::LogLevel::trace __VA_OPT__(,) __VA_ARGS__)
#else
# define LOG_TRACE(FMT, ...) ::geo::debug::internal::log<OSSTR FMT>(::geo::debug::internal::get_short_name<OSSTR __FILE__>().data, __LINE__, ::geo::LogLevel::trace __VA_OPT__(,) __VA_ARGS__)
#endif

/// @def DASSERT
/// Shows a fatal error message and terminates the process with an error code if a condition is not
/// met. No-op on release builds.
#ifdef NDEBUG
# define DASSERT(x) do { (void)(x); } while (0)
#elif defined(LOG_NO_LOCATION)
# define DASSERT(x) do { if (!(x)) ::geo::debug::internal::fatal<OSSTR "Assertion failed: {}">(nullptr, 0, OSSTR #x); } while (0)
#else
# define DASSERT(x) do { if (!(x)) ::geo::debug::internal::fatal<OSSTR "Assertion failed: {}">(::geo::debug::internal::get_short_name<OSSTR __FILE__>().data, __LINE__, OSSTR #x); } while (0)
#endif

/// @}

namespace geo {

    /// @addtogroup geo_system
    /// @{

    /// Represents the severity of a log message.
    enum class LogLevel : unsigned {
        /// Indicates an error that may be of consequence to the user.
        error,

        /// Indicates a potential issue or unexpected behavior that warrants attention.
        warning,

        /// Indicates an important status, typically non-critical but noteworthy.
        notice,

        /// Indicates a general informational message about application state or progress.
        info,

        /// Indicates a message primarily for debugging purposes, only visible in debug builds.
        debug,

        /// Indicates highly detailed tracing information, usually enabled at runtime via a
        /// command line option, and visible only in debug builds.
        trace,
    };

    /// Debugging facilities.
    namespace debug {

        /// Enables the most verbose log messages available on the current build. If used, this
        /// should be called early in the program, particularly before any worker threads are
        /// spawned.
        void enable_verbose();

        namespace internal {

            struct LoggerState;

            // Gets the index of the start of the shortened source file path.
            constexpr size_t get_short_name_index(OsStringView name)
            {
#ifdef LOG_PATH_PREFIX
                constexpr oschar_t prefix[] = OSSTR LOG_PATH_PREFIX;

                size_t i = 0;

                // Skip LOG_PATH_PREFIX
                for (;;) {
                    if (!prefix[i])
                        break;
                    else if (name[i] != prefix[i])
                        return 0;
                    else
                        ++i;
                }

                // Skip leading separator
                if (name[i] == '/')
                    ++i;
# ifdef _WIN32
                else if (name[i] == '\\')
                    ++i;
# endif
                return i;
#else
                return 0;
#endif
            }

            // Gets a shorter source file path for nicer log messages.
            template<StaticString Path>
            consteval auto get_short_name()
            requires std::same_as<typename decltype(Path)::char_type, oschar_t>
            {
                return Path.template substr<internal::get_short_name_index(Path.full_view())>();
            }

            //
            // Log message internals
            //

            bool log_begin(LogLevel level, out<LoggerState*> outState);
            void log_write(oschar_t ch);
            void log_end(LoggerState* state);
            void log_end(LoggerState* state, const oschar_t* sourceFileName, int sourceLine);

            template<Formattable<oschar_t>...Values>
            void log(LogLevel level, OsStringView fmt, const Values&...values)
            {
                LoggerState* state;

                if (internal::log_begin(level, out(state))) {
                    format::write(&log_write, fmt, values...);
                    internal::log_end(state);
                }
            }

            template<Formattable<oschar_t>...Values>
            void log(const oschar_t* sourceFileName, int sourceLine,
                     LogLevel level, OsStringView fmt, const Values&...values)
            {
                LoggerState* state;

                if (internal::log_begin(level, out(state))) {
                    format::write(&log_write, fmt, values...);
                    internal::log_end(state, sourceFileName, sourceLine);
                }
            }

            template<StaticString Fmt, Formattable<oschar_t>...Values>
            void log(LogLevel level, const Values&...values)
            requires (format::validate<oschar_t, Values...>(Fmt.unterminated_view()))
            {
                internal::log(level, Fmt.unterminated_view(), values...);
            }

            template<StaticString Fmt, Formattable<oschar_t>...Values>
            void log(const oschar_t* sourceFileName, int sourceLine,
                     LogLevel level, const Values&...values)
            requires (format::validate<oschar_t, Values...>(Fmt.unterminated_view()))
            {
                internal::log(sourceFileName, sourceLine, level, Fmt.unterminated_view(), values...);
            }

            // No-op log message that still validates the Fmt string at compile time.
            template<StaticString Fmt, Formattable<oschar_t>...Values>
            consteval void discard(const Values&...values)
            requires (format::validate<oschar_t, Values...>(Fmt.unterminated_view()))
            {
            }

            //
            // Fatal error internals
            //

            void fatal_begin(out<LoggerState*> outState);
            void fatal_write(LoggerState* state, oschar_t ch);
            [[noreturn]] void fatal_end(LoggerState* state, const oschar_t* sourceFileName, int sourceLine);

            template<Formattable<oschar_t>...Values>
            [[noreturn]]
            void fatal(const oschar_t* sourceFileName, int sourceLine,
                       OsStringView fmt, const Values&...values)
            {
                LoggerState* state;

                internal::fatal_begin(out(state));
                format::write([&](oschar_t ch) { internal::fatal_write(state, ch); }, fmt, values...);
                internal::fatal_end(state, sourceFileName, sourceLine);
            }

            template<StaticString Fmt, Formattable<oschar_t>...Values>
            [[noreturn]]
            void fatal(const oschar_t* sourceFileName, int sourceLine, const Values&...values)
            requires (format::validate<oschar_t, Values...>(Fmt.unterminated_view()))
            {
                internal::fatal(sourceFileName, sourceLine, Fmt.unterminated_view(), values...);
            }

        } // namespace internal

    } // namespace debug

    /// @}

} // namespace geo

#endif // GEO_SYSTEM_DEBUG_H_INCLUDED
