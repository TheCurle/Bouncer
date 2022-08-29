/***************
 * THECURLE    *
 *     BOUNCER *
 ***************/

#include <catch2/catch_test_macros.hpp>
#include <utility>
#include <core/Matrix.h>
#include <render/Geometry.h>
#include "render/RT/Ray.h"
#include <render/Light.h>
#include <view/Camera.h>

#pragma once


// A holder for objects and light data.
// Effectively the "render scene".
struct World {
    std::unique_ptr<Geo*[]> objects;
    size_t numObjs;
    std::unique_ptr<Light::Light[]> lightSources;
    size_t numLights;

    World() : objects(nullptr), numObjs(0), lightSources(nullptr) {}

    World(std::initializer_list<Geo*> geo, const std::initializer_list<Light::Light>& lights) : objects(std::make_unique<Geo*[]>(geo.size())), numObjs(geo.size()), lightSources(std::make_unique<Light::Light[]>(lights.size())), numLights(lights.size()) {
        std::copy(geo.begin(), geo.end(), objects.get());
        std::copy(lights.begin(), lights.end(), lightSources.get());
    }

    void addObjects(std::initializer_list<Geo*> init) {
        auto geos = std::make_unique<Geo*[]>(numObjs + init.size());
        std::copy(objects.get(), objects.get() + numObjs, geos.get());
        std::copy(init.begin(), init.end(), geos.get() + numObjs);
        numObjs += init.size();
        objects.release();
        objects = std::move(geos);
    }

    static World defaultWorld() {
        return World(
                {
                    new Sphere(Light::Material({ 0.8, 1.0, 0.6 }, 0.1, 0.7, 0.2, 200, 0, 0, 1)),
                    new Sphere(Matrix::scaling(0.5, 0.5, 0.5))
                },

                {
                    { Light::Type::POINT, { -10, 10, -10 }, { 1, 1, 1 } },
                    { Light::Type::AMBIENT, { 0, 0, 0 }, { 1, 1, 1 } }
                }
        );
    }
};
