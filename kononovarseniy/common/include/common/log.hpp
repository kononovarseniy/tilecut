#pragma once

#include <iostream>
#include <source_location>

namespace ka
{

namespace __log_detail
{

[[nodiscard]] std::string format_message(
    std::string_view type,
    const std::source_location & location,
    std::string_view message);

} // namespace __log_detail

inline void log_debug(
    const std::string_view message,
    const std::source_location & location = std::source_location::current())
{
    std::cerr << __log_detail::format_message("D", location, message) << std::endl;
}

inline void log_info(
    const std::string_view message,
    const std::source_location & location = std::source_location::current())
{
    std::cerr << __log_detail::format_message("I", location, message) << std::endl;
}

inline void log_warning(
    const std::string_view message,
    const std::source_location & location = std::source_location::current())
{
    std::cerr << __log_detail::format_message("W", location, message) << std::endl;
}

inline void log_error(
    const std::string_view message,
    const std::source_location & location = std::source_location::current())
{
    std::cerr << __log_detail::format_message("E", location, message) << std::endl;
}

inline void log_fatal(
    const std::string_view message,
    const std::source_location & location = std::source_location::current())
{
    std::cerr << __log_detail::format_message("F", location, message) << std::endl;
}

void log_assert(
    std::string_view assert_type,
    std::string_view condition,
    const std::source_location & location = std::source_location::current());

} // namespace ka
