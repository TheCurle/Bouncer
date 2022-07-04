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

    static Sphere glassSphere() {
        static Material glassMaterial;
        glassMaterial.transparency = 1;
        glassMaterial.refractiveIndex = 1.5;
        static Sphere glassSphere(glassMaterial);

        return glassSphere;
    }

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

struct Plane : public Geo {

    Plane() = default;

    Vector normalAt(const Point &p) override {
        (void) p;

        static const Vector normal = {0, 1, 0};
        return normal;
    }

    Intersections intersect(Ray &r) override {
        if (std::abs(r.direction.y) < 0.001) return {};

        double t = -r.origin.y / r.direction.y;
        return { { t, this } };
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

inline IntersectionDetail Intersection::fillDetail(const Intersection& i, Ray r, Intersections& isections) {
    Point hitPos = Ray::position(r, i.time);
    Vector hitNormal = i.object->normalAt(hitPos);
    Vector eyeDir = -r.direction;
    Vector reflectv = r.direction.reflect(hitNormal);
    Point bumpPoint = Point(hitPos + hitNormal * 0.001);
    Point underPoint = Point(hitPos - hitNormal * 0.001);

    double n1 = 1;
    double n2 = 1;

    std::vector<Geo*> containers;
    for (size_t idx = 0; idx < isections.size(); idx++) {
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

    bool inside = (hitNormal * eyeDir) < 0;
    if (inside) hitNormal = -hitNormal;

    return { i.time, *i.object, hitPos, bumpPoint, underPoint, eyeDir, hitNormal, reflectv, n1, n2, inside };
}

namespace Pattern {
    inline Color colorAt(const Point& point, Geo* object) {
        Point objectPoint = Point(Matrix::inverse(object->transform) * point);
        Point patternPoint = Point(Matrix::inverse(object->material.pattern->transform) * objectPoint);

        return object->material.pattern->at(patternPoint);
    }
}