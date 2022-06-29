/***************
 * THECURLE    *
 *     BOUNCER *
 ***************/

#include <catch2/catch_test_macros.hpp>
#include <type/Matrix.h>
#include <render/Ray.h>

SCENARIO("Normal on a sphere at a point on the X axis") {
    GIVEN("s: sphere()") {
        Sphere s;
        WHEN("n: normal_at(s, point(1, 0, 0))") {
            Vector n = s.normalAt({ 1, 0, 0 });
            THEN("n = vector(1, 0, 0)") {
                REQUIRE(n == Vector(1, 0, 0));
            }
        }
    }
}

SCENARIO("Normal on a sphere at a point on the Y axis") {
    GIVEN("s: sphere()") {
        Sphere s;
        WHEN("n: normal_at(s, point(0, 1, 0))") {
            Vector n = s.normalAt({ 0 , 1, 0 });
            THEN("n = vector(0, 1, 0)") {
                REQUIRE(n == Vector(0, 1, 0));
            }
        }
    }
}

SCENARIO("Normal on a sphere at a point on the Z axis") {
    GIVEN("s: sphere()") {
        Sphere s;
        WHEN("n: normal_at(s, point(0, 0, 1))") {
            Vector n = s.normalAt({ 0, 0, 1 });
            THEN("n = vector(0, 0, 1)") {
                REQUIRE(n == Vector(0, 0, 1));
            }
        }
    }
}

SCENARIO("Normal on a sphere at a point on no axis") {
    GIVEN("s: sphere()") {
        Sphere s;
        WHEN("n: normal_at(s, point(sqrt(3) / 3, sqrt(3) / 3, sqrt(3) / 3))") {
            Vector n = s.normalAt({ std::sqrt(3) / 3, std::sqrt(3) / 3, std::sqrt(3) / 3 });
            THEN("n = vector(sqrt(3) / 3, sqrt(3) / 3, sqrt(3) / 3)") {
                REQUIRE(n == Vector(std::sqrt(3) / 3, std::sqrt(3) / 3, std::sqrt(3) / 3));
            }
        }
    }
}

SCENARIO("Normal vectors are always normalized") {
    GIVEN("s: sphere()") {
        Sphere s;
        WHEN("n: normal_at(s, point(sqrt(3) / 3, sqrt(3) / 3, sqrt(3) / 3))") {
            Vector n = s.normalAt({ std::sqrt(3) / 3, std::sqrt(3) / 3, std::sqrt(3) / 3 });
            THEN("n = normalize(n)") {
                REQUIRE(n == n.normalize());
            }
        }
    }
}

SCENARIO("Normal of a translated sphere") {
    GIVEN("s: sphere()") {
        Sphere s;
        AND_GIVEN("s.set_transform(translation(0, 1, 0))") {
            s.transform = Matrix::translation(0, 1, 0);
            WHEN("n: normal_at(s, point(0, 1.70711, -0.70711))") {
                Vector n = s.normalAt({ 0, 1.70711, -0.70711 });
                THEN("n = vector(0, 0.70711, -0.70711)") {
                    REQUIRE(n == Vector(0, 0.70711, -0.70711));
                }
            }
        }
    }
}

SCENARIO("Normal of a scaled and rotated sphere") {
    GIVEN("s: sphere()") {
        Sphere s;
        AND_GIVEN("m: scaling(1, 0.5, 1) * rotation_z(pi/5)") {
            Matrix m = Matrix::scaling(1, 0.5, 1) * Matrix::rotation_z(M_PI / 5);
            AND_GIVEN("s.set_transform(m)") {
                s.transform = m;
                WHEN("n: normal_at(s, point(0, sqrt(2) / 2, -sqrt(2) / 2))") {
                    Vector n = s.normalAt({ 0, std::sqrt(2) / 2, -std::sqrt(2) / 2 });
                    THEN("n = vector(0, 0.97014, -0.24254)") {
                        REQUIRE(n == Vector(0, 0.97014, -0.24254));
                    }
                }
            }
        }
    }
}

SCENARIO("Reflecting a vector at 45 degrees") {
    GIVEN("v: vector(1, -1, 0)") {
        Vector v { 1, -1, 0 };
        AND_GIVEN("n: vector(0, 1, 0)") {
            Vector n { 0, 1, 0 };
            WHEN("r: reflect(v, n)") {
                Vector r = v.reflect(n);
                THEN("r = vector(1, 1, 0)") {
                    REQUIRE(r == Vector(1, 1, 0));
                }
            }
        }
    }
}

SCENARIO("Reflecting a vector from a tilted surface") {
    GIVEN("v: vector(0, -1, 0)") {
        Vector v { 0, -1, 0 };
        AND_GIVEN("n: vector(sqrt(2) / 2, sqrt(2) / 2, 0)") {
            Vector n { std::sqrt(2) / 2, std::sqrt(2) / 2, 0 };
            WHEN("r: reflect(v, n)") {
                Vector r = v.reflect(n);
                THEN("r = vector(1, 0, 0)") {
                    REQUIRE(r == Vector(1, 0, 0));
                }
            }
        }
    }
}