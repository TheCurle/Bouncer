/***************
 * THECURLE    *
 *     BOUNCER *
 ***************/

#include <catch2/catch_test_macros.hpp>
#include <render/Ray.h>


SCENARIO("Creating and querying a ray") {
    GIVEN("origin: point(1, 2, 3)"){
        Point origin(1, 2, 3);
        AND_GIVEN("direction: vector(4, 5, 6)") {
            Vector direction(4, 5, 6);
            WHEN("r = ray(origin, direction)") {
                Ray r(origin, direction);

                THEN("r.origin = origin") {
                    REQUIRE(r.origin == origin);
                }

                AND_THEN("r.direction = direction") {
                    REQUIRE(r.direction == direction);
                }
            }
        }
    }
}

SCENARIO("Computing a point from a distance") {
    GIVEN("r: ray( point(2, 3, 4), vector(1, 0, 0) )") {
        Ray r(Point(2, 3, 4), Vector(1, 0, 0));

        THEN("position(r, 0) = point(2, 3, 4)") {
            REQUIRE(Ray::position(r, 0) == Point(2, 3, 4));
        }

        AND_THEN("position(r, 1) = point(3, 3, 4)") {
            REQUIRE(Ray::position(r, 1) == Point(3, 3, 4));
        }

        AND_THEN("position(r, -1) = point(1, 3, 4)") {
            REQUIRE(Ray::position(r, -1) == Point(1, 3, 4));
        }

        AND_THEN("position(r, 2.5) = point(4.5, 3, 4)") {
            REQUIRE(Ray::position(r, 2.5) == Point(4.5, 3, 4));
        }
    }
}

SCENARIO("A ray intersects a sphere at two points") {
    GIVEN("r: ray( point(0, 0, -5), vector(0, 0, 1) )") {
        Ray r( Point(0, 0, -5), Vector(0, 0, 1) );
        AND_GIVEN("s: sphere()") {
            Sphere s;

            WHEN("xs: intersect(s, r)") {
                Intersections xs = Ray::intersect(s, r);

                THEN("xs.size = 2") {
                    REQUIRE(xs.size() == 2);
                }

                AND_THEN("xs[0] = 4") {
                    REQUIRE(xs[0].time == 4);
                }

                AND_THEN("xs[1] = 6") {
                    REQUIRE(xs[1].time == 6);
                }
            }
        }
    }
}

SCENARIO("A ray intersects a sphere at a tangent") {
    GIVEN("r: ray( point(0, 1, -5), vector(0, 0, 1) )") {
        Ray r(Point(0, 1, -5), Vector(0, 0, 1));
        AND_GIVEN("s: sphere()") {
            Sphere s;
            WHEN("xs: intersect(s, r)") {
                Intersections xs = Ray::intersect(s, r);

                THEN("xs.size = 2") {
                    REQUIRE(xs.size() == 2);
                }

                AND_THEN("xs[0] = 5") {
                    REQUIRE(xs[0].time == 5);
                }

                AND_THEN("xs[1] = 5") {
                    REQUIRE(xs[1].time == 5);
                }
            }
        }
    }
}

SCENARIO("A ray misses a sphere") {
    GIVEN("r: ray( point(0, 2, -5), vector(0, 0, 1) )") {
        Ray r(Point(0, 2, -5), Vector(0, 0, 1));
        AND_GIVEN("s: sphere()") {
            Sphere s;
            WHEN("xs: intersect(s, r)") {
                Intersections xs = Ray::intersect(s, r);

                THEN("xs.count = 0") {
                    REQUIRE(xs.size() == 0);
                }
            }
        }
    }
}

SCENARIO("A ray originates inside a sphere") {
    GIVEN("r: ray( point(0, 0, 0), vector(0, 0, 1) )") {
        Ray r(Point(0, 0, 0), Vector(0, 0, 1));
        AND_GIVEN("s: sphere()") {
            Sphere s;
            WHEN("xs: intersect(s, r)") {
                Intersections xs = Ray::intersect(s, r);

                THEN("xs.count = 2") {
                    REQUIRE(xs.size() == 2);
                }

                AND_THEN("xs[0] = -1") {
                    REQUIRE(xs[0].time == -1);
                }

                AND_THEN("xs[1] = 1") {
                    REQUIRE(xs[1].time == 1);
                }
            }
        }
    }
}

SCENARIO("A sphere is behind a ray") {
    GIVEN("r: ray( point(0, 0, 5), vector(0, 0, 1) )") {
        Ray r(Point(0, 0, 5), Vector(0, 0, 1));
        AND_GIVEN("s: sphere()") {
            Sphere s;
            WHEN("xs: intersect(s, r)") {
                Intersections xs = Ray::intersect(s, r);

                THEN("xs.count = 2") {
                    REQUIRE(xs.size() == 2);
                }

                AND_THEN("xs[0] = -6") {
                    REQUIRE(xs[0].time == -6);
                }

                AND_THEN("xs[1] = -4") {
                    REQUIRE(xs[1].time == -4);
                }
            }
        }
    }
}