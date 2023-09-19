/***************
 * THECURLE    *
 *     STENCIL *
 ***************/

#include <render/Geometry.h>
#include "RTriangles.h"
#include "RModel.h"

#pragma once


namespace Raster {

    inline Point projectPoint(const Point& p, Camera& c) {
        return { c.horizontalSize / 2 + (p.x * c.horizontalSize / 1), c.verticalSize / 2 + (p.y * c.verticalSize / 1), p.z };
    }

    inline Point projectVertex(const Point& v, Camera& c) {
        return projectPoint({ v.x * 1 / v.z, v.y * 1 / v.z, v.z }, c);
    }

    inline void clipPlane(RenderableModel* m, ClipPlane& p) {
        double distance = getDistanceToPlane(p, m->boundingCenter);
        if (distance < -m->boundingRadius) {
            std::cout << "Model " + std::to_string(m->id) + " being discarded because it is " + std::to_string(distance) + " units away from a clipping plane. Limit: " + std::to_string(-m->boundingRadius) << std::endl;
            m->reject(); // Early discard
        } else {
            m->clipAgainstPlane(p);
        }
    }

    inline void clipPlanes(RenderableModel* m, std::array<ClipPlane, 5> planes) {
        for (size_t i = 0; i < planes.size() && !m->rejected; i++)
            clipPlane(m, planes[i]);
    }

    inline void
    RenderModel(Framebuffer &f, RenderableModel* model, Camera &c, DepthBuffer &d, std::unique_ptr<Light::Light[]>& lights, size_t numLights) {
        std::vector<Point> projected;
        for (auto& p : model->translatedVertices) {
            projected.emplace_back(projectVertex(p, c));
        }

        //model->cull();

        for (auto& t : model->clippedTris) {
            RenderTri(f, t, projected, d, c, lights, numLights);
        }
    }

    inline void Render(World& w, Framebuffer& f, Camera& c, DepthBuffer& d) {
        double s2 = std::sqrt(2) / 2;
        World& cW = ClipWorld(w);

        std::array<ClipPlane, 5> planes {{
            {{0, 0, 1}, -1},
            {{s2, 0, s2}, 0},
            {{-s2, 0, s2}, 0},
            {{0, -s2, s2}, 0},
            {{0, s2, s2}, 0}
        }};

        Matrix& invCamera = c.inverseTransform;

        for (size_t i = 0; i < w.numObjs; i++) {
            RenderableModel* clippedModel = new RenderableModel(*dynamic_cast<Model*>(cW.objects[i]));
            clippedModel->generateWorldSpace();
            clippedModel->applyTransform(invCamera);

            clipPlanes(clippedModel, planes);

            if (!clippedModel->rejected)
                RenderModel(f, clippedModel, c, d, cW.lightSources, cW.numLights);
        }
    }
}