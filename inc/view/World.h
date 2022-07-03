/***************
 * THECURLE    *
 *     BOUNCER *
 ***************/

#include <catch2/catch_test_macros.hpp>
#include <utility>
#include <type/Matrix.h>
#include <render/Geometry.h>
#include <render/Ray.h>
#include "render/Light.h"
#include "Camera.h"

#pragma once

namespace Light {
    Color shadeHit(const World &world, const IntersectionDetail &hit);
    Color at(World w, Ray r);
}

// A holder for objects and light data.
// Effectively the "render scene".
struct World {
    std::vector<Geo*> objects;
    PointLight lightSource;

    World() : lightSource(PointLight({ 0, 0, 0 }, { 0, 0, 0 })) {}

    World(std::vector<Geo*> geo, const PointLight& light) : objects(std::move(geo)), lightSource(light) {}

    static World defaultWorld() {
        return World(
                {
                    new Sphere(Material({ 0.8, 1.0, 0.6 }, 0.1, 0.7, 0.2, 200)),
                    new Sphere(Matrix::scaling(0.5, 0.5, 0.5))
                },

                PointLight({ -10, 10, -10 }, { 1, 1, 1 } )
        );
    }

    static Matrix viewMatrix(const Point& start, const Point& end, Vector up) {
        Vector forward = end - start;
        forward = Vector(forward.normalize());

        Vector left = forward.cross(Vector(up.normalize()));
        Vector trueUp = left.cross(forward);

        Matrix orientation {
                {
                    { left.x, left.y, left.z, 0 },
                    { trueUp.x, trueUp.y, trueUp.z, 0 },
                    { -forward.x, -forward.y, -forward.z, 0 },
                    { 0, 0, 0, 1 }
                }
        };

        return orientation * Matrix::translation(-start.x, -start.y, -start.z);
    }

    Intersections intersect(Ray& r) {
        Intersections xs;

        for (Geo*& object : objects) {
            Intersections intersect = object->intersect(r);
            xs.addAllHits(intersect);
        }

        return xs.sort();
    }

    void render(const Camera& cam, Framebuffer& canvas) {
        auto startTime = std::chrono::system_clock::now();

        #pragma omp parallel for default(none) shared(canvas, cam)
        for (int y = 0; y < cam.verticalSize -1; y++) {
            for (int x = 0; x < cam.horizontalSize -1; x++) {
                Ray r = cam.rayForPixel(x, y);
                canvas.set(x, y, Light::at(*this, r));
            }
        }

        auto endTime = std::chrono::system_clock::now();
        std::cout << "Timing data:" << std::endl <<
                " Pixels rendered: " << cam.horizontalSize * cam.verticalSize << std::endl <<
                " Average time per pixel: " << std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count() / (cam.horizontalSize * cam.verticalSize) << "us" << std::endl <<
                " Total render time: " << std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count() << "us (" << std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count() << "s)" << std::endl;
    }
};


namespace Light {
    inline bool isInShadow(World& world, Point& point) {
        Vector v = world.lightSource.position - point;
        double distance = v.magnitude();
        Vector direction = Vector(v.normalize());

        Ray r { point, direction };
        Intersections sections = world.intersect(r);

        Intersection hit = sections.hit();
        return (!hit.isEmpty() && hit.time < distance);
    }

    inline Color shadeHit(World& world, IntersectionDetail& hit) {
        return lighting(hit.object.material, &hit.object, world.lightSource, hit.overPoint, hit.eyev, hit.normalv, Light::isInShadow(world, hit.overPoint));
    }

    inline Color at(World w, Ray r) {
        Intersections isections = w.intersect(r);
        Intersection hit = isections.hit();

        if (hit.time == 0 && hit.object == nullptr) return { 0, 0, 0 };

        IntersectionDetail detail = Intersection::fillDetail(hit, r);

        return shadeHit(w, detail);
    }

}