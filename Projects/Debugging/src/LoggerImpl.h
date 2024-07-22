#pragma once

#include "spdlog/spdlog.h"

#include "Logger.h"
//
//
namespace debug
{
class LoggerImpl
{
public:
    static constexpr std::string_view INFO_NAME = "info_logger";
    static constexpr std::string_view WARN_NAME = "warn_logger";
    static constexpr std::string_view ERR_NAME = "err_logger";
    static constexpr std::string_view FATAL_NAME = "fatal_logger";
public:
    explicit LoggerImpl(std::string_view logDirectory);
public:
    void log_info(std::string_view msg);
    void log_warn(std::string_view msg);
    void log_error(std::string_view msg, std::string_view file, std::string_view function, int32_t line);
    void log_fatal(std::string_view msg, std::string_view file, std::string_view function, int32_t line);

    void add_logger(std::string_view name, const std::filesystem::path& output, LoggerLevel level);
private:
    [[nodiscard]] std::optional<spdlog::logger*> find_logger(std::string_view name);
private:
    std::filesystem::path m_LogDirectory;
    std::unordered_map<std::string, spdlog::logger> m_Loggers;
};
}    // namespace debug
