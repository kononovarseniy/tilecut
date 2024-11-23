#include <filesystem>

#include <fmt/format.h>

#include <ka/common/log.hpp>

namespace ka
{

namespace __log_detail
{

std::string format_message(
    const std::string_view type,
    const std::source_location & location,
    const std::string_view message)
{
    constexpr auto here = std::source_location::current().file_name();
    const auto root = std::filesystem::path(here).parent_path().parent_path().parent_path();
    return fmt::format(
        "{} ({}:{}.{}) {}",
        type,
        std::filesystem::path(location.file_name()).lexically_proximate(root).string(),
        location.line(),
        location.column(),
        message);
}

} // namespace __log_detail

void log_assert(
    const std::string_view assert_type,
    const std::string_view condition,
    const std::source_location & location)
{
    log_fatal(fmt::format("{} failed: AR_ASSERT({})", assert_type, condition), location);
}

} // namespace ka
