/***************
 * THECURLE    *
 *     BOUNCER *
 ***************/

#include <catch2/catch_test_macros.hpp>

#define GEOMETRY_OPERATOR_OVERLOADS
#include <render/Geometry.h>
#undef GEOMETRY_OPERATOR_OVERLOADS

#include <render/Ray.h>

SCENARIO("An intersection encapsulates t and object") {
    GIVEN("s: sphere()") {
        Sphere s;
        WHEN("i: intersection(3.5, s)") {
            Intersection i(3.5, s);

            THEN("i.time = 3.5") {
                REQUIRE(i.time == 3.5);
            }

            AND_THEN("i.object = s") {
                REQUIRE(i.object == s);
            }
        }
    }
}

SCENARIO("Aggregating intersections") {
    GIVEN("s: sphere()") {
        Sphere s;
        AND_GIVEN("i1: intersection(1, s)") {
            Intersection i1(1, s);
            AND_GIVEN("i2: intersection(2, s)") {
                Intersection i2(2, s);
                WHEN("xs: intersections(i1, i2)") {
                    Intersections xs {i1, i2};

                    THEN("xs.size = 2") {
                        REQUIRE(xs.size() == 2);
                    }

                    AND_THEN("xs[0].time = 1") {
                        REQUIRE(xs[0].time == 1);
                    }

                    AND_THEN("xs[1].time = 2") {
                        REQUIRE(xs[1].time == 2);
                    }
                }
            }
        }
    }
}

SCENARIO("Intersect sets the object") {
    GIVEN("r: ray( point(0, 0, -5), vector(0, 0, 1) )") {
        Ray r(Point(0, 0, -5), Vector(0, 0, 1));
        AND_GIVEN("s: sphere()") {
            Sphere s;
            WHEN("xs: intersect(s, r)") {
                Intersections xs = Ray::intersect(s, r);

                THEN("xs.size = 2") {
                    REQUIRE(xs.size() == 2);
                }

                AND_THEN("xs[0].object = s") {
                    REQUIRE(xs[0].object == s);
                }

                AND_THEN("xs[1].object = s") {
                    REQUIRE(xs[1].object == s);
                }
            }
        }
    }
}