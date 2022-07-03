/***************
 * THECURLE    *
 *     BOUNCER *
 ***************/

#include <catch2/catch_test_macros.hpp>
#include <render/Light.h>
#include <view/World.h>
#include "view/Camera.h"

static bool roundCompare(double a, double b) {
    return std::abs( (std::round(a * 100) / 100) - b) < 0.01;
}

static bool rangeCompare(double a, double b, double tolerance) {
    return a - b < tolerance;
}

SCENARIO("Constructing a Camera") {
    GIVEN("hsize: 160") {
        int hsize = 160;
        AND_GIVEN("vsize: 120") {
            int vsize = 120;
            AND_GIVEN("field_of_view: pi / 2") {
                double fov = M_PI / 2;
                WHEN("c: camera(hsize, vsize, field_of_view)") {
                    Camera c(hsize, vsize, fov);

                    THEN("c.hsize = 160") {
                        REQUIRE(c.horizontalSize == 160);
                    }

                    AND_THEN("c.vsize = 120") {
                        REQUIRE(c.verticalSize == 120);
                    }

                    AND_THEN("c.field_of_view = pi / 2") {
                        REQUIRE(safeCompare(c.fieldOfView, M_PI / 2));
                    }

                    AND_THEN("c.transform = identity_matrix") {
                        REQUIRE(c.transform == Matrix::identity());
                    }
                }
            }
        }
    }
}

SCENARIO("Pixel size of a horizontal camera") {
    GIVEN("c: camera(200, 125, pi / 2)") {
        Camera c(200, 125, M_PI / 2);
        THEN("c.pixel_size = 0.01") {
            REQUIRE(roundCompare(c.pixelSize, 0.01));
        }
    }
}

SCENARIO("Pixel size of a vertical camera") {
    GIVEN("c: camera(125, 200, pi / 2)") {
        Camera c(125, 200, M_PI / 2);
        THEN("c.pixel_size = 0.01") {
            REQUIRE(rangeCompare(c.pixelSize, 0.01, 0.01));
        }
    }
}

SCENARIO("Casting a ray through the camera") {
    GIVEN("c: camera(201, 101, pi / 2)") {
        Camera c(201, 101, M_PI / 2);
        WHEN("r: ray_for_pixel(c, 100, 50)") {
            Ray r = c.rayForPixel(100, 50);

            THEN("r.origin = point(0, 0, 0)") {
                REQUIRE(r.origin == Point(0, 0, 0));
            }

            AND_THEN("r.direction = vector(0, 0, -1)") {
                REQUIRE(r.direction == Vector(0, 0, -1));
            }
        }
    }
}

SCENARIO("Casting a ray through the corner of the camera") {
    GIVEN("c: camera(201, 101, pi / 2)") {
        Camera c(201, 101, M_PI / 2);
        WHEN("r: ray_for_pixel(c, 0, 0)") {
            Ray r = c.rayForPixel(0, 0);

            THEN("r.origin = point(0, 0, 0)") {
                REQUIRE(r.origin == Point(0, 0, 0));
            }

            AND_THEN("r.direction = vector(0.66519, 0.33259, -0.66851)") {
                REQUIRE(r.direction == Vector(0.66519, 0.33259, -0.66851));
            }
        }
    }
}

SCENARIO("Casting a ray through a transformed camera") {
    GIVEN("c: camera(201, 101, pi / 2)") {
        Camera c(201, 101, M_PI / 2);

        WHEN("c.transform = rotation_y(pi / 4) * translation(0, -2, 5)") {
            c.transform = Matrix::rotation_y(M_PI / 4) * Matrix::translation(0, -2, 5);
            AND_WHEN("r: ray_for_pixel(c, 100, 50)") {
                Ray r = c.rayForPixel(100, 50);

                THEN("r.origin = point(0, 2, -5)") {
                    REQUIRE(r.origin == Point(0, 2, -5));
                }

                AND_THEN("r.direction = vector(sqrt(2) / 2, 0, -sqrt(2) / 2)") {
                    REQUIRE(r.direction == Vector(std::sqrt(2) / 2, 0, -std::sqrt(2) / 2));
                }
            }
        }
    }
}

SCENARIO("Rendering with a camera") {
    GIVEN("w: default_world()") {
        World w = World::defaultWorld();
        AND_GIVEN("c: camera(11, 11, pi / 2)") {
            Camera c(11, 11, M_PI / 2);
            AND_GIVEN("from: point(0, 0, -5)") {
                Point from { 0, 0, -5 };
                AND_GIVEN("to: point(0, 0, 0)") {
                    Point to { 0, 0, 0 };
                    AND_GIVEN("up: vector(0, 1, 0)") {
                        Vector up { 0, 1, 0 };
                        AND_GIVEN("c.transform: viewTransform(from, to, up)") {
                            c.transform = World::viewMatrix(from, to, up);
                            WHEN("image: render(c, w)") {
                                Framebuffer image(11, 11);
                                w.render(c, image);

                                THEN("pixel_at(image, 5, 5) = color(0.38066, 0.47583, 0.2855)") {
                                    REQUIRE(image.at(5, 5) == Color(0.38066, 0.47583, 0.2855));
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}