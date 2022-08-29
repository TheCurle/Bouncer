/***************
 * THECURLE    *
 *     STENCIL *
 ***************/

#pragma once

#include "RModel.h"

namespace Raster {
    struct ClipPlane {
        Vector normal;
        float distance;
    };

    float getDistanceToPlane(ClipPlane p, Point pt) {
        return (pt.x * p.normal.x) + (pt.y * p.normal.y) + (pt.z + p.normal.z) + p.distance;
    }

    std::list<Tri> ClipTriangle(Tri t, std::vector<Point> v, ClipPlane p) {
        float d0 = getDistanceToPlane(p, v[t.p0]);
        float d1 = getDistanceToPlane(p, v[t.p1]);
        float d2 = getDistanceToPlane(p, v[t.p2]);

        if (d0 > 0 && d1 > 0 && d2 > 0)
            return { t };
        else if (d0 < 0 && d1 < 0 && d2 < 0)
            return { };
        else if ((d0 > 0) + (d1 > 0) + (d2 > 0) == 1)
            return { }; // todo; one positive point, clip two
        else
            return { }; // todo; one negative point, clip two

    }

    World& ClipWorld(World& w, Camera& c, std::array<ClipPlane, 5> planes) {
        World* cWorld = new World {
                {},
                {}
        };
        cWorld->numObjs = 0;
        cWorld->objects = {};
        cWorld->numLights = w.numLights;
        cWorld->lightSources = std::move(w.lightSources);

        for (int i = 0; i < w.numObjs; ++i) {
            Raster::Model* model = static_cast<Model*>(w.objects[i]);

            model->boundingCenter = Point(c.inverseTransform * model->inverseTransform * model->boundingCenter);
            for (auto& plane : planes) {
                float distance = plane.normal * Vector(model->boundingCenter);
                if (distance < -model->boundingRadius) {
                    std::cout << "Model " + std::to_string(model->id) + " being discarded because it is " + std::to_string(distance) + " units away from a clipping plane. Limit: " + std::to_string(-model->boundingRadius) << std::endl;
                    goto nextObject; // Early discard
                }
            }

            for (auto& vert : model->mesh.vertices)
                vert = Point(c.inverseTransform * model->inverseTransform * vert);

            for (auto& plane : planes) {
                std::vector<Tri> newTris;
                for (auto& tri : model->mesh.tris) {
                    auto clipped = ClipTriangle(tri, model->mesh.vertices, plane);
                    newTris.insert(newTris.end(), clipped.begin(), clipped.end());
                }
                model->mesh.tris = newTris;
            }

            cWorld->addObjects({ model });

            nextObject:;
        }

        return *cWorld;
    }
}