/***************
 * THECURLE    *
 *     BOUNCER *
 ***************/

#include <catch2/catch_test_macros.hpp>
#include <type/Matrix.h>
#include <list>
#include <algorithm>
#include <utility>

#pragma once

struct Ray;
struct Geo;

// An expanded detail version of Intersection, used for rendering.
struct IntersectionDetail {
    double time;
    Geo& object;
    Point point;
    Point overPoint;
    Vector eyev;
    Vector normalv;
    Vector reflectv;
    bool isInternal;
};

// An individual ray-geometry intersection. Used for collision detection.
struct Intersection {
    double time;
    Geo* object;

    Intersection(double time, Geo* object) : time(time), object(object) {}

    Intersection(const Intersection& other) = default;

    bool operator==(const Intersection& other) const {
        return time == other.time && object == other.object;
    }

    [[nodiscard]] bool isEmpty() const {
        return object == nullptr;
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

        return { 0, nullptr };
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

    Ray(Point o, const Vector& d) : origin(o), direction(d) {}

    // Calculate the position of this ray after t units of travel.
    static Point position(Ray r, double t) {
        return Point(r.origin + ((Tuple) r.direction * t));
    }

    static Ray transform(const Ray& r, const Matrix& m) {
        Point transformedOrigin = Point(m * r.origin);
        Vector transformedDir = Vector(m * r.direction);
        return { transformedOrigin, transformedDir };
    }
};
