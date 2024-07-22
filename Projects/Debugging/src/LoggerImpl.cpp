#include "LoggerImpl.h"

// external
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include "engine_defines.h"
#include "common.h"
#include "../../AssetLoader/external/glm/gtc/constants.hpp"


#undef FMT_EXCEPTIONS
namespace
{
constexpr std::string_view INFO_PATTERN = "\n%^[MESSAGE] -  %T\n%v%$";
constexpr std::string_view WARN_PATTERN = "\n%^[THREAD]  -  %t\n[MESSAGE] -  %T\n%v%$";
constexpr std::string_view ERR_PATTERN = "\n%^[FILE]    -  %s\n[FUNC]    -  %!\n[LINE]    -  %#\n[THREAD]  -  %t\n[MESSAGE] -  %T\n%v%$";
//static constexpr const char* vk_pattern = "\n%^[VULKAN] -  %T\n%v%$";

[[nodiscard]] std::string_view pattern_from_level(debug::LoggerLevel level)
{
    using Level = debug::LoggerLevel;

    if (level == Level::info)
    {
        return INFO_PATTERN;
    }
    else if (level == Level::warn)
    {
        return WARN_PATTERN;
    }
    else
    {
        return ERR_PATTERN;
    }
}
[[nodiscard]] spdlog::level::level_enum to_spdlog_level(debug::LoggerLevel level)
{
    using Level = debug::LoggerLevel;

    UNHANDLED_CASE_PROTECTION_ON
    switch (level)
    {
    case Level::info:
        return spdlog::level::info;
    case Level::warn:
        return spdlog::level::warn;
    case Level::error:
        return spdlog::level::err;
    case Level::fatal:
        return spdlog::level::critical;
    }
    UNHANDLED_CASE_PROTECTION_OFF

    std::unreachable();
}
[[nodiscard]] spdlog::logger make_logger(std::string_view name, const std::filesystem::path& logfile, debug::LoggerLevel level)
{
    auto pFileInfo = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logfile.string(), false);
    pFileInfo->set_level(to_spdlog_level(level));

    auto pConsoleInfo = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    pConsoleInfo->set_level(to_spdlog_level(level));

    std::string_view pattern = pattern_from_level(level);
    pConsoleInfo->set_pattern(pattern.data());

    return spdlog::logger{
        name.data(),
        { pConsoleInfo, pFileInfo }
    };
}
}    // namespace
namespace debug
{
LoggerImpl::LoggerImpl(std::string_view logDirectory)
    : m_LogDirectory{ logDirectory.data() }
    , m_Loggers{}
{
    m_LogDirectory = common::trim_trailing_seperator(m_LogDirectory);
    // Turn off exceptions
    spdlog::set_error_handler([](const std::string& msg) { spdlog::get("console")->error("*** LOGGER ERROR ***: {}", msg); });
}
void LoggerImpl::log_info(std::string_view msg)
{
    std::optional<spdlog::logger*> result = find_logger(INFO_NAME);
    if (!result)
    {
        add_logger(INFO_NAME, m_LogDirectory, LoggerLevel::info);
        result = find_logger(INFO_NAME);
    }

    spdlog::logger* pLogger = result.value();
    pLogger->warn(msg);
    pLogger->flush();
}
void LoggerImpl::log_warn(std::string_view msg)
{
    std::optional<spdlog::logger*> result = find_logger(WARN_NAME);
    if (!result)
    {
        add_logger(WARN_NAME, m_LogDirectory, LoggerLevel::warn);
        result = find_logger(WARN_NAME);
    }

    spdlog::logger* pLogger = result.value();
    pLogger->warn(msg);
    pLogger->flush();
}
void LoggerImpl::log_error(std::string_view msg, std::string_view file, std::string_view function, int32_t line)
{
    std::optional<spdlog::logger*> result = find_logger(ERR_NAME);
    if (!result)
    {
        add_logger(ERR_NAME, m_LogDirectory, LoggerLevel::error);
        result = find_logger(ERR_NAME);
    }

    spdlog::logger* pLogger = result.value();
    pLogger->log({ msg.data(), line, function.data() }, spdlog::level::err, msg);
    pLogger->flush();
}
void LoggerImpl::log_fatal(std::string_view msg, std::string_view file, std::string_view function, int32_t line)
{
    std::optional<spdlog::logger*> result = find_logger(FATAL_NAME);
    if (!result)
    {
        add_logger(FATAL_NAME, m_LogDirectory, LoggerLevel::error);
        result = find_logger(FATAL_NAME);
    }

    spdlog::logger* pLogger = result.value();
    pLogger->log({ msg.data(), line, function.data() }, spdlog::level::critical, msg);
    pLogger->flush();
}
void LoggerImpl::add_logger(std::string_view name, const std::filesystem::path& output, LoggerLevel level)
{
    std::filesystem::path logfile = output;
    logfile = logfile.remove_filename();

    const auto date = std::filesystem::path{ common::todays_date() };
    logfile = logfile / date / output.filename();

    auto [it, emplaced] = m_Loggers.try_emplace(std::string{ name }, make_logger(name, logfile, level));
    ODIN_ASSERT(emplaced);
}
std::optional<spdlog::logger*> LoggerImpl::find_logger(std::string_view name)
{
    auto iter = m_Loggers.find(name.data());
    if (iter == std::end(m_Loggers))
    {
        return std::nullopt;
    }
    else
    {
        return std::optional<spdlog::logger*>{ std::in_place, &(iter->second) };
    }
}
}    // namespace debug
