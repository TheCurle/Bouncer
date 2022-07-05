/***************
 * THECURLE    *
 *     BOUNCER *
 ***************/

#include <catch2/catch_test_macros.hpp>
#include <type/Matrix.h>
#include <render/Ray.h>
#include "render/Geometry.h"

SCENARIO("The normal of a plane is constant everywhere") {
    GIVEN("p: plane()") {
        Plane p;
        WHEN("n1: normal_at(p, point(0, 0, 0) )") {
            Vector n1 = p.normalAt({ 0, 0, 0 });
            AND_WHEN("n2: normal_at(p, point(10, 0, -10))") {
                Vector n2 = p.normalAt({ 10, 0, -10});
                AND_WHEN("n3: normal_at(p, point(-5, 0, 150) )") {
                    Vector n3 = p.normalAt({ -5, 0, 150});

                    Vector expected { 0, 1, 0 };
                    THEN("n1 = vector(0, 1, 0)") {
                        REQUIRE(n1 == expected);
                    }

                    AND_THEN("n2 = vector(0, 1, 0)") {
                        REQUIRE(n2 == expected);
                    }

                    AND_THEN("n3 = vector(0, 1, 0)") {
                        REQUIRE(n3 == expected);
                    }
                }
            }
        }
    }
}

SCENARIO("Intersecting parallel to the plane") {
    GIVEN("p: plane()") {
        Plane p;
        AND_GIVEN("r: ray( point(0, 10, 0), vector(0, 0, 1) )") {
            Ray r { { 0, 10, 0 }, { 0, 0, 1 } };
            WHEN("xs: intersect(p, r)") {
                std::vector<Intersection> sect;
                p.intersect(r, sect);
                Intersections xs ( sect.begin(), sect.end() );
                THEN("xs is empty") {
                    REQUIRE(xs.size == 0);
                }
            }
        }
    }
}

SCENARIO("Intersecting coplanar to the plane") {
    GIVEN("p: plane()") {
        Plane p;
        AND_GIVEN("r: ray( point(0, 0, 0), vector(0, 0, 1) )") {
            Ray r { { 0, 0, 0 }, { 0, 0, 1 } };
            WHEN("xs: intersect(p, r)") {
                std::vector<Intersection> sect;
                p.intersect(r, sect);
                Intersections xs ( sect.begin(), sect.end() );
                THEN("xs is empty") {
                    REQUIRE(xs.size == 0);
                }
            }
        }
    }
}

SCENARIO("A ray intersecting a plane from above") {
    GIVEN("p: plane()") {
        Plane p;
        AND_GIVEN("r: ray( point(0, 1, 0), vector(0, -1, 0) )") {
            Ray r { { 0, 1, 0 }, { 0, -1, 0 } };
            WHEN("xs: intersect(p, r)") {
                std::vector<Intersection> sect;
                p.intersect(r, sect);
                Intersections xs ( sect.begin(), sect.end() );

                THEN("xs.count = 1") {
                    REQUIRE(xs.size == 1);
                }

                AND_THEN("xs[0].t = 1") {
                    REQUIRE(xs[0].time == 1);
                }

                AND_THEN("xs[0].object == p") {
                    REQUIRE(*xs[0].object == p);
                }
            }
        }
    }
}

SCENARIO("A ray intersecting a plane from below") {
    GIVEN("p: plane()") {
        Plane p;
        AND_GIVEN("r: ray( point(0, -1, 0), vector(0, 1, 0) )") {
            Ray r { { 0, -1, 0 }, { 0, 1, 0 } };
            WHEN("xs: intersect(p, r)") {
                std::vector<Intersection> sect;
                p.intersect(r, sect);
                Intersections xs ( sect.begin(), sect.end() );

                THEN("xs.count = 1") {
                    REQUIRE(xs.size == 1);
                }

                AND_THEN("xs[0].t = 1") {
                    REQUIRE(xs[0].time == 1);
                }

                AND_THEN("xs[0].object == p") {
                    REQUIRE(*xs[0].object == p);
                }
            }
        }
    }
}