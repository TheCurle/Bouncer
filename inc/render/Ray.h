/***************
 * THECURLE    *
 *     BOUNCER *
 ***************/

#include <catch2/catch_test_macros.hpp>
#include <type/Matrix.h>
#include <render/Geometry.h>
#include <list>
#include <algorithm>
#include <utility>

#pragma once
using namespace std;

struct Ray;

// An expanded detail version of Intersection, used for rendering.
struct IntersectionDetail {
    double time;
    Geo object;
    Point point;
    Vector eyev;
    Vector normalv;
    bool isInternal;
};

// An individual ray-geometry intersection. Used for collision detection.
struct Intersection {
    double time;
    Geo object;

    Intersection(double time, Geo object) : time(time), object(std::move(object)) {}

    Intersection(const Intersection& other) = default;

    bool operator==(const Intersection& other) const {
        return time == other.time && object == other.object;
    }

    [[nodiscard]] bool isEmpty() const {
        return object.id == -1;
    }

    static IntersectionDetail fillDetail(const Intersection& i, Ray r);
};

// A collection of Intersection objects
struct Intersections {
    std::vector<Intersection> isections;

    Intersections(std::initializer_list<Intersection> init) : isections(init) {
        sort();
    }

    Intersections sort() {
        std::sort(isections.begin(), isections.end(), [](const Intersection& a, const Intersection& b) {return a.time < b.time; });
        return *this;
    }

    Intersections() = default;

    [[nodiscard]] size_t size() const {
        return isections.size();
    }

    Intersection operator[](size_t index) {
        return isections[index];
    }

    Intersection hit() {
        for (const auto& i : isections) {
            if (i.time >= 0) return i;
        }

        return { 0, Geo(-1) };
    }

    void addHit(const Intersection& isection) {
        isections.emplace_back(isection);
    }

    void addAllHits(const Intersections& other) {
        isections.insert(isections.end(), other.isections.begin(), other.isections.end());
    }
};

// The ray that gives Ray Tracing its' name
struct Ray {
    Point origin;
    Vector direction;

    Ray(Point o, Vector d) : origin(o), direction(d) {}

    // Calculate the position of this ray after t units of travel.
    static Point position(Ray r, double t) {
        return Point(r.origin + ((Tuple) r.direction * t));
    }

    // Calculate all of the points where the given ray intersects the given geometry
    static Intersections intersect(const Geo& geo, const Ray& r) {

        // Transform the ray according to the object's properties
        Ray r2 = Ray::transform(r, Matrix::inverse(geo.transform));

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

        Intersection intersection1 = { (-b - std::sqrt(discriminant)) / (2 * a), geo };
        Intersection intersection2 = { (-b + std::sqrt(discriminant)) / (2 * a), geo };

        return { intersection1, intersection2 };
    }

    static Ray transform(const Ray& r, const Matrix& m) {
        Point transformedOrigin = Point(m * r.origin);
        Vector transformedDir = Vector(m * r.direction);
        return { transformedOrigin, transformedDir };
    }
};

inline IntersectionDetail Intersection::fillDetail(const Intersection& i, Ray r) {
    Point hitPos = Ray::position(r, i.time);
    Vector hitNormal = i.object.normalAt(hitPos);
    Vector eyeDir = -r.direction;

    bool inside = (hitNormal * eyeDir) < 0;
    if (inside) hitNormal = -hitNormal;

    return { i.time, i.object, hitPos, eyeDir, hitNormal, inside };
}
