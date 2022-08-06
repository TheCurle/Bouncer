/***************
 * THECURLE    *
 *     STENCIL *
 ***************/

#include <core/Tuple.hpp>
#include <core/Raster.hpp>
#include "RInterp.h"
#include <iostream>

#pragma once

namespace Raster {
    inline void DrawFilledTri(Framebuffer& f, Point p0, Point p1, Point p2, Color c) {
        std::vector<float> xLeft, xRight;

        if (p0.y > p1.y) { std::swap(p0.y, p1.y); std::swap(p0.x, p1.x); }
        if (p0.y > p2.y) { std::swap(p0.y, p2.y); std::swap(p0.x, p2.x); }
        if (p1.y > p2.y) { std::swap(p1.y, p2.y); std::swap(p1.x, p2.x); }

        auto x01 = lerp(p0.y, p0.x, p1.y, p1.x);
        auto x12 = lerp(p1.y, p1.x, p2.y, p2.x);
        auto x02 = lerp(p0.y, p0.x, p2.y, p2.x);

        std::remove(x01.begin(), x01.end(), x01.back());
        auto x012 = std::vector<float>(x01.size() + x12.size());
        x012.insert(std::begin(x012), std::begin(x01), std::end(x01));
        x012.insert(std::begin(x012) + x01.size(), std::begin(x12), std::end(x12));

        auto m = std::floor(x012.size() / 2);
        if (x02[m] < x012[m]) {
            xLeft = x02;
            xRight = x012;
        } else {
            xRight = x02;
            xLeft = x012;
        }

        for (size_t y = p0.y; y <= p2.y; ++y) {
            size_t leftX = xLeft[y - p0.y], rightX = xRight[y - p0.y];
            for (size_t x = leftX; x <= rightX; ++x) {
                f.set(x, y, c);
            }
        }
    }
}