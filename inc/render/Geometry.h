/***************
 * THECURLE    *
 *     BOUNCER *
 ***************/

#include <catch2/catch_test_macros.hpp>
#include <utility>
#include <type/Matrix.h>
#include "Light.h"

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

    [[nodiscard]] Vector normalAt(const Point& p) const {
        Point oP = Point(Matrix::inverse(transform) * p);
        Vector oN = oP - Point(0, 0, 0);
        Vector wN = Vector(Matrix::transpose(Matrix::inverse(transform)) * oN);
        wN.w = 0;
        return Vector(wN.normalize());
    }
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