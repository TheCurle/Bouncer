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
    };

    inline Vector normal(Tri t, std::vector<Point> v) {
        return ( Vector(v[t.p1] + (v[t.p0] * -1)).cross(Vector(v[t.p2] + (v[t.p0] * -1))) );
    }

    inline Vector normal(Point p0, Point p1, Point p2) {
        return (Vector(p1 + (p0 * -1)).cross(Vector(p2 + (p0 * -1))));
    }

    inline void DrawFilledTri(Framebuffer& f, Point p0, Point p1, Point p2, Color c, DepthBuffer& d, Camera& cam,
                              std::unique_ptr<Light::Light[]>& lights, size_t numLights) {
        std::vector<double> xLeft, xRight, zLeft, zRight;

        Vector triNorm = normal({ 0, 1, 2, { 0, 0, 0 } }, { p0, p1, p2 });

        if (p0.y > p1.y) { std::swap(p0.y, p1.y); std::swap(p0.x, p1.x); }
        if (p0.y > p2.y) { std::swap(p0.y, p2.y); std::swap(p0.x, p2.x); }
        if (p1.y > p2.y) { std::swap(p1.y, p2.y); std::swap(p1.x, p2.x); }

        auto x01 = lerp(p0.y, p0.x, p1.y, p1.x);
        auto z01 = lerp(p0.y, p0.z, p1.y, p1.z);
        auto x12 = lerp(p1.y, p1.x, p2.y, p2.x);
        auto z12 = lerp(p1.y, p1.z, p2.y, p2.z);
        auto x02 = lerp(p0.y, p0.x, p2.y, p2.x);
        auto z02 = lerp(p0.y, p0.z, p2.y, p2.z);

        std::remove(x01.begin(), x01.end(), x01.back());
        auto x012 = std::vector<double>(x02.size());
        x012.insert(std::begin(x012), std::begin(x01), std::end(x01));
        x012.insert(std::begin(x012) + x01.size(), std::begin(x12), std::end(x12));

        std::remove(z01.begin(), z01.end(), z01.back());
        auto z012 = std::vector<double>(z02.size());
        z012.insert(std::begin(z012), std::begin(z01), std::end(z01));
        z012.insert(std::begin(z012) + z01.size(), std::begin(z12), std::end(z12));

        auto m = x012.size() / 4;
        if (x02[m] < x012[m]) {
            xLeft = x02;
            xRight = x012;

            zLeft = z02;
            zRight = z012;
        } else {
            xRight = x02;
            xLeft = x012;

            zLeft = z012;
            zRight = z02;
        }

        for (size_t y = p0.y; y <= p2.y; ++y) {
            size_t leftX = (size_t) xLeft[y - p0.y], rightX = xRight[y - p0.y];
            auto zLine = lerp((double) leftX, zLeft[y - p0.y], (double) rightX, zRight[y - p0.y]);

            for (size_t x = leftX; x <= rightX; ++x) {
                double z = 1 / zLine[x - leftX];
                if (z - d.at(x, y) > 0.001) {
                    Color final = c *
                            Raster::lightAt(
                                Point(p0 + p1 + p2  * ((double) -1 / (double) 3)),
                                triNorm,
                                cam,
                                lights,
                                numLights
                    );

                    f.set(x, y, final);
                    d.set(x, y, z);
                }
            }
        }
    }

    inline void RenderTri(Framebuffer &f, Tri &t, std::vector<Point> verts, DepthBuffer &d, Camera &cam,
                          std::unique_ptr<Light::Light[]>& lights, size_t numLights) {
        Vector norm = Vector(normal(verts[t.p0], verts[t.p1], verts[t.p2]).normalize());
        Point v0 = verts[t.p0];
        Point camera = cam.getPosition();

        double discriminant = (v0 - camera) * norm;
        if (discriminant >= 0) {
            //std::cout << "tri " << t.p0 << "-" << t.p1 << "-" << t.p2 << " d" << discriminant << std::endl;
            return;
        }

        Raster::DrawFilledTri(f, verts[t.p0], verts[t.p1], verts[t.p2], t.c, d, cam, lights, numLights);
    }
}