/***************
 * THECURLE    *
 *     BOUNCER *
 ***************/

#include <catch2/catch_test_macros.hpp>
#include <core/Raster.hpp>
#include <render/Light.h>

SCENARIO("A point light has position and intensity") {
    GIVEN("intensity: color(1, 1, 1)") {
        Color intensity { 1, 1, 1 };
        AND_GIVEN("position: point(0, 0, 0)") {
            Point position { 0, 0, 0 };
            WHEN("point_light: point_light(position, intensity)") {
                PointLight pointLight { position, intensity };

                THEN("point_light.position = position") {
                    REQUIRE(pointLight.position == position);
                }

                AND_THEN("point_light.intensity = intensity") {
                    REQUIRE(pointLight.intensity == intensity);
                }
            }
        }
    }
}

