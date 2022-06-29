/***************
 * THECURLE    *
 *     BOUNCER *
 ***************/

#include <catch2/catch_test_macros.hpp>
#include <type/Raster.hpp>
#define LIGHT_OPERATOR_OVERLOADS
#include <render/Light.h>
#include <render/Geometry.h>

SCENARIO("The default material") {
    GIVEN("m: material()") {
        Material m;

        THEN("m.color = color(1, 1, 1)") {
            REQUIRE(m.color == Color(1, 1, 1));
        }

        AND_THEN("m.ambient = 0.1") {
            REQUIRE(m.ambient == 0.1);
        }

        AND_THEN("m.diffuse = 0.9") {
            REQUIRE(m.diffuse == 0.9);
        }

        AND_THEN("m.specular = 0.9") {
            REQUIRE(m.specular == 0.9);
        }

        AND_THEN("m.shininess = 200") {
            REQUIRE(m.shininess == 200);
        }
    }
}

SCENARIO("A sphere has a default material") {
    GIVEN("s: sphere()") {
        Sphere s;
        WHEN("m: s.material") {
            Material m = s.material;

            THEN("m = material()") {
                REQUIRE(m == Material());
            }
        }
    }
}

SCENARIO("A sphere may be assigned a material") {
    GIVEN("s: sphere()") {
        Sphere s;
        AND_GIVEN("m: material()") {
            Material m;
            AND_GIVEN("m.ambient = 1") {
                m.ambient = 1;
                WHEN("s.material = m") {
                    s.material = m;

                    THEN("s.material = m") {
                        REQUIRE(s.material == m);
                    }
                }
            }
        }
    }
}
