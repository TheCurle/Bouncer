/***************
 * THECURLE    *
 *     STENCIL *
 ***************/

#include <core/Tuple.hpp>
#include <core/Raster.hpp>
#include "RInterp.h"
#include "RLines.h"
#include <iostream>

#pragma once

namespace Raster {
    struct Tri {
        Tri() : c({ 0, 0, 0 }) {}
        Tri(size_t o, size_t t, size_t th, Color co) : p0(o), p1(t), p2(th), c(co) {}

        size_t p0;
        size_t p1;
        size_t p2;
        Color c;
    };

    inline void DrawFilledTri(Framebuffer& f, Point p0, Point p1, Point p2, Color c) {
        std::vector<float> xLeft, xRight;

        if (p0.y > p1.y) { std::swap(p0.y, p1.y); std::swap(p0.x, p1.x); }
        if (p0.y > p2.y) { std::swap(p0.y, p2.y); std::swap(p0.x, p2.x); }
        if (p1.y > p2.y) { std::swap(p1.y, p2.y); std::swap(p1.x, p2.x); }

        auto x01 = lerp(p0.y, p0.x, p1.y, p1.x);
        auto x12 = lerp(p1.y, p1.x, p2.y, p2.x);
        auto x02 = lerp(p0.y, p0.x, p2.y, p2.x);

        std::remove(x01.begin(), x01.end(), x01.back());
        auto x012 = std::vector<float>(x02.size());
        x012.insert(std::begin(x012), std::begin(x01), std::end(x01));
        x012.insert(std::begin(x012) + x01.size(), std::begin(x12), std::end(x12));

        auto m = x012.size() / 4;
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

    inline void DrawShadedTriangle(Framebuffer& f, Point p0, Point p1, Point p2, Color c) {
        std::vector<float> xLeft, xRight, hLeft, hRight;
        float h0, h1, h2;

        if (p0.y > p1.y) { std::swap(p0.y, p1.y); std::swap(p0.x, p1.x); }
        if (p0.y > p2.y) { std::swap(p0.y, p2.y); std::swap(p0.x, p2.x); }
        if (p1.y > p2.y) { std::swap(p1.y, p2.y); std::swap(p1.x, p2.x); }

        auto x01 = lerp(p0.y, p0.x, p1.y, p1.x);
        auto h01 = lerp(p0.y, h0, p1.y, h1);

        auto x12 = lerp(p1.y, p1.x, p2.y, p2.x);
        auto h12 = lerp(p1.y, h1, p2.y, h2);

        auto x02 = lerp(p0.y, p0.x, p2.y, p2.x);
        auto h02 = lerp(p0.y, h0, p2.y, h2);

        std::remove(x01.begin(), x01.end(), x01.back());
        auto x012 = std::vector<float>(x02.size());
        x012.insert(std::begin(x012), std::begin(x01), std::end(x01));
        x012.insert(std::begin(x012) + x01.size(), std::begin(x12), std::end(x12));


        std::remove(h01.begin(), h01.end(), h01.back());
        auto h012 = std::vector<float>(h02.size());
        h012.insert(std::begin(h012), std::begin(h01), std::end(h01));
        h012.insert(std::begin(h012) + h01.size(), std::begin(h12), std::end(h12));

        auto m = x012.size() / 2;
        if (x02[m] < x012[m]) {
            xLeft = x02;
            xRight = x012;

            hLeft = h02;
            hRight = h012;
        } else {
            xRight = x02;
            xLeft = x012;

            hLeft = h012;
            hRight = h02;
        }

        for (size_t y = p0.y; y <= p2.y; ++y) {
            size_t leftX = xLeft[y - p0.y], rightX = xRight[y - p0.y];
            auto hLine = lerp(leftX, hLeft[y - p0.y], rightX, hRight[y - p0.y]);

            for (size_t x = leftX; x <= rightX; ++x) {
                Color cShaded = c * hLine[x - leftX];
                f.set(x, y, cShaded);
            }
        }
    }

    inline void RenderTri(Framebuffer& f, Raster::Tri& t, std::vector<Point> verts) {
        Raster::DrawFilledTri(f, verts[t.p0], verts[t.p1], verts[t.p2], t.c);
    }
}