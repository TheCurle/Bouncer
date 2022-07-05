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
struct Intersections;

// An expanded detail version of Intersection, used for rendering.
struct IntersectionDetail {
    double time;
    Geo& object;
    Point point;
    Point overPoint;
    Point underPoint;
    Vector eyev;
    Vector normalv;
    Vector reflectv;
    double refractiveIdxIncoming;
    double refractiveIdxOutgoing;
    bool isInternal;
};

// An individual ray-geometry intersection. Used for collision detection.
struct Intersection {
    double time;
    Geo* object;

    Intersection() {
        time = -1;
        object = nullptr;
    }

    Intersection(double time, Geo* object) : time(time), object(object) {}

    Intersection(const Intersection& other) = default;

    bool operator==(const Intersection& other) const {
        return time == other.time && object == other.object;
    }

    [[nodiscard]] bool isEmpty() const {
        return object == nullptr;
    }

    static IntersectionDetail fillDetail(const Intersection& i, Ray r, Intersections& sections);
};

// A collection of Intersection objects
struct Intersections {
    size_t size;
    std::unique_ptr<Intersection[]> isections;

    template <class iter>
    Intersections(const iter start, const iter end) {
        size = std::distance(start, end);
        isections = std::make_unique<Intersection[]>(size);
        std::copy(start, end, isections.get());
        sort();
    }

    Intersections(const std::initializer_list<Intersection>& init) {
        size = init.size();
        isections = std::make_unique<Intersection[]>(size);
        std::copy(init.begin(), init.end(), isections.get());
        sort();
    }

    Intersections sort() {
        std::sort(isections.get(), isections.get() + size, [](const Intersection& a, const Intersection& b) {return a.time < b.time; });
        return *this;
    }

    Intersections() = default;
    Intersections(const Intersections& sections) : size(sections.size), isections(std::make_unique<Intersection[]>(sections.size)) {
        std::copy(sections.isections.get(),sections.isections.get() + sections.size, isections.get());
    }

    Intersection operator[](size_t index) {
        return isections[index];
    }

    Intersection hit() {
        for (size_t i = 0; i < size; i++) {
            if (isections[i].time >= 0) return isections[i];
        }

        return { 0, nullptr };
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
