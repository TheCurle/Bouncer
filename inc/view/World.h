/***************
 * THECURLE    *
 *     BOUNCER *
 ***************/

#include <catch2/catch_test_macros.hpp>
#include <utility>
#include <core/Matrix.h>
#include <render/Geometry.h>
#include <render/Ray.h>
#include <render/Light.h>
#include <view/Camera.h>

#pragma once

// A holder for objects and light data.
// Effectively the "render scene".
struct World {
    std::unique_ptr<Geo*[]> objects;
    size_t numObjs = 0;
    PointLight lightSource;

    World() : lightSource(PointLight({ 0, 0, 0 }, { 0, 0, 0 })) {}

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

    // Get a list of intersections that the given ray will have.
    // Iterates every object in the world and checks whether the ray will intersect.
    // TODO: this is a vicious hotspot, how can we remove the Vector from this?
    RT::Intersections intersect(RT::Ray& r) {
        std::vector<RT::Intersection> isects;
        isects.reserve(5);

        for (size_t i = 0; i < numObjs; i++) {
            objects[i]->intersect(r, isects);
        }

        return { isects.begin(), isects.end() };
    }

    // Render this world using Ray Tracing, onto the given canvas.
    void renderRT(const Camera& cam, Framebuffer& canvas, int fromX, int fromY, int toX, int toY, bool fast) {
        auto startTime = std::chrono::system_clock::now();

        #pragma omp target teams distribute parallel for default(none) shared(canvas, cam, fromX, fromY, toX, toY, fast)
        for (int y = fromY; y < toY; y++) {
            for (int x = fromX; x < toX; x++) {
                RT::Ray r = cam.rayForPixel(x, y);
                Color pix = Light::at(*this, r, 10, fast);
                canvas.set(x, y, pix);
            }
        }

        // Some performance detail.
        auto endTime = std::chrono::system_clock::now();
        std::cout << "RT Timing data:" << std::endl <<
                " Pixels rendered: " << cam.horizontalSize * cam.verticalSize << " (" << toX << "x" << toY << ")" << std::endl <<
                " Average time per pixel: " << std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime).count() / (cam.horizontalSize * cam.verticalSize) << "ns" << std::endl <<
                " Total render time: " << std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count() << "us (" << std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count() << "s)" << std::endl;
    }
};
