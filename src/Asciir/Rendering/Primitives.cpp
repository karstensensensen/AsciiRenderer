#include "Primitives.h"
#include "Asciir/Maths/Maths.h"

namespace Asciir
{
    Coord Quad::centrePoint(Transform& transform) const
    {
        // simply get the centre of the quad as if it was not transformed, and then reverse the transformation to the resulting point.
        return transform.applyTransform(offset + (size) / 2);
    }

    bool Quad::isInside(Coord coord, Transform& transform) const
    {
        // first, reverse the transformation on coord.
        // this makes sure we do not have to transform the quad itself, making the calculations more expensive.

        coord = transform.reverseTransform(coord) - offset;

        return    0 <= coord.x && coord.x <= size.x
               && 0 <= coord.y && coord.y <= size.y;
    }
    
    bool Quad::isInsideGrid(Coord coord, Transform& transform, Real resolution) const
    {
        // simply create a new quad which is fitted to a grid corresponding to the resolution

        coord = transform.reverseTransformGrid(coord, resolution) - offset;
        coord.x = round(coord.x, resolution) + (Real)0.5;
        coord.y = round(coord.y, resolution) + (Real)0.5;

        return    0 <= coord.x && coord.x <= size.x
               && 0 <= coord.y && coord.y <= size.y;
    }
}
