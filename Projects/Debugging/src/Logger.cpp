//
// Created by qwerty on 22/07/2024.
//
#include "Logger.h"
#include "LoggerImpl.h"
//
//
namespace debug
{
Logger::Logger(std::string_view msg)
    : m_pLogger{ std::make_unique<LoggerImpl>(msg) } {};
void Logger::log_info(std::string_view msg) const
{
    m_pLogger->log_info(msg);
}
void Logger::log_warn(std::string_view msg) const
{
    m_pLogger->log_warn(msg);
}
void Logger::log_error(std::string_view msg, std::string_view file, std::string_view function, int32_t line) const
{
    m_pLogger->log_error(msg, file, function, line);
}
void Logger::log_fatal(std::string_view msg, std::string_view file, std::string_view function, int32_t line) const
{
    m_pLogger->log_fatal(msg, file, function, line);
}
void Logger::add_logger(std::string_view name, const std::filesystem::path& output, LoggerLevel level) const
{
    m_pLogger->add_logger(name, output, level);
}
Logger& logger(std::string_view logpath)
{
    static Logger logger{ logpath };
    return logger;
}
}    // namespace debug
