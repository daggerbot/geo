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
/// Shows a fatal error message and terminates the process.
#ifdef LOG_NO_SOURCE
# define FATAL(FMT, ...) ::geo::LogSystem::get().fatal<OSSTR FMT>(nullptr, 0 __VA_OPT__(,) __VA_ARGS__)
#else
# define FATAL(FMT, ...) ::geo::LogSystem::get().fatal<OSSTR FMT>(::geo::LogSystem::get_short_path<OSSTR __FILE__>().data, __LINE__ __VA_OPT__(,) __VA_ARGS__)
#endif

/// @def LOG_ERROR
/// Logs an error message.
#ifdef LOG_NO_SOURCE
# define LOG_ERROR(FMT, ...) ::geo::LogSystem::get().log<OSSTR FMT>(::geo::LogLevel::error, nullptr, 0 __VA_OPT__(,) __VA_ARGS__)
#else
# define LOG_ERROR(FMT, ...) ::geo::LogSystem::get().log<OSSTR FMT>(::geo::LogLevel::error, ::geo::LogSystem::get_short_path<OSSTR __FILE__>().data, __LINE__ __VA_OPT__(,) __VA_ARGS__)
#endif

/// @def LOG_WARNING
/// Logs a warning message.
#ifdef LOG_NO_SOURCE
# define LOG_WARNING(FMT, ...) ::geo::LogSystem::get().log<OSSTR FMT>(::geo::LogLevel::warning, nullptr, 0 __VA_OPT__(,) __VA_ARGS__)
#else
# define LOG_WARNING(FMT, ...) ::geo::LogSystem::get().log<OSSTR FMT>(::geo::LogLevel::warning, ::geo::LogSystem::get_short_path<OSSTR __FILE__>().data, __LINE__ __VA_OPT__(,) __VA_ARGS__)
#endif

/// @def LOG_NOTICE
/// Logs an important status message.
#ifdef LOG_NO_SOURCE
# define LOG_NOTICE(FMT, ...) ::geo::LogSystem::get().log<OSSTR FMT>(::geo::LogLevel::notice, nullptr, 0 __VA_OPT__(,) __VA_ARGS__)
#else
# define LOG_NOTICE(FMT, ...) ::geo::LogSystem::get().log<OSSTR FMT>(::geo::LogLevel::notice, ::geo::LogSystem::get_short_path<OSSTR __FILE__>().data, __LINE__ __VA_OPT__(,) __VA_ARGS__)
#endif

/// @def LOG_INFO
/// Logs a neutral status message.
#ifdef LOG_NO_SOURCE
# define LOG_INFO(FMT, ...) ::geo::LogSystem::get().log<OSSTR FMT>(::geo::LogLevel::info, nullptr, 0 __VA_OPT__(,) __VA_ARGS__)
#else
# define LOG_INFO(FMT, ...) ::geo::LogSystem::get().log<OSSTR FMT>(::geo::LogLevel::info, ::geo::LogSystem::get_short_path<OSSTR __FILE__>().data, __LINE__ __VA_OPT__(,) __VA_ARGS__)
#endif

/// @def LOG_DEBUG
/// Logs a message on debug builds only.
#ifdef NDEBUG
# define LOG_DEBUG(FMT, ...) ::geo::LogSystem::discard<OSSTR FMT>(__VA_ARGS__)
#elif defined(LOG_NO_SOURCE)
# define LOG_DEBUG(FMT, ...) ::geo::LogSystem::get().log<OSSTR FMT>(::geo::LogLevel::debug, nullptr, 0 __VA_OPT__(,) __VA_ARGS__)
#else
# define LOG_DEBUG(FMT, ...) ::geo::LogSystem::get().log<OSSTR FMT>(::geo::LogLevel::debug, ::geo::LogSystem::get_short_path<OSSTR __FILE__>().data, __LINE__ __VA_OPT__(,) __VA_ARGS__)
#endif

/// @def LOG_TRACE
/// Logs a message that is only visible on debug builds with verbose messages explicitly enabled.
#ifdef NDEBUG
# define LOG_TRACE(FMT, ...) ::geo::LogSystem::discard<OSSTR FMT>(__VA_ARGS__)
#elif defined(LOG_NO_SOURCE)
# define LOG_TRACE(FMT, ...) ::geo::LogSystem::get().log<OSSTR FMT>(::geo::LogLevel::trace, nullptr, 0 __VA_OPT__(,) __VA_ARGS__)
#else
# define LOG_TRACE(FMT, ...) ::geo::LogSystem::get().log<OSSTR FMT>(::geo::LogLevel::trace, ::geo::LogSystem::get_short_path<OSSTR __FILE__>().data, __LINE__ __VA_OPT__(,) __VA_ARGS__)
#endif

/// @def DASSERT
/// Shows an error message and exits if a condition is not met on debug builds. Does nothing on
/// release builds.
#ifdef NDEBUG
# define DASSERT(x) do { (void)(x); } while (0)
#elif defined(LOG_NO_SOURCE)
# define DASSERT(x) do { if (!(x)) ::geo::LogSystem::get().fatal<OSSTR "Assertion failed: {}">(nullptr, 0, OSSTR #x); } while (0)
#else
# define DASSERT(x) do { if (!(x)) ::geo::LogSystem::get().fatal<OSSTR "Assertion failed: {}">(::geo::LogSystem::get_short_path<OSSTR __FILE__>().data, __LINE__, OSSTR #x); } while (0)
#endif

/// @}

namespace geo {

    /// @addtogroup geo_system
    /// @{

    /// Indicates the severity level of a log message.
    enum class LogLevel {
        /// A fatal error has occurred, and the process will terminate shortly.
        fatal,

        /// A non-fatal error has occurred, and the program will try to continue.
        error,

        /// Something non-critical has gone wrong.
        warning,

        /// An important status message.
        notice,

        /// A neural status message.
        info,

        /// A message that is only visible on debug builds.
        debug,

        /// A message that is only visible on debug builds with verbose messages explicitly enabled.
        trace,

        min = fatal,
#ifdef NDEBUG
        max = info,
        default_ = notice,
#else
        max = trace,
        default_ = debug,
#endif
    };

    /// System for writing messages that can be used for debugging purposes.
    class LogSystem {
    public:
        LogSystem(const LogSystem&) = delete;
        LogSystem& operator=(const LogSystem&) = delete;

        /// Checks the format of a log message at compile time, but does nothing at runtime.
        template<StaticString Fmt, Formattable<oschar_t>...Values>
        static consteval void discard(const Values&...values)
        requires (format::validate<oschar_t, Values...>(Fmt.unterminated_view()))
        {
        }

        /// Enables verbose log messages.
        void enable_verbose() { m_maxLevel = LogLevel::max; }

        /// Logs a fatal error message and terminates the process with an error code.
        template<StaticString Fmt, Formattable<oschar_t>...Values>
        [[noreturn]]
        void fatal(const oschar_t* file, int line, const Values&...values)
        requires (format::validate<oschar_t, Values...>(Fmt.unterminated_view()))
        {
            fatal(file, line, Fmt.unterminated_view(), values...);
        }

        /// Gets the global logger instance.
        static LogSystem& get();

        /// Gets a shorter source path for log messages by removing the value of the
        /// `LOG_PATH_PREFIX` macro (if defined) from the beginning.
        template<StaticString Path>
        static consteval auto get_short_path()
        requires std::same_as<typename decltype(Path)::char_type, oschar_t>
        {
            return Path.template substr<get_short_path_index(Path.full_view())>();
        }

        /// Writes a log message.
        template<StaticString Fmt, Formattable<oschar_t>...Values>
        void log(LogLevel level, const oschar_t* file, int line, const Values&...values)
        requires (format::validate<oschar_t, Values...>(Fmt.unterminated_view()))
        {
            log(level, file, line, Fmt.unterminated_view(), values...);
        }

    private:
        LogLevel m_maxLevel = LogLevel::default_;
        std::recursive_mutex m_mutex;
        bool m_locked = false; // Indicates whether the current thread is already writing a message
        OsString m_fatalMessage;

        LogSystem();
        ~LogSystem();

        // log() internals
        bool log_begin(LogLevel level);
        static void log_write(oschar_t ch);
        void log_end(const oschar_t* file, int line);

        template<Formattable<oschar_t>...Values>
        void log(LogLevel level, const oschar_t* file, int line, OsStringView fmt, const Values&...values)
        {
            if (log_begin(level)) {
                format::write([](oschar_t ch) { log_write(ch); }, fmt, values...);
                log_end(file, line);
            }
        }

        // fatal() internals
        void fatal_begin();
        void fatal_write(oschar_t ch);
        [[noreturn]] void fatal_end(const oschar_t* file, int line);

        template<Formattable<oschar_t>...Values>
        [[noreturn]]
        void fatal(const oschar_t* file, int line, OsStringView fmt, const Values&...values)
        {
            fatal_begin();
            format::write([this](oschar_t ch) { fatal_write(ch); }, fmt, values...);
            fatal_end(file, line);
        }

        // Gets the index of the first character in the source path after LOG_PATH_PREFIX.
        static consteval size_t get_short_path_index(OsStringView path)
        {
#ifdef LOG_PATH_PREFIX
            constexpr oschar_t prefix[] = OSSTR LOG_PATH_PREFIX;

            size_t index = 0;

            // Skip LOG_PATH_PREFIX
            for (;;) {
                if (!prefix[index])
                    break;
                else if (path[index] != prefix[index])
                    return 0;
                else
                    ++index;
            }

            // Skip path separators
            if (path[index] == '/')
                ++index;
# ifdef _WIN32
            else if (path[index] == '\\')
                ++index;
# endif
            return index;
#else // !defined(LOG_PATH_PREFIX)
            return 0;
#endif
        }
    };

    /// @}

} // namespace geo

#endif // GEO_SYSTEM_DEBUG_H_INCLUDED
