/***************
 * THECURLE    *
 *     BOUNCER *
 ***************/

#include <catch2/catch_test_macros.hpp>
#include <string>
#include <core/Raster.hpp>
#include <fstream>

SCENARIO("Creating a Framebuffer", "[Raster]") {
    GIVEN("f <- framebuffer(10, 20)") {
        Framebuffer f(10, 20);

        THEN("f.width = 10") {
            REQUIRE(f.width == 10);
        }
        AND_THEN("f.height = 20") {
            REQUIRE(f.height == 20);
        }
        AND_THEN("every pixel of f is color(0, 0, 0)") {
            REQUIRE(f.at(0, 0) == Color::black().pack());
            REQUIRE(f.at(2, 3) == Color::black().pack());
            REQUIRE(f.at(9, 19) == Color::black().pack());
        }
    }
}

SCENARIO("Writing a pixel to a framebuffer", "[Raster]") {
    GIVEN("f <- framebuffer(10, 20)") {
        Framebuffer f(10, 20);
        AND_GIVEN("red <- Color(1, 0, 0)") {
            Color red(1, 0, 0);
            WHEN("f.set(2, 3, red)") {
                f.set(2, 3, red);
                THEN("f.at(2, 3) = red") {
                    REQUIRE((f.at(2, 3) == red.pack()));
                    REQUIRE(!(f.at(2, 3) == Color::black().pack()));
                }
            }
        }
    }
}
