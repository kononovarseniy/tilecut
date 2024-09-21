#pragma once

#include <vector>

#include <tilecut/uvec16.h>

namespace ka::tilecut::triangulation
{

struct Halfedge final
{
    uvec16 * point;
    Halfedge * sym;
    Halfedge * next;
    Halfedge * prev;
};

} // namespace ka::tilecut::triangulation
