/***************
 * THECURLE    *
 *     BOUNCER *
 ***************/

#include <catch2/catch_test_macros.hpp>
#include <string>
#include <type/Raster.hpp>
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
            REQUIRE(f.at(0, 0) == Color(0, 0, 0));
            REQUIRE(f.at(2, 3) == Color(0, 0, 0));
            REQUIRE(f.at(9, 19) == Color(0, 0, 0));
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
                    REQUIRE((f.at(2, 3) == red));
                    REQUIRE(!(f.at(2, 3) == Color(0, 0, 0)));
                }
            }
        }
    }
}

SCENARIO("Constructing a Portable PixMap header", "[Raster]") {
    GIVEN("f <- framebuffer(5, 3)") {
        Framebuffer f(5, 10);
        WHEN("ppm <- f.export_ppm()") {
            std::string ppm = f.export_ppm();
            THEN(
                R"--(lines 1-3 of ppm are 
"""
P3
5 10
255
"""             )--") {
                REQUIRE(ppm.substr(0, std::string("P3\n5 10\n255").size())
                            .compare("P3\n5 10\n255") == 0);
            }
        }
    }
}

SCENARIO("Constructing the Portable PixMap data", "[Raster]") {
    GIVEN("f <- framebuffer(5, 3)") {
        Framebuffer f(5, 3);
        AND_GIVEN("c1 <- color(1.5, 0, 0)") {
            Color c1(1.5, 0, 0);
            AND_GIVEN("c2 <- color(0, 0.5, 0)") {
                Color c2(0, 0.5, 0);
                AND_GIVEN("c3 <- color(-0.5, 0, 1)") {
                    Color c3(-0.5, 0, 1);
                    WHEN("f.set(0, 0, c1)") {
                        f.set(0, 0, c1);
                        AND_WHEN("f.set(2, 1, c2)") {
                            f.set(2, 1, c2);
                            AND_WHEN("f.set(4, 2, c3)") {
                                f.set(4, 2, c3);

                                AND_WHEN("ppm <- f.export_ppm()") {
                                    std::string ppm = f.export_ppm();
                                    std::string expectedData("255 0 0 0 0 0 0 0 0 0 0 0 0 0 0\n0 0 0 0 0 0 0 128 0 0 0 0 0 0 0\n0 0 0 0 0 0 0 0 0 0 0 0 0 0 255\n");
                                    THEN("lines 4-6 of ppm are\n"
                                         << expectedData.c_str())
                                    {
                                        REQUIRE(ppm.substr(11, expectedData.size() + 2)
                                                    .compare(expectedData) == 0);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}


SCENARIO("Portable PixMap lines are never longer than 70 characters", "[Raster]") {
    GIVEN("f <- framebuffer(10, 2)") {
        Framebuffer f(10, 2);
        AND_GIVEN("every pixel is set to color(1, 0.8, 0.6)") {
            for(int i = 0; i < 10; i++) 
                for(int j = 0; j < 2; j++) 
                    f.set(i, j, Color(1, 0.8, 0.6));
            
            AND_WHEN("ppm <- f.export_ppm()") {
                std::string ppm = f.export_ppm();
                std::string expectedData("255 204 153 255 204 153 255 204 153 255 204 153 255 204 153 255 204\n153 255 204 153 255 204 153 255 204 153 255 204 153\n255 204 153 255 204 153 255 204 153 255 204 153 255 204 153 255 204\n153 255 204 153 255 204 153 255 204 153 255 204 153\n");
                std::string ppmData = ppm.substr(12, expectedData.size() + 2);
                
                THEN("lines 4-7 of ppm are\n" << expectedData.c_str()) {
                    REQUIRE(ppmData.compare(expectedData) == 0);
                }
            }
        }
    }
}

SCENARIO("Portable PixMap files end with a newline", "[Raster]") {
    GIVEN("f <- framebuffer(10, 2)") {
        Framebuffer f(10, 2);
        WHEN("ppm <- f.export_ppm()") {
            std::string ppm = f.export_ppm();
            THEN("ppm ends with a newline") {
                REQUIRE(ppm.at(ppm.size() - 1) == '\n');
            }
        }
    }
}