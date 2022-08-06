/***************
 * THECURLE    *
 *     STENCIL *
 ***************/

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

        Model(Mesh m, Matrix t) : Geo(), mesh(m) {
            setMatrix(t);
        }

        Vector normalAt(const Point& p) override { return { 0, 0, 0 }; }
    };

    inline Point projectPoint(Point p, Camera& c) {
        return { c.horizontalSize / 2 + (p.x * c.horizontalSize / 1), c.verticalSize / 2 + (p.y * c.verticalSize / 1), 0 };
    }

    inline Point projectVertex(Point v, Camera& c) {
        return projectPoint({ v.x * 1 / v.z, v.y * 1 / v.z, 0 }, c);
    }


    inline void RenderModel(Framebuffer& f, Raster::Model model, Camera& c) {
        std::vector<Point> projected;
        for (auto& p : model.mesh.vertices) {
            projected.emplace_back(projectVertex(Point((c.transform * model.transform) * p), c));
        }

        for (auto& t : model.mesh.tris) {
            RenderTri(f, t, projected);
        }
    }

    inline void Render(World& w, Framebuffer& f, Camera& c) {
        for (size_t i = 0; i < w.numObjs; i++)
            RenderModel(f, *static_cast<Model*>(w.objects[i]), c);
    }
}