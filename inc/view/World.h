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
    Color at(World& w, Ray r, int countdown = 10);
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
                    new Sphere(Material({ 0.8, 1.0, 0.6 }, 0.1, 0.7, 0.2, 200, 0, 0, 1)),
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
        std::vector<Intersection> isects;
        isects.reserve(5);

        for (Geo*& object : objects) {
            object->intersect(r, isects);
        }

        return { isects.begin(), isects.end() };
    }

    void render(const Camera& cam, Framebuffer& canvas) {
        auto startTime = std::chrono::system_clock::now();

        #pragma omp parallel for default(none) shared(canvas, cam)
        for (int y = 0; y < cam.verticalSize -1; y++) {
            for (int x = 0; x < cam.horizontalSize -1; x++) {
                Ray r = cam.rayForPixel(x, y);
                Color pix = Light::at(*this, r);
                canvas.set(x, y, pix);
            }
        }

        auto endTime = std::chrono::system_clock::now();
        std::cout << "Timing data:" << std::endl <<
                " Pixels rendered: " << cam.horizontalSize * cam.verticalSize << std::endl <<
                " Average time per pixel: " << std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime).count() / (cam.horizontalSize * cam.verticalSize) << "ns" << std::endl <<
                " Total render time: " << std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count() << "us (" << std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count() << "s)" << std::endl;
    }
};


namespace Light {
    inline double fresnelContribution(IntersectionDetail& detail) {
        double cos = detail.eyev * detail.normalv;

        if (detail.refractiveIdxIncoming > detail.refractiveIdxOutgoing) {
            double ratio = detail.refractiveIdxIncoming / detail.refractiveIdxOutgoing;
            double sin2t = ratio * ratio * ( 1 - cos * cos);
            if (sin2t > 1) return 1;

            double cost = std::sqrt(1 - sin2t);
            cos = cost;
        }

        double r0 = std::pow((detail.refractiveIdxIncoming - detail.refractiveIdxOutgoing) / (detail.refractiveIdxIncoming + detail.refractiveIdxOutgoing), 2);
        return std::pow(r0 + (1 - r0) * (1-cos), 5);

    }

    inline Color reflected(World w, IntersectionDetail details, int countdown) {
        if (details.object.material.reflectivity == 0) return Color::black();
        if (countdown < 1) return Color::black();

        Ray reflectRay { details.overPoint, details.reflectv };
        Color color = Light::at(w, reflectRay, countdown - 1);

        return color * details.object.material.reflectivity;
    }

    inline Color refracted(World w, IntersectionDetail details, int countdown) {
        if (details.object.material.transparency == 0) return Color::black();
        if (countdown < 1) return Color::black();

        // Check for Total Internal Reflection; derived from Snell's Law
        double ratio = details.refractiveIdxIncoming / details.refractiveIdxOutgoing;
        double cosi = details.eyev * details.normalv;
        double sin2t = (ratio*ratio) * (1 - (cosi*cosi));

        if(sin2t > 1)
            return Color::black();

        // Cast the refracted ray
        double cost = std::sqrt((double) 1.0 - sin2t);
        Vector dir = details.normalv * (ratio * cosi - cost) - details.eyev * ratio;

        Ray refract { details.underPoint, dir };
        Color col = Light::at(w, refract, countdown - 1);

        return col * details.object.material.transparency;
    }

    inline bool isInShadow(World& world, Point& point) {
        Vector v = world.lightSource.position - point;
        double distance = v.magnitude();
        Vector direction = Vector(v.normalize());

        Ray r { point, direction };
        Intersections sections = world.intersect(r);

        Intersection hit = sections.hit();
        return (!hit.isEmpty() && hit.time < distance);
    }

    inline Color shadeHit(World& world, IntersectionDetail& hit, int countdown) {
        Color rayTarget = lighting(hit.object.material, &hit.object, world.lightSource, hit.overPoint, hit.eyev, hit.normalv, Light::isInShadow(world, hit.overPoint));
        Color reflectedRay = Light::reflected(world, hit, countdown);
        Color refractedRay = Light::refracted(world, hit, countdown);

        Material mat = hit.object.material;
        if (mat.reflectivity > 0 && mat.transparency > 0) {
            double reflectance = fresnelContribution(hit);
            return rayTarget + (reflectedRay * reflectance) + (refractedRay * (1 - reflectance));
        } else {
            return refractedRay + rayTarget + reflectedRay;
        }

    }

    inline Color at(World& w, Ray r, int countdown) {
        Intersections isections = w.intersect(r);
        Intersection hit = isections.hit();

        if (hit.time == -1 && hit.object == nullptr) return Color::black();

        IntersectionDetail detail = Intersection::fillDetail(hit, r, isections);

        return shadeHit(w, detail, countdown);
    }

}