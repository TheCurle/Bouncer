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

    Geo() {
        static int ids = 0;
        id = ids++;
    }

    Geo(int nid) { id = nid; }
};

struct Sphere : public Geo {

};

bool operator==(Geo x, Geo y);

#ifdef GEOMETRY_OPERATOR_OVERLOADS
bool operator==(Geo x, Geo y) {
    return x.id == y.id;
}
#endif