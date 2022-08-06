/***************
 * THECURLE    *
 *     STENCIL *
 ***************/

#include <vector>
#include <core/Tuple.hpp>
#include <render/Geometry.h>
#include "RTriangles.h"

#pragma once

namespace Raster {

    struct Mesh {
        std::vector<Point> vertices;
        std::vector<Tri> tris;
    };

    struct Model : public Geo {
        Mesh mesh;

        Point boundingCenter;
        float boundingRadius;

        Model(Mesh m, Matrix t) : Geo(), mesh(m), boundingCenter({ 0, 0, 0 }), boundingRadius(std::sqrt(3)) {
            setMatrix(t);
        }

        Vector normalAt(const Point& p) override { return { 0, 0, 0 }; }
    };
}