//
// Created by qwerty on 22/07/2024.
//

#pragma once

#include "debug_defines.h"

#include <memory>
#include <filesystem>
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
enum class LoggerLevel
{
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
    void log_info(std::string_view msg) const;
    void log_warn(std::string_view msg) const;
    void log_error(std::string_view msg, std::string_view file, std::string_view function, int32_t line) const;
    void log_fatal(std::string_view msg, std::string_view file, std::string_view function, int32_t line) const;
    void add_logger(std::string_view name, const std::filesystem::path& output, LoggerLevel level) const;
private:
    std::unique_ptr<Impl> m_pImpl;
};
[[nodiscard]] Logger& logger(std::string_view logpath);

#define LOG_INFO(expr) debug::logger(LOG_DIRECTORY).log_info(expr)
#define LOG_WARN(expr) debug::logger(LOG_DIRECTORY).log_warn(expr)
#define LOG_ERR(expr) debug::logger(LOG_DIRECTORY).log_error(expr, __FILE__, __func__, __LINE__)
#define LOG_FATAL(expr)                                                                                                                    \
    debug::logger(LOG_DIRECTORY).log_fatal(expr, __FILE__, __func__, __LINE__);                                                            \
    DEBUG_BREAK;
}    // namespace debug
