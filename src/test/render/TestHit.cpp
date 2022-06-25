/***************
 * THECURLE    *
 *     BOUNCER *
 ***************/

#include <catch2/catch_test_macros.hpp>
#include <render/Ray.h>

SCENARIO("Hit when all intersections have positive t") {
    GIVEN("s: sphere()") {
        Sphere s;
        AND_GIVEN("i1: intersection(1, s)") {
            Intersection i1 {1, s};
            AND_GIVEN("i2: intersection(2, s)") {
                Intersection i2 {2, s};
                AND_GIVEN("xs: intersections(i2, i1)") {
                    Intersections xs { i2, i1 };

                    WHEN("i: hit(xs)") {
                        Intersection i = xs.hit();

                        THEN("i = i1") {
                            REQUIRE(i == i1);
                        }
                    }
                }
            }
        }
    }
}

SCENARIO("Hit when some intersections have negative t") {
    GIVEN("s: sphere()") {
        Sphere s;
        AND_GIVEN("i1: intersection(-1, s)") {
            Intersection i1 {-1, s};
            AND_GIVEN("i2: intersection(1, s)") {
                Intersection i2 {1, s};
                AND_GIVEN("xs: intersections(i2, i1)") {
                    Intersections xs { i2, i1 };

                    WHEN("i: hit(xs)") {
                        Intersection i = xs.hit();

                        THEN("i = i2") {
                            REQUIRE(i == i2);
                        }
                    }
                }
            }
        }
    }
}

SCENARIO("Hit when all intersections have negative t") {
    GIVEN("s: sphere()") {
        Sphere s;
        AND_GIVEN("i1: intersection(-2, s)") {
            Intersection i1 {-2, s};
            AND_GIVEN("i2: intersection(-1, s)") {
                Intersection i2 {-1, s};
                AND_GIVEN("xs: intersections(i2, i1)") {
                    Intersections xs { i2, i1 };

                    WHEN("i: hit(xs)") {
                        Intersection i = xs.hit();

                        THEN("i = none") {
                            REQUIRE(i.isEmpty());
                        }
                    }
                }
            }
        }
    }
}

SCENARIO("Hit is always the lowest non-negative intersection") {
    GIVEN("s: sphere()") {
        Sphere s;
        AND_GIVEN("i1: intersection(5, s)") {
            Intersection i1 {5, s};
            AND_GIVEN("i2: intersection(7, s)") {
                Intersection i2 {7, s};
                AND_GIVEN("i3: intersection(-3, s)") {
                    Intersection i3 {-3, s};
                    AND_GIVEN("i4: intersection(2, s)") {
                        Intersection i4 {2, s};

                        AND_GIVEN("xs: intersections(i1, i2, i3, i4)") {
                            Intersections xs { i1, i2, i3, i4 };

                            WHEN("i: hit(xs)") {
                                Intersection i = xs.hit();

                                THEN("i = i4") {
                                    REQUIRE(i == i4);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

