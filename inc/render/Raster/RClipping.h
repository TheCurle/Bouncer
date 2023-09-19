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

    double getDistanceToPlane(const ClipPlane& p, const Point& pt) {
        return (pt.x * p.normal.x) + (pt.y * p.normal.y) + (pt.z + p.normal.z) + p.distance;
    }


    std::list<Tri> RenderableModel::clipTriangle(Tri t, std::vector<Point> v, const ClipPlane& p) {
        double d0 = getDistanceToPlane(p, v[t.p0]);
        double d1 = getDistanceToPlane(p, v[t.p1]);
        double d2 = getDistanceToPlane(p, v[t.p2]);

        bool flipped = false;
        Point p0 = v[t.p0], p1 = v[t.p1], p2 = v[t.p2];
        size_t t0 = t.p0, t1 = t.p1, t2 = t.p2;

        SWAP_IF_BIGGER(d0, d1, p0, p1, t0, t1)
        SWAP_IF_BIGGER(d0, d2, p0, p2, t0, t2)
        SWAP_IF_BIGGER(d1, d2, p1, p2, t1, t2)

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
                newTri = { idx, idx + 1, t2, t.c };
            else
                newTri = { idx + 1, idx, t2, t.c };

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
                new0 = { t2, t1, idx, t.c };
                new1 = { idx, t1, idx + 1, t.c };
            } else {
                new0 = { t1, t2, idx, t.c };
                new1 = { t1, idx, idx + 1, t.c };
            }

            return { new0, new1 };
        }
    }

    World& ClipWorld(World& w) {
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

            cWorld->addObjects({ model });
        }

        return *cWorld;
    }
}