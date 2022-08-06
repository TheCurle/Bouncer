/***************
 * THECURLE    *
 *     STENCIL *
 ***************/

#include <core/Tuple.hpp>
#include <core/Raster.hpp>
#include "RInterp.h"

#pragma once

namespace Raster {

    void DrawLine(Framebuffer& f, Point p0, Point p1, Color c) {
        float dx = p1.x - p0.x, dy = p1.y - p0.y;
        if (std::abs(dx) > std::abs(dy)) {
            // Horizontal line
            if (dx < 0) {
                auto temp = p1; p1 = p0; p0 = temp;
            }
            auto ys = lerp(p0.x, p0.y, p1.x, p1.y);
            for (float x = p0.x; x <= p1.x; ++x)
                f.set(x, ys[x - p0.x], c);
        } else {
            // Vertical line
            if (dy < 0) {
                auto temp = p1; p1 = p0; p0 = temp;
            }
            auto xs = lerp(p0.y, p0.x, p1.y, p1.x);
            for (float y = p0.y; y <= p1.y; ++y)
                f.set(xs[y - p0.y], y, c);
        }
    }

    void DrawWireframeTriangle(Framebuffer& f, Point p0, Point p1, Point p2, Color c) {
        DrawLine(f, p0, p1, c);
        DrawLine(f, p1, p2, c);
        DrawLine(f, p2, p0, c);
    }
}