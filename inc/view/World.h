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
    PointLight lightSource;

    World() : objects(nullptr), numObjs(0), lightSource(PointLight({ 0, 0, 0 }, { 0, 0, 0 })) {}

    World(std::initializer_list<Geo*> geo, const PointLight& light) : objects(std::make_unique<Geo*[]>(geo.size())), numObjs(geo.size()), lightSource(light) {
        std::copy(geo.begin(), geo.end(), objects.get());
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
                    new Sphere(Material({ 0.8, 1.0, 0.6 }, 0.1, 0.7, 0.2, 200, 0, 0, 1)),
                    new Sphere(Matrix::scaling(0.5, 0.5, 0.5))
                },

                PointLight({ -10, 10, -10 }, { 1, 1, 1 } )
        );
    }
};
