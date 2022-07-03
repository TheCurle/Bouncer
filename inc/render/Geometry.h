/***************
 * THECURLE    *
 *     BOUNCER *
 ***************/

#include <catch2/catch_test_macros.hpp>
#include <utility>
#include <type/Matrix.h>
#include "Light.h"
#include "Ray.h"

#pragma once

struct Geo {
    int id;
    Matrix transform;
    Material material;

    Geo() {
        static int ids = 0;
        id = ids++;

        transform = Matrix::identity();
        material = Material();
    }

    explicit Geo(int nid) {
        id = nid;
        transform = Matrix::identity();
        material = Material();
    }

    virtual Vector normalAt(const Point& p) = 0;

    virtual Intersections intersect(Ray& r) = 0;
};

struct Sphere : public Geo {
    Sphere() = default;
    Sphere(Sphere&) = default;
    Sphere(Sphere&&) = default;

    explicit Sphere(Material mat) {
        material = mat;
    }

    explicit Sphere(Matrix trans) {
        transform = std::move(trans);
    }

    Intersections intersect(Ray& r) override {
        // Transform the ray according to the object's properties
        Ray r2 = Ray::transform(r, Matrix::inverse(transform));

        // First calculate the discriminant of the intersection;
        // we need to avoid entering an infinite loop if the ray doesn't intersect.

        Vector delta = r2.origin - Point(0, 0, 0);

        double a = r2.direction * r2.direction;
        double b = 2 * (r2.direction * delta);
        double c = (delta * delta) - 1;

        double discriminant = (b * b) - 4 * a * c;

        // If the discriminant is negative, there is no intersection

        if (discriminant < 0) {
            return {};
        }

        // If the discriminant is 0 or positive, there is at least one intersection.

        Intersection intersection1 = {(-b - std::sqrt(discriminant)) / (2 * a), this};
        Intersection intersection2 = {(-b + std::sqrt(discriminant)) / (2 * a), this};

        return {intersection1, intersection2};
    }

    Vector normalAt(const Point& p) override {
        Point oP = Point(Matrix::inverse(transform) * p);
        Vector oN = oP - Point(0, 0, 0);
        Vector wN = Vector(Matrix::transpose(Matrix::inverse(transform)) * oN);
        wN.w = 0;
        return Vector(wN.normalize());
    }
};

bool operator==(const Geo& x, const Geo& y);

#ifdef GEOMETRY_OPERATOR_OVERLOADS
bool operator==(const Geo& x, const Geo& y) {
    return x.transform == y.transform && x.material == y.material;
}
#endif


// Specializations to allow Catch to print these types

namespace Catch {
    template < >
    struct StringMaker<Geo> {
        static std::string convert(Geo const &p) {
            std::stringstream buf;
            buf << "Geo (" << p.id << ")";
            return buf.str();
        }
    };

    template < >
    struct StringMaker<Sphere> {
        static std::string convert(Sphere const &p) {
            std::stringstream buf;
            buf << "Sphere (" << p.id << ")";
            return buf.str();
        }
    };
}

inline IntersectionDetail Intersection::fillDetail(const Intersection& i, Ray r) {
    Point hitPos = Ray::position(r, i.time);
    Vector hitNormal = i.object->normalAt(hitPos);
    Vector eyeDir = -r.direction;

    Point bumpPoint = Point(hitPos + hitNormal * 0.001);

    bool inside = (hitNormal * eyeDir) < 0;
    if (inside) hitNormal = -hitNormal;

    return { i.time, *i.object, hitPos, bumpPoint, eyeDir, hitNormal, inside };
}
