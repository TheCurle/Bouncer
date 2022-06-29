/***************
 * THECURLE    *
 *     BOUNCER *
 ***************/

#include <catch2/catch_test_macros.hpp>
#include <type/Raster.hpp>
#define LIGHT_OPERATOR_OVERLOADS
#include <render/Light.h>
#include <render/Geometry.h>

SCENARIO("") {
    GIVEN("m: material()") {
        Material m;
        AND_GIVEN("position: point(0, 0, 0)") {
            Point position { 0, 0, 0 };


        }
    }
}

SCENARIO("Lighting; Eye between the light and surface") {
    GIVEN("m: material()") {
        Material m;
        AND_GIVEN("position: point(0, 0, 0)") {
            Point position { 0, 0, 0 };
            AND_GIVEN("eyev: vector(0, 0, -1)") {
                Vector eyev { 0, 0, -1 };
                AND_GIVEN("normalv: vector(0, 0, -1)") {
                    Vector normalv { 0, 0, -1 };
                    AND_GIVEN("light: point_light(point(0, 0, -10), color(1, 1, 1))") {
                        PointLight light { {0, 0, -10 }, { 1, 1, 1 } };
                        WHEN("result: lighting(m, light, position, eyev, normalv)") {
                            Color result = Light::lighting(m, light, position, eyev, normalv);

                            THEN("result = color(1.9, 1.9, 1.9)") {
                                REQUIRE(result == Color(1.9, 1.9, 1.9));
                            }
                        }
                    }
                }
            }
        }
    }
}

SCENARIO("Lighting; Eye between the light and surface, eye offset 45 degrees") {
    GIVEN("m: material()") {
        Material m;
        AND_GIVEN("position: point(0, 0, 0)") {
            Point position { 0, 0, 0 };
            AND_GIVEN("eyev: vector(0, sqrt(2) / 2, sqrt(2) / 2)") {
                Vector eyev { 0, std::sqrt(2) / 2, std::sqrt(2) / 2 };
                AND_GIVEN("normalv: vector(0, 0, -1)") {
                    Vector normalv { 0, 0, -1 };
                    AND_GIVEN("light: point_light(point(0, 0, -10), color(1, 1, 1))") {
                        PointLight light { {0, 0, -10 }, { 1, 1, 1 } };
                        WHEN("result: lighting(m, light, position, eyev, normalv)") {
                            Color result = Light::lighting(m, light, position, eyev, normalv);

                            THEN("result = color(1.0, 1.0, 1.0)") {
                                REQUIRE(result == Color(1.0, 1.0, 1.0));
                            }
                        }
                    }
                }
            }
        }
    }
}

SCENARIO("Lighting; Eye opposite surface, light offset 45 degrees") {
    GIVEN("m: material()") {
        Material m;
        AND_GIVEN("position: point(0, 0, 0)") {
            Point position { 0, 0, 0 };
            AND_GIVEN("eyev: vector(0, 0, -1)") {
                Vector eyev { 0, 0, -1 };
                AND_GIVEN("normalv: vector(0, 0, -1)") {
                    Vector normalv { 0, 0, -1 };
                    AND_GIVEN("light: point_light(point(0, 10, -10), color(1, 1, 1))") {
                        PointLight light { {0, 10, -10 }, { 1, 1, 1 } };
                        WHEN("result: lighting(m, light, position, eyev, normalv)") {
                            Color result = Light::lighting(m, light, position, eyev, normalv);

                            THEN("result = color(0.7364, 0.7364, 0.7364)") {
                                REQUIRE(result == Color(0.7364, 0.7364, 0.7364));
                            }
                        }
                    }
                }
            }
        }
    }
}

SCENARIO("Lighting; Eye in path of reflection") {
    GIVEN("m: material()") {
        Material m;
        AND_GIVEN("position: point(0, 0, 0)") {
            Point position { 0, 0, 0 };
            AND_GIVEN("eyev: vector(0, -sqrt(2) / 2, -sqrt(2) / 2)") {
                Vector eyev { 0, -std::sqrt(2) / 2, -std::sqrt(2) / 2 };
                AND_GIVEN("normalv: vector(0, 0, -1)") {
                    Vector normalv { 0, 0, -1 };
                    AND_GIVEN("light: point_light(point(0, 10, -10), color(1, 1, 1))") {
                        PointLight light { {0, 10, -10 }, { 1, 1, 1 } };
                        WHEN("result: lighting(m, light, position, eyev, normalv)") {
                            Color result = Light::lighting(m, light, position, eyev, normalv);

                            THEN("result = color(1.6364, 1.6364, 1.6364)") {
                                REQUIRE(result == Color(1.6364, 1.6364, 1.6364));
                            }
                        }
                    }
                }
            }
        }
    }
}

SCENARIO("Lighting; light behind surface") {
    GIVEN("m: material()") {
        Material m;
        AND_GIVEN("position: point(0, 0, 0)") {
            Point position { 0, 0, 0 };
            AND_GIVEN("eyev: vector(0, 0, -1)") {
                Vector eyev { 0, 0, -1 };
                AND_GIVEN("normalv: vector(0, 0, -1)") {
                    Vector normalv { 0, 0, -1 };
                    AND_GIVEN("light: point_light(point(0, 0, 10), color(1, 1, 1))") {
                        PointLight light { {0, 0, 10 }, { 1, 1, 1 } };
                        WHEN("result: lighting(m, light, position, eyev, normalv)") {
                            Color result = Light::lighting(m, light, position, eyev, normalv);

                            THEN("result = color(0.1, 0.1, 0.1)") {
                                REQUIRE(result == Color(0.1, 0.1, 0.1));
                            }
                        }
                    }
                }
            }
        }
    }
}