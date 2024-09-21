#include <span>
#include <utility>

#include <common/assert.hpp>

#include <tilecut/orient.h>
#include <tilecut/tilecut.h>

namespace ka::tilecut::triangulation
{

struct TriangulationResult final
{
    //! Полуребро выпуклой оболочки против часовой стрелки из самой левой вершины.
    Halfedge * leftmost;
    //! Полуребро выпуклой оболочки по часовой стрелке из самой правой вершины.
    Halfedge * rightmost;
};

struct Triangulation final
{
    std::vector<uvec16> points;
    std::vector<Halfedge> halfedges;
    Halfedge * free_edge;
};

[[nodiscard]] Halfedge * alloc_halfedge(Triangulation & state)
{
    AR_PRE(state.free_edge != nullptr);
    return std::exchange(state.free_edge, state.free_edge->next);
}

[[nodiscard]] Halfedge * create_segment(Triangulation & state, uvec16 * src, uvec16 * dst)
{
    AR_PRE(state.points.data() <= src && src < state.points.data() + state.points.size());
    AR_PRE(state.points.data() <= dst && dst < state.points.data() + state.points.size());
    const auto new_halfedge = alloc_halfedge(state);
    const auto sym_halfedge = alloc_halfedge(state);
    *new_halfedge = {
        .point = src,
        .sym = sym_halfedge,
        .next = sym_halfedge,
        .prev = sym_halfedge,
    };
    *sym_halfedge = {
        .point = dst,
        .sym = new_halfedge,
        .next = new_halfedge,
        .prev = new_halfedge,
    };
    return new_halfedge;
}

void splice(Halfedge * a, Halfedge * b) noexcept
{
    a->prev->next = b;
    b->prev->next = a;
    std::swap(a->prev, b->prev);
}

Halfedge * connect(Triangulation & state, Halfedge * a, Halfedge * b) noexcept
{
    const auto edge = create_segment(state, a->point, b->point);
    splice(a->sym, edge);
    splice(edge->sym, b);
    return edge;
}

[[nodiscard]] PointLocation point_location(Halfedge * halfedge, uvec16 * point) noexcept
{
    return PointLocation(*halfedge->point, *halfedge->sym->point, *point);
}

//! Триангулирует набор точек.
//! Делаем как в статье, но не триангуляцию Делоне.
//! Guibas, L. and Stolfi, J., "Primitives for the Manipulation of General Subdivisions and the Computation of Voronoi Diagrams", ACM Transactions on Graphics, Vol.4, No.2, April 1985, pages 74-123.
[[nodiscard]] TriangulationResult triangulate(Triangulation & state, std::span<uvec16> points)
{
    AR_PRE(points.size() >= 2);

    if (points.size() == 2)
    {
        const auto segment = create_segment(state, &points[0], &points[1]);
        return {
            .leftmost = segment,
            .rightmost = segment->sym,
        };
    }
    if (points.size() == 3)
    {
        auto p1 = &points[0];
        auto p2 = &points[1];
        auto p3 = &points[2];

        const auto a = create_segment(state, p1, p2);
        const auto b = create_segment(state, p2, p3);
        splice(a->sym, b);
        const PointOrder orientation { *p1, *p2, *p3 };
        if (orientation.ccw())
        {
            connect(state, b->sym, a);
            return {
                .leftmost = a,
                .rightmost = b->sym,
            };
        }
        if (orientation.cw())
        {
            const auto c = connect(state, b->sym, a);
            return {
                .leftmost = c->sym,
                .rightmost = c,
            };
        }
        return {
            .leftmost = a,
            .rightmost = b->sym,
        };
    }
    const auto split = points.size() / 2;
    auto [lhs_left_edge, lhs_right_edge] = triangulate(state, points.first(split));
    auto [rhs_left_edge, rhs_right_edge] = triangulate(state, points.subspan(split));

    // Ищем базу с колторой начнем объединять две триангшуляции.
    // База должна сохранять выпуклость.
    while (true)
    {
        if (point_location(lhs_right_edge, rhs_left_edge->point).left())
        {
            lhs_right_edge = lhs_right_edge->sym->prev->sym;
        }
        else if (point_location(rhs_left_edge, lhs_right_edge->point).right())
        {
            rhs_left_edge = rhs_left_edge->next;
        }
        else
        {
            break;
        }
    }
    auto base = connect(state, lhs_right_edge, rhs_left_edge);
    if (lhs_left_edge == lhs_right_edge->sym)
    {
        lhs_left_edge = base;
    }
    if (rhs_right_edge == rhs_left_edge->sym)
    {
        rhs_right_edge = base->sym;
    }

    while (true)
    {
        auto base_left = base;
        auto base_right = base->sym;
        auto left_candidate = base->sym->next->next;
        auto right_candidate = base->sym->prev;
        const auto left_is_valid = point_location(base, left_candidate->point).left();
        const auto right_is_valid = point_location(base, right_candidate->point).left();
        if (!left_is_valid && !right_is_valid)
        {
            break;
        }
        const auto left_is_visible =
            left_is_valid && PointLocation(*base_left->point, *right_candidate->point, *left_candidate->point).left();
        if (left_is_visible)
        {
            base = connect(state, base_right, left_candidate);
            continue;
        }
        const auto right_is_visible =
            right_is_valid && PointLocation(*base_left->point, *right_candidate->point, *right_candidate->point).left();
        if (right_is_visible)
        {
            base = connect(state, base_right, left_candidate);
            continue;
        }
        AR_UNREACHABLE;
    }

    return {
        .leftmost = lhs_left_edge,
        .rightmost = rhs_right_edge,
    };
}

} // namespace ka::tilecut::triangulation
