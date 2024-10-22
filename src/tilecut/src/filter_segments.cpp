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
    if (segments.empty())
    {
        return;
    }
    std::ranges::sort(segments, {}, unoriented);
    auto out_it = segments.begin();
    const Segment2s64 * main_segment;
    s64 counter = 0;
    for (auto it = out_it; it != segments.end(); ++it)
    {
        if (main_segment == nullptr)
        {
            if (it->a != it->b)
            {
                main_segment = &*it;
                counter = 1;
            }
        }
        else if (unoriented(*main_segment) == unoriented(*it))
        {
            if (*main_segment == *it)
            {
                ++counter;
            }
            else
            {
                --counter;
            }
        }
        else
        {
            AR_PRE(-1 <= counter && counter <= 1);
            if (counter > 0)
            {
                *out_it++ = *main_segment;
            }
            else if (counter < 0)
            {
                *out_it++ = flipped(*main_segment);
            }
        }
    }
    segments.erase(out_it, segments.end());
}

} // namespace r7
