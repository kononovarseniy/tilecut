#include <algorithm>

#include <common/assert.hpp>
#include <tilecut/filter_segments.hpp>

namespace r7
{

inline namespace
{

[[nodiscard]] Segment2s64 flipped(const Segment2s64 & segment) noexcept
{
    return { segment.b, segment.a };
}

[[nodiscard]] Segment2s64 unoriented(const Segment2s64 & segment) noexcept
{
    return segment.a < segment.b ? segment : flipped(segment);
}

} // namespace

void filter_segments(std::vector<Segment2s64> & segments) noexcept
{
    segments.erase(
        std::remove_if(
            segments.begin(),
            segments.end(),
            [](const auto & segment)
            {
                return segment.a == segment.b;
            }),
        segments.end());

    if (segments.empty())
    {
        return;
    }
    std::ranges::sort(segments, {}, unoriented);
    auto out_it = segments.begin();
    s64 counter = 0;
    const auto orient_and_push_segment = [&](const auto & segment)
    {
        AR_PRE(-1 <= counter && counter <= 1);
        if (counter > 0)
        {
            *out_it++ = segment;
        }
        else if (counter < 0)
        {
            *out_it++ = flipped(segment);
        }
    };

    auto main_segment = segments.front();
    counter = 1;
    for (auto it = std::next(segments.begin()); it != segments.end(); ++it)
    {
        if (main_segment == *it)
        {
            ++counter;
        }
        else if (main_segment == flipped(*it))
        {
            --counter;
        }
        else
        {
            orient_and_push_segment(main_segment);
            main_segment = *it;
            counter = 1;
        }
    }
    orient_and_push_segment(main_segment);
    segments.erase(out_it, segments.end());
}

} // namespace r7
