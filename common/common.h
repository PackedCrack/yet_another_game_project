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
/**
 * @brief Calculates the date a specified number of days before the current date.
 *
 * This function computes the date that is a specified number of days before the current date
 * and returns it as a formatted string.
 *
 * @tparam int_t The type of the integer parameter, which must satisfy the integral concept.
 *
 * @param daysToSubtract The number of days to subtract from the current date.
 *
 * @return A string representing the previous date in the format "YYYY-MM-DD".
 * @retval std::string The formatted date string.
 *
 * @note If `daysToSubtract` is less than or equal to zero, the current date will be returned.
 */
template<typename int_t>
requires std::integral<int_t>
[[nodiscard]] std::string previous_date(int_t daysToSubtract)
{
    std::chrono::sys_days date = std::chrono::year_month_day{ floor<std::chrono::days>(std::chrono::system_clock::now()) };
    if(daysToSubtract > 0)
        date -= std::chrono::days{ daysToSubtract };

    return std::format("{}", date);
}
/**
 * @brief Retrieves today's date formatted as a string.
 *
 * This function returns the current date in the format "YYYY-MM-DD".
 *
 * @return A string representing today's date in the format "YYYY-MM-DD".
 * @retval std::string Todays date.
 */
[[nodiscard]] inline std::string todays_date()
{
    static constexpr int32_t DAYS_TO_SUBTRACT = 0;
    return previous_date(DAYS_TO_SUBTRACT);
}
/**
 * @brief Removes the trailing directory separator from a filesystem path.
 *
 * This function takes a filesystem path and removes the trailing directory separator.
 *
 * @tparam path_t Universal non-const reference to `std::filesystem::path`.
 *
 * @param filepath The filesystem path to be trimmed.
 *
 * @return A `std::filesystem::path` object with the trailing separator removed, if it was present.
 * @retval std::filesystem::path The resulting path without a trailing separator.
 *
 * @note If the input path does not have a trailing separator, it remains unchanged.
 */
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
/**
 * @brief Create a std::span from a contiguous container.
 *
 * This function takes a contiguous container and returns a `std::span` of the specified length,
 * starting from the given offset.
 *
 * @tparam element_t The type of elements in the container.
 * @tparam container_t The type of the container, which must have a contiguous iterator.
 *
 * @param buffer The container to be converted to a `std::span`.
 * @param length The number of elements to include in the span.
 * @param offset The starting offset for the span within the container. Default is 0.
 *
 * @pre The container must be contiguous.
 * @pre The offset must be within the bounds of the container.
 *
 * @return A `std::span` of the specified length starting from the given offset.
 * @retval std::span<element_t> The resulting span from the container.
 *
 * @throws std::out_of_range if the offset is out of bounds of the container.
 *
 * @warning The behavior is undefined if the length specified exceeds the container's size minus the offset.
 *
 * @cppcheck-suppress unknownMacro
 */
template<typename element_t, typename container_t>
requires std::contiguous_iterator<typename container_t::iterator>
[[nodiscard]] std::span<element_t> make_view(container_t& buffer, std::size_t length, int64_t offset = 0u)
{
    // cppcheck-suppress unknownMacro
    ODIN_ASSERT(auto boundsCheck = std::begin(buffer) + offset; boundsCheck <= std::end(buffer));
    return std::span<element_t>{ buffer.data() + offset, length };
}
}    // namespace common