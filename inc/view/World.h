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

namespace Light {
    Color at(World& w, RT::Ray r, int countdown = 10);
}

// A holder for objects and light data.
// Effectively the "render scene".
struct World {
    std::unique_ptr<Geo*[]> objects;
    size_t numObjs;
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
    void renderRT(const Camera& cam, Framebuffer& canvas, int fromX, int fromY, int toX, int toY) {
        auto startTime = std::chrono::system_clock::now();

        #pragma omp parallel for default(none) shared(canvas, cam, fromX, fromY, toX, toY)
        for (int y = fromY; y < toY; y++) {
            for (int x = fromX; x < toX; x++) {
                RT::Ray r = cam.rayForPixel(x, y);
                Color pix = Light::at(*this, r);
                canvas.set(x, y, pix);
            }
        }

        // Some performance detail.
        auto endTime = std::chrono::system_clock::now();
        std::cout << "RT Timing data:" << std::endl <<
                " Pixels rendered: " << cam.horizontalSize * cam.verticalSize << std::endl <<
                " Average time per pixel: " << std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime).count() / (cam.horizontalSize * cam.verticalSize) << "ns" << std::endl <<
                " Total render time: " << std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count() << "us (" << std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count() << "s)" << std::endl;
    }
};



namespace Light {
    // Using the Schlick approximation of the Fresnel effect.
    // The returned double is the ratio between reflection and refraction in the final ray.
    inline double fresnelContribution(RT::IntersectionDetail& detail) {
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

    // Generate and calculate the color of a reflection from the given point.
    inline Color reflected(World& w, RT::IntersectionDetail details, int countdown) {
        if (details.object.material.reflectivity == 0) return Color::black();
        if (countdown < 1) return Color::black();

        RT::Ray reflectRay { details.overPoint, details.reflectv };
        Color color = Light::at(w, reflectRay, countdown - 1);

        return color * details.object.material.reflectivity;
    }

    // Generate and calculate the color of a refraction from the given point.
    inline Color refracted(World& w, RT::IntersectionDetail details, int countdown) {
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

        RT::Ray refract { details.underPoint, dir };
        Color col = Light::at(w, refract, countdown - 1);

        return col * details.object.material.transparency;
    }

    // Trace a ray from the point to the nearest light source, and determine whether there is something blocking it.
    inline bool isInShadow(World& world, Point& point) {
        Vector v = world.lightSource.position - point;
        double distance = v.magnitude();
        Vector direction = Vector(v.normalize());

        RT::Ray r { point, direction };
        RT::Intersections sections = world.intersect(r);

        RT::Intersection hit = sections.hit();
        return (!hit.isEmpty() && hit.time < distance);
    }

    // Calculate the final color of an interection by summing up the Phong lighting, the reflections, and refractions of a point.
    inline Color shadeHit(World& world, RT::IntersectionDetail& hit, int countdown) {
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

    // Calculate the color at the intersection between the ray and the world.
    inline Color at(World& w, RT::Ray r, int countdown) {
        RT::Intersections isections = w.intersect(r);
        RT::Intersection hit = isections.hit();

        if (hit.time == -1 && hit.object == nullptr) return Color::black();

        RT::IntersectionDetail detail = RT::Intersection::fillDetail(hit, r, isections);

        return shadeHit(w, detail, countdown);
    }

}