/***************
 * THECURLE    *
 *     STENCIL *
 ***************/

#include <render/Geometry.h>
#include "RTriangles.h"
#include "RModel.h"

#pragma once


namespace Raster {

    inline Point projectPoint(Point p, Camera& c) {
        return { c.horizontalSize / 2 + (p.x * c.horizontalSize / 1), c.verticalSize / 2 + (p.y * c.verticalSize / 1), 0 };
    }

    inline Point projectVertex(Point v, Camera& c) {
        return projectPoint({ v.x * 1 / v.z, v.y * 1 / v.z, 0 }, c);
    }


    inline void RenderModel(Framebuffer& f, Raster::Model model, Camera& c) {
        std::vector<Point> projected;
        for (auto& p : model.mesh.vertices) {
            projected.emplace_back(projectVertex(p, c));
        }

        for (auto& t : model.mesh.tris) {
            RenderTri(f, t, projected);
        }
    }

    inline void Render(World& w, Framebuffer& f, Camera& c) {
        float s2 = std::sqrt(2) / 2;
        World& cW = ClipWorld(w, c, std::array<ClipPlane, 5> {{
            {{0, 0, 1}, -1},
            {{s2, 0, s2}, 0},
            {{-s2, 0, s2}, 0},
            {{0, -s2, s2}, 0},
            {{0, s2, s2}, 0}
        }});

        for (size_t i = 0; i < w.numObjs; i++)
            RenderModel(f, *static_cast<Model*>(cW.objects[i]), c);
    }
}