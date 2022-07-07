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
    Matrix inverseTransform;
    Material material;

    Geo() {
        static int ids = 0;
        id = ids++;

        transform = Matrix::identity();
        inverseTransform = Matrix::identity();
        material = Material();
    }

    explicit Geo(int nid) {
        id = nid;
        transform = Matrix::identity();
        material = Material();
    }

    void setMatrix(const Matrix& mat) {
        transform = mat;
        inverseTransform = Matrix::fastInverse(mat);
    }

    virtual Vector normalAt(const Point& p) = 0;

    virtual void intersect(RT::Ray& r, std::vector<RT::Intersection>& s) = 0;
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

    explicit Sphere(const Matrix& trans) {
        transform = trans;
        inverseTransform = Matrix::fastInverse(trans);
    }

    void intersect(RT::Ray& r, std::vector<RT::Intersection>& s) override {
        // Transform the ray according to the object's properties
        RT::Ray r2 = RT::Ray::transform(r, inverseTransform);

        // First calculate the discriminant of the intersection;
        // we need to avoid entering an infinite loop if the ray doesn't intersect.

        Vector delta = r2.origin - Point(0, 0, 0);

        double a = r2.direction * r2.direction;
        double b = 2 * (r2.direction * delta);
        double c = (delta * delta) - 1;

        double discriminant = (b * b) - 4 * a * c;

        // If the discriminant is negative, there is no intersection

        if (discriminant < 0) {
            return;
        }

        // If the discriminant is 0 or positive, there is at least one intersection.

        RT::Intersection intersection1 = {(-b - std::sqrt(discriminant)) / (2 * a), this};
        RT::Intersection intersection2 = {(-b + std::sqrt(discriminant)) / (2 * a), this};

        s.emplace_back(intersection1);
        s.emplace_back(intersection2);
    }

    Vector normalAt(const Point& p) override {
        Point oP = Point(inverseTransform * p);
        Vector oN = oP - Point(0, 0, 0);
        Vector wN = Vector(Matrix::transpose(inverseTransform) * oN);
        wN.w = 0;
        return Vector(wN.normalize());
    }
};

struct Plane : public Geo {

    Plane() = default;

    Vector normalAt(const Point &p) override {
        (void) p;
        static const Vector normal = {0, 1, 0};
        return Vector(inverseTransform * normal);
    }

    void intersect(RT::Ray& r, std::vector<RT::Intersection>& s) override {
        RT::Ray r2 = RT::Ray::transform(r, inverseTransform);
        if (std::abs(r2.direction.y) < 0.001) return;

        double t = -r2.origin.y / r2.direction.y;

        // TODO: hotspot. 0.527s / 4.728s spent here; 11% of execution time.
        s.emplace_back(RT::Intersection(t, this));
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

inline RT::IntersectionDetail RT::Intersection::fillDetail(const Intersection& i, Ray r, Intersections& isections) {
    Point hitPos = Ray::position(r, i.time);
    Vector hitNormal = i.object->normalAt(hitPos);
    Vector eyeDir = -r.direction;
    Vector reflectv = r.direction.reflect(hitNormal);
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

    bool inside = (hitNormal * eyeDir) < 0;
    if (inside) hitNormal = -hitNormal;

    return { i.time, *i.object, hitPos, bumpPoint, underPoint, eyeDir, hitNormal, reflectv, n1, n2, inside };
}

namespace Pattern {
    inline Color colorAt(const Point& point, Geo* object) {
        Point objectPoint = Point(object->inverseTransform * point);
        Point patternPoint = Point(object->material.pattern->inverseTransform * objectPoint);

        return object->material.pattern->at(patternPoint);
    }
}