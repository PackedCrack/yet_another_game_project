//
// Created by qwerty on 22/07/2024.
//
#include "Logger.h"
#include "common.h"
#include "debug_defines.h"


#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wlanguage-extension-token"
#elif _MSC_VER
    #pragma warning(disable: 4'239)
#else
    #error Unknown compiler
#endif
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#ifdef __clang__
    #pragma clang diagnostic pop
#elif _MSC_VER
    #pragma warning(enable: 4'239)
#endif
//
//
#undef FMT_EXCEPTIONS
namespace
{
constexpr std::string_view DEBUG_PATTERN = "\n[DEBUG] -  %v%$";
constexpr std::string_view INFO_PATTERN = "\n%^[TIME]    -  %T\n[MESSAGE] -  %v%$";
constexpr std::string_view WARN_PATTERN = "\n%^[TIME]    -  %T\n[THREAD]  -  %t\n[MESSAGE] -  %v%$";
constexpr std::string_view ERR_PATTERN =
    "\n%^[TIME]    -  %T\n[FILE]    -  %s\n[FUNC]    -  %!\n[LINE]    -  %#\n[THREAD]  -  %t\n[MESSAGE] -  %v%$";
//static constexpr const char* vk_pattern = "\n%^[VULKAN] -  %T\n%v%$";

[[nodiscard]] std::string_view pattern_from_level(debug::LoggerLevel level)
{
    using Level = debug::LoggerLevel;

    // clang-format off
    if (level == Level::debug)
    { return DEBUG_PATTERN; }
    else if (level == Level::info)
    { return INFO_PATTERN; }
    else if (level == Level::warn)
    { return WARN_PATTERN; }
    else
    { return ERR_PATTERN; }
    // clang-format on
}
[[nodiscard]] spdlog::level::level_enum to_spdlog_level(debug::LoggerLevel level)
{
    using Level = debug::LoggerLevel;

    UNHANDLED_CASE_PROTECTION_ON
    // clang-format off
    switch (level)
    {
    case Level::debug: return spdlog::level::debug;
    case Level::info: return spdlog::level::info;
    case Level::warn: return spdlog::level::warn;
    case Level::error: return spdlog::level::err;
    case Level::fatal: return spdlog::level::critical;
    }
    // clang-format on
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
class Logger::Impl
{
public:
    static constexpr std::string_view DEBUG_NAME = "debug_log";
    static constexpr std::string_view INFO_NAME = "info_log";
    static constexpr std::string_view WARN_NAME = "warn_log";
    static constexpr std::string_view ERR_NAME = "err_log";
    static constexpr std::string_view FATAL_NAME = "fatal_log";
public:
    explicit Impl(std::string_view logDirectory)
        : m_LogDirectory{ logDirectory.data() }
        , m_Loggers{}
    {
        m_LogDirectory = common::trim_trailing_seperator(m_LogDirectory);
        // Turn off exceptions
        spdlog::set_error_handler([](const std::string& msg) { spdlog::get("console")->error("*** LOGGER ERROR ***: {}", msg); });
    }
public:
    void log_debug(std::string_view msg)
    {
        std::optional<spdlog::logger*> result = find_logger(DEBUG_NAME);
        if (!result)
        {
            add_logger(DEBUG_NAME, m_LogDirectory, LoggerLevel::debug);
            result = find_logger(DEBUG_NAME);
            ODIN_ASSERT(result);
        }

        spdlog::logger* pLogger = result.value();
        if (pLogger->level() != spdlog::level::debug)
        {
            pLogger->set_level(spdlog::level::debug);
        }

        pLogger->debug(msg);
        pLogger->flush();
    }
    void log_info(std::string_view msg)
    {
        std::optional<spdlog::logger*> result = find_logger(INFO_NAME);
        if (!result)
        {
            add_logger(INFO_NAME, m_LogDirectory, LoggerLevel::info);
            result = find_logger(INFO_NAME);
            ODIN_ASSERT(result);
        }

        spdlog::logger* pLogger = result.value();
        pLogger->info(msg);
        pLogger->flush();
    }
    void log_warn(std::string_view msg)
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
    void log_error(std::string_view msg, std::string_view file, std::string_view function, int32_t line)
    {
        std::optional<spdlog::logger*> result = find_logger(ERR_NAME);
        if (!result)
        {
            add_logger(ERR_NAME, m_LogDirectory, LoggerLevel::error);
            result = find_logger(ERR_NAME);
        }

        spdlog::logger* pLogger = result.value();
        pLogger->log({ file.data(), line, function.data() }, spdlog::level::err, msg);
        pLogger->flush();
    }
    void log_fatal(std::string_view msg, std::string_view file, std::string_view function, int32_t line)
    {
        std::optional<spdlog::logger*> result = find_logger(FATAL_NAME);
        if (!result)
        {
            add_logger(FATAL_NAME, m_LogDirectory, LoggerLevel::fatal);
            result = find_logger(FATAL_NAME);
        }

        spdlog::logger* pLogger = result.value();
        pLogger->log({ file.data(), line, function.data() }, spdlog::level::critical, msg);
        pLogger->flush();
    }
    void add_logger(std::string_view name, const std::filesystem::path& output, LoggerLevel level)
    {
        std::filesystem::path logfile = output;
        if (logfile.has_extension())
        {
            logfile = logfile.remove_filename();
            logfile = logfile / std::filesystem::path{ common::todays_date() } / output.filename();
        }
        else
        {
            logfile = logfile / std::filesystem::path{ common::todays_date() } / name;
            logfile = logfile.replace_extension(".txt");
        }

        auto [it, emplaced] = m_Loggers.try_emplace(std::string{ name }, make_logger(name, logfile, level));
        ODIN_ASSERT(emplaced);
    }
private:
    [[nodiscard]] std::optional<spdlog::logger*> find_logger(std::string_view name)
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
private:
    std::filesystem::path m_LogDirectory;
    std::unordered_map<std::string, spdlog::logger> m_Loggers;
};
// Interface
Logger::Logger(std::string_view logDirectory)
    : m_pImpl{ std::make_unique<Impl>(logDirectory) } {};
Logger::~Logger() = default;
Logger::Logger(Logger&& other) noexcept = default;
Logger& Logger::operator=(Logger&& other) noexcept = default;
void Logger::log_debug(std::string_view msg) const
{
    m_pImpl->log_debug(msg);
}
void Logger::log_info(std::string_view msg) const
{
    m_pImpl->log_info(msg);
}
void Logger::log_warn(std::string_view msg) const
{
    m_pImpl->log_warn(msg);
}
void Logger::log_error(std::string_view msg, std::string_view file, std::string_view function, int32_t line) const
{
    m_pImpl->log_error(msg, file, function, line);
}
void Logger::log_fatal(std::string_view msg, std::string_view file, std::string_view function, int32_t line) const
{
    m_pImpl->log_fatal(msg, file, function, line);
}
void Logger::add_logger(std::string_view name, const std::filesystem::path& output, LoggerLevel level) const
{
    m_pImpl->add_logger(name, output, level);
}
namespace details
{
    Logger& get_logger(std::string_view logpath)
    {
        static Logger logger{ logpath };
        return logger;
    }
}    // namespace details
}    // namespace debug
