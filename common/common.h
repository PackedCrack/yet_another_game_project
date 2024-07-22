//
// Created by qwerty on 22/07/2024.
//

#pragma once

#include <chrono>
#include <concepts>
#include <filesystem>
#include <format>
#include <string>
//
//
namespace common
{
template<typename int_t>
requires std::integral<int_t>
[[nodiscard]] std::string previous_date(int_t daysToSubtract)
{
    std::chrono::sys_days date = std::chrono::year_month_day{ floor<std::chrono::days>(std::chrono::system_clock::now()) };
    if(daysToSubtract > 0)
        date -= std::chrono::days{ daysToSubtract };


    std::string dateAsStr = std::format("{}", date);
    std::erase_if(dateAsStr, [](char c){ return c == '-'; });

    return dateAsStr;
}
[[nodiscard]] inline std::string todays_date()
{
    static constexpr int32_t DAYS_TO_SUBTRACT = 0;
    return previous_date(DAYS_TO_SUBTRACT);
}
template<typename path_t>
requires std::same_as<std::remove_reference_t<path_t>, std::filesystem::path>
[[nodiscard]] std::filesystem::path trim_trailing_seperator(path_t&& filepath)
{
    std::string tmp{ std::forward<path_t>(filepath).string() };
    if(tmp.ends_with(std::filesystem::path::preferred_separator))
    {
        tmp.pop_back();
        filepath = std::move(tmp);
    }

    return filepath;
}
}    // namespace common