/***************
 * THECURLE    *
 *     STENCIL *
 ***************/

#include <core/Tuple.hpp>
#include <core/Raster.hpp>
#include "RInterp.h"
#include "RLines.h"
#include <render/Raster/shading/RLight.h>
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

        static Tri sentinel() { // a value used in clipping to detect "no change"
            static Tri value = Tri(0, 0, 0, Color::white());
            return value;
        }

        bool operator==(const Tri& other) {
            return p0 == other.p0 && p1 == other.p1 && p2 == other.p2; // ignore color
        }
    };

    inline void DrawDepthTriangle(Framebuffer& f, DepthBuffer& d, Point& p0, Point& p1, Point& p2, Color c, double h0, double h1, double h2) {

        if (p1.y < p0.y) {
            std::swap(p1, p0);
            std::swap(h1, h0);
        }

        if (p2.y < p0.y) {
            std::swap(p2, p0);
            std::swap(h2, h0);
        }

        if (p2.y < p1.y) {
            std::swap(p2, p1);
            std::swap(h2, h1);
        }

        auto long_x = lerp(p0.y, p0.x, p2.y, p2.x);
        auto long_h = lerp(p0.y, h0, p2.y, h2);

        auto combined_x = lerp(p0.y, p0.x, p1.y, p1.x);
        auto combined_h = lerp(p0.y, h0, p1.y, h1);
        auto temp = lerp(p1.y, p1.x, p2.y, p2.x);
        combined_x.insert(combined_x.end(), temp.begin(), temp.end());
        temp = lerp(p1.y, h1, p2.y, h2);
        combined_h.insert(combined_h.end(), temp.begin(), temp.end());

        std::vector<double>* leftSideX;
        std::vector<double>* leftSideH;
        std::vector<double>* rightSideX;
        std::vector<double>* rightSideH;
        int idx = (int) (p1.y - p0.y);
        if (long_x[idx] < combined_x[idx]) {
            leftSideX = &long_x;
            leftSideH = &long_h;
            rightSideX = &combined_x;
            rightSideH = &combined_h;
        } else {
            rightSideX = &long_x;
            rightSideH = &long_h;
            leftSideX = &combined_x;
            leftSideH = &combined_h;
        }

        for (double y = p0.y; y <= p2.y; y++) {
            int yIdx = (int) (y - p0.y);

            auto hSeg = lerp((*leftSideX)[yIdx], (*leftSideH)[yIdx], (*rightSideX)[yIdx], (*rightSideH)[yIdx]);

            for (int x = (*leftSideX)[yIdx]; x <= (*rightSideX)[yIdx]; x++) {
                double z = hSeg[x - int((*leftSideX)[yIdx])];
                if (z > d.at(x, y)) {
                    f.set(x, y, c);
                    d.set(x, y, z);
                }
            }
        }
    }


    inline void RenderTri(Framebuffer &f, DepthBuffer &d, Tri &t, std::vector<Point> verts, std::vector<Point> cameraSpacePoints) {

        double d0 = 1 / (cameraSpacePoints[t.p0].z);
        double d1 = 1 / (cameraSpacePoints[t.p1].z);
        double d2 = 1 / (cameraSpacePoints[t.p2].z);

        Raster::DrawDepthTriangle(f, d, verts[t.p0], verts[t.p1], verts[t.p2], t.c, d0, d1, d2);
    }
}