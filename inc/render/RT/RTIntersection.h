/***************
 * THECURLE    *
 *     BOUNCER *
 ***************/

#include <render/RT/RTWorld.h>

#pragma once

namespace RT {
    inline RT::IntersectionDetail fillDetail(const Intersection& i, Ray r, Intersections& isections) {
        Point hitPos = Ray::position(r, i.time);
        Vector hitNormal = i.object->normalAt(hitPos);
        Vector eyeDir = -r.direction;
        Vector reflectv = r.direction.reflect(hitNormal);

        bool inside = (hitNormal * eyeDir) < 0;
        if (inside) {
            hitNormal = -hitNormal;
        }

        Point bumpPoint = Point(hitPos + hitNormal * 0.001);
        Point underPoint = Point(hitPos - hitNormal * 0.001);

        double n1 = 1;
        double n2 = 1;

        std::vector<Geo*> containers;
        containers.reserve(isections.size);
        for (size_t idx = 0; idx < isections.size; idx++) {
            if (isections[idx] == i)
                if (!containers.empty())
                    n1 = containers.back()->material.refractiveIndex;

            auto pos = std::find(containers.begin(), containers.end(), isections[idx].object);
            if (pos != containers.end()) {
                containers.erase(pos);
            } else
                containers.emplace_back(isections[idx].object);

            if (isections[idx] == i) {
                if (!containers.empty())
                    n2 = containers.back()->material.refractiveIndex;
                break;
            }
        }


        return { i.time, *i.object, hitPos, bumpPoint, underPoint, eyeDir, hitNormal, reflectv, n1, n2, inside };
    }
}