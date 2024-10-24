#include <tilecut/find_cuts.hpp>

namespace r7
{

inline namespace
{

struct BorderPoint final
{
    u32 position;
    s32 outgoing_vs_incoming;
    bool starts_border_segment;
};

} // namespace

void find_cuts(
    const std::span<const Segment2u16> segments,
    std::vector<Segment2u16> & result,
    const u16 tile_size) noexcept
{
    const auto register_segment = [&](const Vec2u16 a, const Vec2u16 b, const bool reversed)
    {
        if (a.x == 0)
        {
            if (b.x == 0)
            {
            }
        }
    };
}

} // namespace r7
