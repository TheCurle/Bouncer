/***************
 * THECURLE    *
 *     BOUNCER *
 ***************/

#include <catch2/catch_test_macros.hpp>
#include <utility>
#include <core/Matrix.h>
#include <render/Light.h>
#include <render/Ray.h>

#pragma once

/**
 * Represents a generic object that can be rendered.
 * Objects are determined to be identical if they have identical transformations and materials.
 *
 * The collision and normal logic is abstract, to be implemented in subclasses.
 */
struct Geo {
    // The integer ID of this object
    int id;
    // Transformation of this object; determines how it is moved and rotated relative to the world origin.
    Matrix transform;
    // Inverse transformation; determines how the world must be moved and rotated relative to the object.
    Matrix inverseTransform;
    // Render material detail. Determines how it is rendered and the effect of various lighting calculations.
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

    // Get the normal vector at the given point on the object.
    virtual Vector normalAt(const Point& p) = 0;

    // For raytracing; append all intersections with the given ray to the given vector.
    virtual void intersect(RT::Ray& r, std::vector<RT::Intersection>& s) = 0;
};

/**
 * A sphere.
 * By default, a unit sphere (of radius 1) centered on the origin (0, 0, 0).
 *
 * Apply transformations to move it.
 */
struct Sphere : public Geo {
    Sphere() = default;
    Sphere(Sphere&) = default;
    Sphere(Sphere&&) = default;

    static Sphere glassSphere() {
        static Material glassMaterial;
        glassMaterial.diffuse = 0;
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

        // a, b and c have left us with a quadratic formula to solve.
        // We either have 0, 1 or 2 solutions.
        // Calculate the discriminant b^2 - 4ac to determine how many solutions there are.
        double discriminant = (b * b) - 4 * a * c;

        // If the discriminant is negative, there is no intersection
        if (discriminant < 0) {
            return;
        }

        // If the discriminant is 0 or positive, there is at least one intersection.
        // We can now solve the quadratic (-b +/- sqrt(b^2 - 4ac)) / 2a to get our solutions.
        // If the ray is tangent to the sphere, both will return the same value.
        // This keeps the ordering of intersections.

        RT::Intersection intersection1 = {(-b - std::sqrt(discriminant)) / (2 * a), this};
        RT::Intersection intersection2 = {(-b + std::sqrt(discriminant)) / (2 * a), this};

        s.emplace_back(intersection1);
        s.emplace_back(intersection2);
    }

    // The normal of a sphere is the opposite of the vector leading from the point to the origin.
    // We subtract the point from the origin to get the vector, then transpose the inverse matrix to account for scaling and skewing of the sphere.
    // Normalizing this vector will return the normal of the sphere at that point.
    Vector normalAt(const Point& p) override {
        Point oP = Point(inverseTransform * p);
        Vector oN = oP - Point(0, 0, 0);
        Vector wN = Vector(Matrix::transpose(inverseTransform) * oN);
        return Vector(wN.normalize());
    }
};

/**
 * A plane; infinitely flat, infinitely large.
 */
struct Plane : public Geo {

    Plane() = default;

    // The normal of a plane is always upward.
    // We can rotate and translate it according to the transformation, but this should not change.
    Vector normalAt(const Point &p) override {
        (void) p;
        static const Vector normal = {0, 1, 0};
        return Vector(inverseTransform * normal);
    }

    // Intersecting the ray with a plane is simple; translate the ray, check whether it's parallel, and append the intersection.
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

    bool inside = (hitNormal * eyeDir) < 0;
    if (inside) {
        hitNormal = -hitNormal;
    }

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


    return { i.time, *i.object, hitPos, bumpPoint, underPoint, eyeDir, hitNormal, reflectv, n1, n2, inside };
}

namespace Pattern {
    inline Color colorAt(const Point& point, Geo* object) {
        Point objectPoint = Point(object->inverseTransform * point);
        Point patternPoint = Point(object->material.pattern->inverseTransform * objectPoint);

        return object->material.pattern->at(patternPoint);
    }
}