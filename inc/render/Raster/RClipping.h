/***************
 * THECURLE    *
 *     STENCIL *
 ***************/

#pragma once

#include "RModel.h"

#define SWAP_IF_BIGGER(p0, p1, d0, d1, t0, t1) \
    if ( (p0) > (p1) ) {       \
       std::swap(p0, p1);      \
       std::swap(d0, d1);      \
       std::swap(t0, t1);      \
       flipped = !flipped;     \
    }


namespace Raster {
    struct ClipPlane {
        Vector normal;
        float distance;

        Point intersect(const Point& pA, const Point& pB) const {
            Vector diff = (pA - pB);
            double t = (-distance - (normal.x * pA.x + normal.y * pA.y + normal.z * pA.z)) / (normal.x * diff.x + normal.y * diff.y + normal.z * diff.z);
            return pA + ( diff * t );
        }
    };

    double getDistanceToPlane(const ClipPlane& p, const Point& pt) {
        return (pt.x * p.normal.x) + (pt.y * p.normal.y) + (pt.z + p.normal.z) + p.distance;
    }


    std::list<Tri> ClipTriangle(Tri t, std::vector<Point> v, const ClipPlane& p) {
        double d0 = getDistanceToPlane(p, v[t.p0]);
        double d1 = getDistanceToPlane(p, v[t.p1]);
        double d2 = getDistanceToPlane(p, v[t.p2]);

        bool flipped = false;
        Point p0 = v[t.p0], p1 = v[t.p1], p2 = v[t.p2];

        SWAP_IF_BIGGER(d0, d1, p0, p1, t.p0, t.p1)
        SWAP_IF_BIGGER(d0, d2, p0, p2, t.p0, t.p2)
        SWAP_IF_BIGGER(d1, d2, p1, p2, t.p1, t.p2)

        if (d0 > 0) {
            return { t };
        } else if (d2 < 0) {
            return {};
        } else if (d1 < 0) {
            Point pB = p.intersect(p2, p1);
            Point pC = p.intersect(p2, p0);
            size_t idx = v.size();
            v.push_back(pB); v.push_back(pC);

            Tri newTri;
            if (flipped)
                newTri = { idx, idx + 1, t.p2, t.c };
            else
                newTri = { idx + 1, idx, t.p2, t.c };

            return { newTri };
        } else {
            Point* pC = &p0;
            Point* pB = &p1;
            Point* pA = &p2;
            Point pAp = p.intersect(*pA, *pC);
            Point pBp = p.intersect(*pB, *pC);
            size_t idx = v.size();
            v.push_back(pAp); v.push_back(pBp);

            Tri new0, new1;
            if (flipped) {
                new0 = {t.p2, t.p1, idx, t.c };
                new1 = { idx, t.p1, idx + 1, t.c };
            } else {
                new0 = { t.p1, t.p2, idx, t.c };
                new1 = { t.p1, idx, idx + 1, t.c };
            }

            return { new0, new1 };
        }
    }

    World& ClipWorld(World& w, Camera& c, std::array<ClipPlane, 5> planes) {
        auto* cWorld = new World {
                {},
                {}
        };
        cWorld->numObjs = 0;
        cWorld->objects = {};
        cWorld->numLights = w.numLights;
        cWorld->lightSources = std::move(w.lightSources);

        for (size_t i = 0; i < w.numObjs; ++i) {
            auto* model = dynamic_cast<Model*>(w.objects[i]);

            model->boundingCenter = Point(c.inverseTransform * model->inverseTransform * model->boundingCenter);
            for (auto& plane : planes) {
                double distance = plane.normal * Vector(model->boundingCenter);
                if (distance < -model->boundingRadius) {
                    std::cout << "Model " + std::to_string(model->id) + " being discarded because it is " + std::to_string(distance) + " units away from a clipping plane. Limit: " + std::to_string(-model->boundingRadius) << std::endl;
                    goto nextObject; // Early discard
                }
            }

            for (auto& vert : model->mesh.vertices)
                vert = Point(c.inverseTransform * model->inverseTransform * vert);

            for (auto& plane : planes) {
                std::vector<Tri> newTris;
                for (std::vector<Tri>::iterator iter = model->mesh.tris.begin(); iter != model->mesh.tris.end(); iter++) {
                    auto clipped = ClipTriangle(*iter, model->mesh.vertices, plane);
                    if (clipped.size() > 0)
                        newTris.insert(newTris.end(), clipped.begin(), clipped.end());
                    else {
                        model->mesh.tris.erase(iter);
                        iter--;
                    }
                }
                model->mesh.tris = newTris;
            }

            cWorld->addObjects({ model });

            nextObject:;
        }

        return *cWorld;
    }
}