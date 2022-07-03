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
            Intersection i(3.5, &s);

            THEN("i.time = 3.5") {
                REQUIRE(i.time == 3.5);
            }

            AND_THEN("i.object = s") {
                REQUIRE(*i.object == s);
            }
        }
    }
}

SCENARIO("Aggregating intersections") {
    GIVEN("s: sphere()") {
        Sphere s;
        AND_GIVEN("i1: intersection(1, s)") {
            Intersection i1(1, &s);
            AND_GIVEN("i2: intersection(2, s)") {
                Intersection i2(2, &s);
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
                Intersections xs = s.intersect(r);

                THEN("xs.size = 2") {
                    REQUIRE(xs.size() == 2);
                }

                AND_THEN("xs[0].object = s") {
                    REQUIRE(*xs[0].object == s);
                }

                AND_THEN("xs[1].object = s") {
                    REQUIRE(*xs[1].object == s);
                }
            }
        }
    }
}

SCENARIO("Filling intersection detail") {
    GIVEN("r: ray( point(0, 0, -5), vector(0, 0, 1) )") {
        Ray r { { 0, 0, -5 }, { 0, 0, 1 } };
        AND_GIVEN("shape: sphere()") {
            Sphere s;
            AND_GIVEN("i: intersection(4, shape)") {
                Intersection i { 4, &s };
                WHEN("detail: prepare_computation(i, r)") {
                    IntersectionDetail detail = Intersection::fillDetail(i, r);

                    THEN("detail.time = i.time") {
                        REQUIRE(detail.time == i.time);
                    }

                    AND_THEN("detail.object = i.object") {
                        REQUIRE(detail.object == *i.object);
                    }

                    AND_THEN("detail.point = point(0, 0, -1)") {
                        REQUIRE(detail.point == Point(0, 0, -1));
                    }

                    AND_THEN("detail.eyev = vector(0, 0, -1)") {
                        REQUIRE(detail.eyev == Vector(0, 0, -1));
                    }

                    AND_THEN("detail.normalv = vector(0, 0, -1)") {
                        REQUIRE(detail.normalv == Vector(0, 0, -1));
                    }
                }
            }
        }
    }
}

SCENARIO("Hit of an external intersection") {
    GIVEN("r: ray( point(0, 0, -5), vector(0, 0, 1) )") {
        Ray r {{0, 0, -5}, {0, 0, 1}};
        AND_GIVEN("shape: sphere()") {
            Sphere s;
            AND_GIVEN("i: intersection(4, shape)") {
                Intersection i {4, &s};
                WHEN("detail: prepare_computation(i, r)") {
                    IntersectionDetail detail = Intersection::fillDetail(i, r);

                    THEN("detail.inside = false") {
                        REQUIRE(detail.isInternal == false);
                    }
                }
            }
        }
    }
}

SCENARIO("Hit of an internal intersection") {
    GIVEN("r: ray( point(0, 0, 0), vector(0, 0, 1) )") {
        Ray r {{0, 0, 0}, {0, 0, 1}};
        AND_GIVEN("shape: sphere()") {
            Sphere s;
            AND_GIVEN("i: intersection(1, shape)") {
                Intersection i {1, &s};
                WHEN("detail: prepare_computation(i, r)") {
                    IntersectionDetail detail = Intersection::fillDetail(i, r);

                    THEN("detail.point = point(0, 0, 1)") {
                        REQUIRE(detail.point == Point(0, 0, 1));
                    }

                    AND_THEN("detail.eyev = vector(0, 0, -1)") {
                        REQUIRE(detail.eyev == Vector(0, 0, -1));
                    }

                    AND_THEN("detail.inside = true") {
                        REQUIRE(detail.isInternal == true);
                    }

                    AND_THEN("detail.normalv = vector(0, 0, -1)") {
                        REQUIRE(detail.normalv == Vector(0, 0, -1));
                    }
                }
            }
        }
    }
}

SCENARIO("Reflecting a vector") {
    GIVEN("shape: plane()") {
        Plane shape;
        AND_GIVEN("r: ray( point(0, 1, -1), vector(0, -sqrt(2) / 2, sqrt(2) / 2) )") {
            Ray r { { 0, 1, -1 }, { 0, -std::sqrt(2) / 2, std::sqrt(2) / 2 } };
            AND_GIVEN("i: intersection(sqrt(2), shape)") {
                Intersection i { std::sqrt(2), &shape };
                WHEN("detail: fillDetail(i, r)") {
                    IntersectionDetail detail = Intersection::fillDetail(i, r);
                    THEN("detail.reflectv = vector(0, sqrt(2) / 2, sqrt(2) / 2)") {
                        REQUIRE(detail.reflectv == Vector(0, std::sqrt(2) / 2, std::sqrt(2) / 2));
                    }
                }
            }
        }
    }
}