/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef SYSTEM_DEBUG_H_INCLUDED
#define SYSTEM_DEBUG_H_INCLUDED

#include <concepts>
#include <iterator>
#include <optional>
#include <string_view>

#include <fmt/format.h>
#include <fmt/xchar.h>

#include <core/str.h>

#include "encoding.h"

// All log levels
#define LOG_LEVEL_NONE 0
#define LOG_LEVEL_FATAL 1
#define LOG_LEVEL_ERROR 2
#define LOG_LEVEL_WARNING 3
#define LOG_LEVEL_INFO 4
#define LOG_LEVEL_DEBUG 5
#define LOG_LEVEL_TRACE 6

/// @def LOG_LEVEL_MAX
/// Compile-time maximum log level. Log messages with a higher level than this are discarded.
#ifndef LOG_LEVEL_MAX
# define LOG_LEVEL_MAX LOG_LEVEL_INFO
#endif

/// @def LOG_LEVEL_DEFAULT
/// Default log level used on startup.
#ifndef LOG_LEVEL_DEFAULT
# define LOG_LEVEL_DEFAULT LOG_LEVEL_WARNING
#endif
#if LOG_LEVEL_DEFAULT > LOG_LEVEL_MAX
# undef LOG_LEVEL_DEFAULT
# define LOG_LEVEL_DEFAULT LOG_LEVEL_MAX
#endif

/// @def FATAL
/// Logs a fatal error message and exits with an error code.
#if LOG_LEVEL_MAX < LOG_LEVEL_FATAL
# define FATAL(...) ::geo::debug::detail::fatal_silent(OSSTR __VA_ARGS__)
#elif defined(LOG_NO_SOURCE)
# define FATAL(...) ::geo::debug::detail::fatal(OSSTR __VA_ARGS__)
#else
# define FATAL(...) ::geo::debug::detail::fatal_src(__FILE__, __LINE__, OSSTR __VA_ARGS__)
#endif

/// @def LOG_ERROR
/// Logs a non-fatal error message.
#if LOG_LEVEL_MAX < LOG_LEVEL_ERROR
# define LOG_ERROR(...) ::geo::debug::detail::log_nop(OSSTR __VA_ARGS__)
#elif defined(LOG_NO_SOURCE)
# define LOG_ERROR(...) ::geo::debug::detail::log(::geo::LogLevel::error, OSSTR __VA_ARGS__)
#else
# define LOG_ERROR(...) ::geo::debug::detail::log_src(__FILE__, __LINE__, ::geo::LogLevel::error, OSSTR __VA_ARGS__)
#endif

/// @def LOG_WARNING
/// Logs a non-critical issue.
#if LOG_LEVEL_MAX < LOG_LEVEL_WARNING
# define LOG_WARNING(...) ::geo::debug::detail::log_nop(OSSTR __VA_ARGS__)
#elif defined(LOG_NO_SOURCE)
# define LOG_WARNING(...) ::geo::debug::detail::log(::geo::LogLevel::warning, OSSTR __VA_ARGS__)
#else
# define LOG_WARNING(...) ::geo::debug::detail::log_src(__FILE__, __LINE__, ::geo::LogLevel::warning, OSSTR __VA_ARGS__)
#endif

/// @def LOG_INFO
/// Logs a neutral status message.
#if LOG_LEVEL_MAX < LOG_LEVEL_INFO
# define LOG_INFO(...) ::geo::debug::detail::log_nop(OSSTR __VA_ARGS__)
#elif defined(LOG_NO_SOURCE)
# define LOG_INFO(...) ::geo::debug::detail::log(::geo::LogLevel::info, OSSTR __VA_ARGS__)
#else
# define LOG_INFO(...) ::geo::debug::detail::log_src(__FILE__, __LINE__, ::geo::LogLevel::info, OSSTR __VA_ARGS__)
#endif

/// @def LOG_DEBUG
/// Logs an important debugging message.
#if LOG_LEVEL_MAX < LOG_LEVEL_DEBUG
# define LOG_DEBUG(...) ::geo::debug::detail::log_nop(OSSTR __VA_ARGS__)
#elif defined(LOG_NO_SOURCE)
# define LOG_DEBUG(...) ::geo::debug::detail::log(::geo::LogLevel::debug, OSSTR __VA_ARGS__)
#else
# define LOG_DEBUG(...) ::geo::debug::detail::log_src(__FILE__, __LINE__, ::geo::LogLevel::debug, OSSTR __VA_ARGS__)
#endif

/// @def LOG_TRACE
/// Logs a verbose debugging message.
#if LOG_LEVEL_MAX < LOG_LEVEL_TRACE
# define LOG_TRACE(...) ::geo::debug::detail::log_nop(OSSTR __VA_ARGS__)
#elif defined(LOG_NO_SOURCE)
# define LOG_TRACE(...) ::geo::debug::detail::log(::geo::LogLevel::trace, OSSTR __VA_ARGS__)
#else
# define LOG_TRACE(...) ::geo::debug::detail::log_src(__FILE__, __LINE__, ::geo::LogLevel::trace, OSSTR __VA_ARGS__)
#endif

/// @def ASSERT
/// Logs a fatal error message and exits with an error code if a condition is not met. Does nothing
/// if `NDEBUG` is defined.
#ifdef NDEBUG
# define ASSERT(x) do { (void)(x); } while (0)
#elif LOG_LEVEL_MAX < LOG_LEVEL_FATAL
# define ASSERT(x) do { if (!(x)) ::geo::debug::detail::exit_fatal(); } while (0)
#elif defined(LOG_NO_SOURCE)
# define ASSERT(x) do { if (!(x)) ::geo::debug::detail::fatal(OSSTR "Assertion failed: {}", #x); } while (0)
#else
# define ASSERT(x) do { if (!(x)) ::geo::debug::detail::fatal_src(__FILE__, __LINE__, OSSTR "Assertion failed: {}", #x); } while (0)
#endif

namespace geo {

    /// Determines the severity level of a log message.
    enum class LogLevel {
        none = LOG_LEVEL_NONE,
        fatal = LOG_LEVEL_FATAL,
        error = LOG_LEVEL_ERROR,
        warning = LOG_LEVEL_WARNING,
        info = LOG_LEVEL_INFO,
        debug = LOG_LEVEL_DEBUG,
        trace = LOG_LEVEL_TRACE,
    };

    /// Debugging and logging utilities.
    namespace debug {

        void init_logger();
        void shut_down_logger();
        void enable_console();
        void set_max_log_level(LogLevel level);

        namespace detail {

            using FormatContext = fmt::buffered_context<oschar_t>;
            using FormatArgs = fmt::basic_format_args<FormatContext>;
            using StringView = fmt::basic_string_view<oschar_t>;

            extern LogLevel max_log_level;

            [[noreturn]] void exit_fatal();

            // Forwards format arguments, doing conversions if necessary.
            template<fmt::formattable<oschar_t> T>
            constexpr const T& forward(const T& arg) { return arg; }

#ifdef _WIN32
            constexpr Widen forward(char* str) { return Widen{str}; }
            constexpr Widen forward(const char* str) { return Widen{str}; }
            constexpr Widen forward(std::string_view str) { return Widen{str}; }
            constexpr Widen forward(const std::string& str) { return Widen{str}; }
#endif

            // Gets an lvalue reference.
            template<typename T>
            constexpr T& unmove(T& t)
            {
                return t;
            }

            template<typename T>
            constexpr T& unmove(T&& t)
            {
                return t;
            }

            // Concept for formattable (or forwardable) types.
            template<typename T>
            concept Formattable = requires(const T& arg) {
                { detail::forward(arg) } -> fmt::formattable<oschar_t>;
            };

            // Wrapper function for fmt::make_format_args that uses our custom conversions.
            template<Formattable...Args>
            constexpr auto make_format_args(const Args&...args)
            {
                return fmt::make_format_args<FormatContext>(detail::unmove(detail::forward(args))...);
            }

            // Type returned by `forward`.
            template<typename T>
            using forward_t = std::remove_cvref_t<decltype(detail::forward(std::declval<const T&>()))>;

            // String type that enables compile-time format validation.
            template<Formattable...Args>
            using FormatString = fmt::basic_format_string<oschar_t, std::type_identity_t<forward_t<Args>>...>;

            // Platform-dependent implementation for all log messages.
            void vlog_src(const char* file, int line, LogLevel level, StringView fmt, FormatArgs args);

            inline void vlog(LogLevel level, StringView fmt, FormatArgs args)
            {
                detail::vlog_src(nullptr, 0, level, fmt, args);
            }

            // Discards a log message.
            template<Formattable...Args>
            void log_nop(FormatString<Args...>, const Args&...)
            {
            }

            // Logs a message.
            template<Formattable...Args>
            void log(LogLevel level, FormatString<Args...> fmt, const Args&...args)
            {
                detail::vlog(level, fmt.get(), detail::make_format_args(args...));
            }

            // Logs a message with a source location.
            template<Formattable...Args>
            void log_src(const char* file, int line, LogLevel level, FormatString<Args...> fmt, const Args&...args)
            {
                detail::vlog_src(file, line, level, fmt.get(), detail::make_format_args(args...));
            }

            // Discards a fatal error message then exits with an error code.
            template<Formattable...Args>
            [[noreturn]]
            void fatal_silent(FormatString<Args...>, const Args&...)
            {
                exit_fatal();
            }

            // Shows a fatal error message and exits with an error code.
            template<Formattable...Args>
            [[noreturn]]
            void fatal(FormatString<Args...> fmt, const Args&...args)
            {
                detail::vlog(LogLevel::fatal, fmt.get(), detail::make_format_args(args...));
                exit_fatal();
            }

            // Shows a fatal error message with a source location and exits with an error code.
            template<Formattable...Args>
            [[noreturn]]
            void fatal_src(const char* file, int line, FormatString<Args...> fmt, const Args&...args)
            {
                detail::vlog_src(file, line, LogLevel::fatal, fmt.get(), detail::make_format_args(args...));
                exit_fatal();
            }

        } // namespace detail

    } // namespace debug

} // namespace geo

#endif // SYSTEM_DEBUG_H_INCLUDED
