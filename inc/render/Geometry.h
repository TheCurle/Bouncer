/***************
 * THECURLE    *
 *     BOUNCER *
 ***************/

#include <catch2/catch_test_macros.hpp>
#include <type/Matrix.h>

#pragma once
using namespace std;

struct Geo {
    int id;
    Matrix transform;

    Geo() {
        static int ids = 0;
        id = ids++;

        transform = Matrix::identity();
    }

    Geo(int nid) {
        id = nid;
        transform = Matrix::identity();
    }

    Vector normalAt(const Point& p) {
        Point oP = Point(Matrix::inverse(transform) * p);
        Vector oN = oP - Point(0, 0, 0);
        Vector wN = Vector(Matrix::transpose(Matrix::inverse(transform)) * oN);
        wN.w = 0;
        return Vector(wN.normalize());
    }
};

struct Sphere : public Geo {

};

bool operator==(Geo x, Geo y);

#ifdef GEOMETRY_OPERATOR_OVERLOADS
bool operator==(Geo x, Geo y) {
    return x.id == y.id;
}
#endif