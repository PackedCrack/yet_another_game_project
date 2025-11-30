//
// Created by qwerty on 22/07/2024.
//

#pragma once

#include "debug_defines.hpp"
// std
#include <exception>
#include <memory>
#include <filesystem>
#include <stacktrace>
#include <string_view>


#ifndef SELF_COMPILIATION
    #ifndef LOG_DIRECTORY
        #error You must define the log directory before including this file!
    #endif
#else
    #define LOG_DIRECTORY "null"
#endif
//
//
namespace debug
{
class fatal : public std::exception
{
public:
    explicit fatal(std::string msg)
        : m_Msg{ std::move(msg) }
    {}
    [[nodiscard]] const char* what() const noexcept override { return m_Msg.c_str(); }
private:
    std::string m_Msg;
};
enum class LoggerLevel
{
    debug,
    info,
    warn,
    error,
    fatal
};
class Logger
{
    class Impl;
public:
    explicit Logger(std::string_view logDirectory);
    ~Logger();
    Logger(const Logger& other) = delete;
    Logger(Logger&& other) noexcept;
    Logger& operator=(const Logger& other) = delete;
    Logger& operator=(Logger&& other) noexcept;
public:
    void log_debug(std::string_view msg) const;
    void log_info(std::string_view msg) const;
    void log_warn(std::string_view msg) const;
    void log_error(std::string_view msg, std::string_view file, std::string_view function, int32_t line) const;
    void log_fatal(std::string_view msg, std::string_view file, std::string_view function, int32_t line) const;
    void add_logger(std::string_view name, const std::filesystem::path& output, LoggerLevel level) const;
private:
    std::unique_ptr<Impl> m_pImpl;
};
namespace details
{
    [[nodiscard]] Logger& get_logger(std::string_view logpath);
    template<LoggerLevel level, typename... args_t>
    constexpr void log(std::string_view msg, args_t&&... args)
    {
        const Logger& logger = get_logger(LOG_DIRECTORY);

        if constexpr (level == LoggerLevel::debug)
        {
            logger.log_debug(std::vformat(msg, std::make_format_args(args...)));
        }
        else if constexpr (level == LoggerLevel::info)
        {
            logger.log_info(std::vformat(msg, std::make_format_args(args...)));
        }
        else if constexpr (level == LoggerLevel::warn)
        {
            logger.log_warn(std::vformat(msg, std::make_format_args(args...)));
        }
        else
        {
            static_assert(level != LoggerLevel::error && level != LoggerLevel::fatal);
        }
    }
    template<LoggerLevel level, typename... args_t>
    constexpr void log_e(std::string_view file, std::string_view func, int32_t line, std::string_view msg, args_t&&... args)
    {
        static_assert(level == LoggerLevel::error);
        const Logger& logger = get_logger(LOG_DIRECTORY);
        logger.log_error(std::vformat(msg, std::make_format_args(args...)), file, func, line);
    }
    template<LoggerLevel level, typename... args_t>
    constexpr void
    log_f(std::string_view file, std::string_view func, int32_t line, auto&& stacktrace, std::string_view msg, args_t&&... args)
    {
        static_assert(level == LoggerLevel::fatal);
        const Logger& logger = get_logger(LOG_DIRECTORY);
        logger.log_fatal(std::format("{}\n\n{}", std::vformat(msg, std::make_format_args(args...)), stacktrace), file, func, line);
    }
}    // namespace details
#define LOG_INFO(...) debug::details::log<debug::LoggerLevel::info>(__VA_ARGS__)
#define LOG_WARN(...) debug::details::log<debug::LoggerLevel::warn>(__VA_ARGS__)
#define LOG_ERR(...) debug::details::log_e<debug::LoggerLevel::error>(__FILE__, __func__, __LINE__, __VA_ARGS__)
#define LOG_FATAL(...)                                                                                                                     \
    debug::details::log_f<debug::LoggerLevel::fatal>(__FILE__, __func__, __LINE__, std::stacktrace::current(), __VA_ARGS__);               \
    throw debug::fatal{ "Fatal error occured. See stack trace in log." };

#ifndef NDEBUG
    #define LOG_DEBUG(...) debug::details::log<debug::LoggerLevel::debug>(__VA_ARGS__)
#else
    #define LOG_DEBUG(...)
#endif
}    // namespace debug
