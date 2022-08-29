/***************
 * THECURLE    *
 *     STENCIL *
 ***************/

#include <core/Tuple.hpp>
#include <view/Camera.h>
#include <render/Light.h>
#pragma once

namespace Raster {

        inline Color lightAt(Point vertex, Vector normal, Camera& cam, std::unique_ptr<Light::Light[]>& lights, size_t numLights) {
            Color final(0, 0, 0);

            for (size_t i = 0; i < numLights; i++) {
                auto light = lights[i];

                if (light.type == Light::Type::AMBIENT) {
                    final += light.intensity;
                    continue;
                }

                Vector v(0, 0, 0);
                if (light.type == Light::Type::POINT) {
                    v = Vector(Vector((cam.inverseTransform * light.position)).normalize());
                }

                double cosA = v * normal / (v.magnitude() * normal.magnitude());
                if (cosA > 0)
                    final += light.intensity * cosA;

                Vector reflected = Vector((normal + (v * -1)) * ((v * normal) * 2));
                Vector view = Vector(cam.inverseTransform * (vertex * -1));

                double cosB = reflected * view / (reflected.magnitude() * view.magnitude());
                if (cosB > 0)
                    final += light.intensity * std::pow(cosB, 50);
            }

            return final;
        }

}