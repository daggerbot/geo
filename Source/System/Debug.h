/*
 * Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef GEO_SYSTEM_DEBUG_H_INCLUDED
#define GEO_SYSTEM_DEBUG_H_INCLUDED

#include <optional>

#include <Core/Format.h>
#include <Core/StringTypes.h>

/// @addtogroup System
/// @{

#define LOG_LEVEL_OFF 0 ///< All logging is disabled.
#define LOG_LEVEL_FATAL 1 ///< Log level for fatal errors.
#define LOG_LEVEL_ERROR 2 ///< Log level for non-fatal errors.
#define LOG_LEVEL_WARNING 3 ///< Log level for non-critical issues.
#define LOG_LEVEL_INFO 4 ///< Log level for neutral status messages.
#define LOG_LEVEL_DEBUG 5 ///< Log level for important debug messages.
#define LOG_LEVEL_TRACE 6 ///< Log level for detailed debug messages.

/// @def LOG_LEVEL_MAX
/// The highest log level compiled into the game executable. When using the appropriate logging
/// macros, any messages with a level higher than this are discarded and do not appear in the
/// compiled game executable.
#ifndef LOG_LEVEL_MAX
# define LOG_LEVEL_MAX LOG_LEVEL_INFO
#endif

/// @def LOG_LEVEL_DEFAULT
/// The default log level on startup.
#ifndef LOG_LEVEL_DEFAULT
# define LOG_LEVEL_DEFAULT LOG_LEVEL_WARNING
#endif
#if LOG_LEVEL_DEFAULT > LOG_LEVEL_MAX
# undef LOG_LEVEL_DEFAULT
# define LOG_LEVEL_DEFAULT LOG_LEVEL_MAX
#endif

/// @def FATAL
/// Logs an error message and terminates the process with an error code.
#if LOG_LEVEL_MAX < LOG_LEVEL_FATAL
# define FATAL(FMT, ...) ::Geo::Debug::Internal::FatalSilent<OSSTR FMT>(__VA_ARGS__)
#elif defined(LOG_NO_SOURCE)
# define FATAL(FMT, ...) ::Geo::Debug::Internal::Fatal<OSSTR FMT>(__VA_ARGS__)
#else
# define FATAL(FMT, ...) ::Geo::Debug::Internal::FatalSrc<OSSTR FMT>(__FILE__, __LINE__ __VA_OPT__(,) __VA_ARGS__)
#endif

/// @def LOG_ERROR
/// Logs a non-fatal error message.
#if LOG_LEVEL_MAX < LOG_LEVEL_ERROR
# define LOG_ERROR(FMT, ...) ::Geo::Debug::Internal::Discard<OSSTR FMT>(__VA_ARGS__)
#elif defined(LOG_NO_SOURCE)
# define LOG_ERROR(FMT, ...) ::Geo::Debug::Internal::Log<OSSTR FMT>(::Geo::LogLevel::Error __VA_OPT__(,) __VA_ARGS__)
#else
# define LOG_ERROR(FMT, ...) ::Geo::Debug::Internal::LogSrc<OSSTR FMT>(__FILE__, __LINE__, ::Geo::LogLevel::Error __VA_OPT__(,) __VA_ARGS__)
#endif

/// @def LOG_WARNING
/// Logs a non-critical issue.
#if LOG_LEVEL_MAX < LOG_LEVEL_WARNING
# define LOG_WARNING(FMT, ...) ::Geo::Debug::Internal::Discard<OSSTR FMT>(__VA_ARGS__)
#elif defined(LOG_NO_SOURCE)
# define LOG_WARNING(FMT, ...) ::Geo::Debug::Internal::Log<OSSTR FMT>(::Geo::LogLevel::Warning __VA_OPT__(,) __VA_ARGS__)
#else
# define LOG_WARNING(FMT, ...) ::Geo::Debug::Internal::LogSrc<OSSTR FMT>(__FILE__, __LINE__, ::Geo::LogLevel::Warning __VA_OPT__(,) __VA_ARGS__)
#endif

/// @def LOG_INFO
/// Logs a neutral status message.
#if LOG_LEVEL_MAX < LOG_LEVEL_INFO
# define LOG_INFO(FMT, ...) ::Geo::Debug::Internal::Discard<OSSTR FMT>(__VA_ARGS__)
#elif defined(LOG_NO_SOURCE)
# define LOG_INFO(FMT, ...) ::Geo::Debug::Internal::Log<OSSTR FMT>(::Geo::LogLevel::Info __VA_OPT__(,) __VA_ARGS__)
#else
# define LOG_INFO(FMT, ...) ::Geo::Debug::Internal::LogSrc<OSSTR FMT>(__FILE__, __LINE__, ::Geo::LogLevel::Info __VA_OPT__(,) __VA_ARGS__)
#endif

/// @def LOG_DEBUG
/// Logs an important debug message.
#if LOG_LEVEL_MAX < LOG_LEVEL_DEBUG
# define LOG_DEBUG(FMT, ...) ::Geo::Debug::Internal::Discard<OSSTR FMT>(__VA_ARGS__)
#elif defined(LOG_NO_SOURCE)
# define LOG_DEBUG(FMT, ...) ::Geo::Debug::Internal::Log<OSSTR FMT>(::Geo::LogLevel::Debug __VA_OPT__(,) __VA_ARGS__)
#else
# define LOG_DEBUG(FMT, ...) ::Geo::Debug::Internal::LogSrc<OSSTR FMT>(__FILE__, __LINE__, ::Geo::LogLevel::Debug __VA_OPT__(,) __VA_ARGS__)
#endif

/// @def LOG_TRACE
/// Logs a detailed debug message.
#if LOG_LEVEL_MAX < LOG_LEVEL_TRACE
# define LOG_TRACE(FMT, ...) ::Geo::Debug::Internal::Discard<OSSTR FMT>(__VA_ARGS__)
#elif defined(LOG_NO_SOURCE)
# define LOG_TRACE(FMT, ...) ::Geo::Debug::Internal::Log<OSSTR FMT>(::Geo::LogLevel::Trace __VA_OPT__(,) __VA_ARGS__)
#else
# define LOG_TRACE(FMT, ...) ::Geo::Debug::Internal::LogSrc<OSSTR FMT>(__FILE__, __LINE__, ::Geo::LogLevel::Trace __VA_OPT__(,) __VA_ARGS__)
#endif

/// @def ASSERT
/// Logs an error message and terminates the process with an error code if a condition is not met.
/// If `NDEBUG` is defined, this does nothing.
#ifdef NDEBUG
# define ASSERT(x) do { (void)(x); } while (0)
#elif LOG_LEVEL_MAX < LOG_LEVEL_FATAL
# define ASSERT(x) do { if (!(x)) ::Geo::Debug::Internal::ExitFatal(); } while (0)
#elif defined(LOG_NO_SOURCE)
# define ASSERT(x) do { if (!(x)) ::Geo::Debug::Internal::Fatal<OSSTR "Assertion failed: {}">(#x); } while (0)
#else
# define ASSERT(x) do { if (!(x)) ::Geo::Debug::Internal::FatalSrc<OSSTR "Assertion failed: {}">(__FILE__, __LINE__, #x); } while (0)
#endif

/// @}

namespace Geo {

    /// @addtogroup System
    /// @{

    /// Indicates the severity of a log message.
    enum class LogLevel {
        Off = LOG_LEVEL_OFF, ///< All logging is disabled.
        Fatal = LOG_LEVEL_FATAL, ///< Fatal error.
        Error = LOG_LEVEL_ERROR, ///< Non-fatal error.
        Warning = LOG_LEVEL_WARNING, ///< Non-critical issue.
        Info = LOG_LEVEL_INFO, ///< Neutral status message.
        Debug = LOG_LEVEL_DEBUG, ///< Important debug message.
        Trace = LOG_LEVEL_TRACE, ///< Detailed debug message.
    };

    /// Debugging utilities.
    namespace Debug {

        /// Sets the global maximum log level.
        void SetMaxLogLevel(LogLevel level);

        std::optional<LogLevel> ParseLogLevel(OsStringView str);

        namespace Internal {

            // System implementation for non-fatal log messages.
            bool BeginLogMessage(LogLevel level);
            void WriteLogMessage(OsStringView str);
            void EndLogMessage();
            void EndLogMessage(const oschar_t* file, int line);

            template<StaticString Fmt, Formattable<oschar_t>...Args>
            consteval void Discard(const Args&...)
            requires (Format::Validate<oschar_t, Args...>(Fmt.GetUnterminatedView()))
            {
            }

            template<Formattable<oschar_t>...Args>
            void LogImpl(LogLevel level, OsStringView fmt, const Args&...args)
            {
                if (BeginLogMessage(level)) {
                    Format::Write(&WriteLogMessage, fmt, args...);
                    EndLogMessage();
                }
            }

            template<StaticString Fmt, Formattable<oschar_t>...Args>
            void Log(LogLevel level, const Args&...args)
            requires (Format::Validate<oschar_t, Args...>(Fmt.GetUnterminatedView()))
            {
                LogImpl(level, Fmt.GetUnterminatedView(), args...);
            }

            template<Formattable<oschar_t>...Args>
            void LogSrcImpl(const oschar_t* file, int line, LogLevel level, OsStringView fmt, const Args&...args)
            {
                if (BeginLogMessage(level)) {
                    Format::Write(&WriteLogMessage, fmt, args...);
                    EndLogMessage(file, line);
                }
            }

            template<StaticString Fmt, Formattable<oschar_t>...Args>
            void LogSrc(const oschar_t* file, int line, LogLevel level, const Args&...args)
            requires (Format::Validate<oschar_t, Args...>(Fmt.GetUnterminatedView()))
            {
                LogSrcImpl(file, line, level, Fmt.GetUnterminatedView(), args...);
            }

            // System implementation for fatal log messages.
            void BeginFatalError();
            void WriteFatalError(OsStringView str);
            [[noreturn]] void EndFatalError();
            [[noreturn]] void EndFatalError(const oschar_t* file, int line);
            [[noreturn]] void ExitFatal();

            template<StaticString Fmt, Formattable<oschar_t>...Args>
            [[noreturn]] consteval void FatalSilent(const Args&...)
            requires (Format::Validate<oschar_t, Args...>(Fmt.GetUnterminatedView()))
            {
                ExitFatal();
            }

            template<Formattable<oschar_t>...Args>
            [[noreturn]] void FatalImpl(OsStringView fmt, const Args&...args)
            {
                BeginFatalError();
                Format::Write(&WriteFatalError, fmt, args...);
                EndFatalError();
            }

            template<StaticString Fmt, Formattable<oschar_t>...Args>
            [[noreturn]] void Fatal(const Args&...args)
            requires (Format::Validate<oschar_t, Args...>(Fmt.GetUnterminatedView()))
            {
                FatalImpl(Fmt.GetUnterminatedView(), args...);
            }

            template<Formattable<oschar_t>...Args>
            [[noreturn]] void FatalSrcImpl(const oschar_t* file, int line, OsStringView fmt, const Args&...args)
            {
                BeginFatalError();
                Format::Write(&WriteFatalError, fmt, args...);
                EndFatalError(file, line);
            }

            template<StaticString Fmt, Formattable<oschar_t>...Args>
            [[noreturn]] void FatalSrc(const oschar_t* file, int line, const Args&...args)
            requires (Format::Validate<oschar_t, Args...>(Fmt.GetUnterminatedView()))
            {
                FatalSrcImpl(file, line, Fmt.GetUnterminatedView(), args...);
            }

        } // namespace Internal

    } // namespace Debug

    /// @}

} // namespace Geo

#endif // GEO_SYSTEM_DEBUG_H_INCLUDED
