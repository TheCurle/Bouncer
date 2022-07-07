/***************
 * THECURLE    *
 *     BOUNCER *
 ***************/

#include <catch2/catch_test_macros.hpp>
#include <core/Raster.hpp>
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

        AND_THEN("m.reflectivity = 0") {
            REQUIRE(m.reflectivity == 0);
        }

        AND_THEN("m.transparency = 0") {
            REQUIRE(m.transparency == 0);
        }

        AND_THEN("m.refractiveIndex = 1") {
            REQUIRE(m.refractiveIndex == 1);
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

SCENARIO("Lighting with a pattern") {
    GIVEN("m: material()") {
        Material m;

        AND_GIVEN("m.pattern: stripe_pattern(color(1, 1, 1), color(0, 0, 0))") {
            m.pattern = new Pattern::Stripe(Color::white(), Color::black());
            AND_GIVEN("m.ambient: 1") {
                m.ambient = 1;
                AND_GIVEN("m.diffuse: 0") {
                    m.diffuse = 0;
                    AND_GIVEN("m.specular: 0") {
                        m.specular = 0;
                        AND_GIVEN("eyev: vector(0, 0, -1)") {
                            Vector eyev { 0, 0, -1 };
                            AND_GIVEN("normalv: vector(0, 0, -1)") {
                                Vector normalv {0, 0, -1};
                                AND_GIVEN("light: point_light( point(0, 0, -10), color(1, 1, 1) )") {
                                    PointLight light { { 0, 0, -10 }, { 1, 1, 1 } };
                                    WHEN("c1: lighting(m, light, point(0, 0, 0), eyev, normalv, false)") {
                                        Color c1 = Light::lighting(m, new Sphere(m), light,  Point(0, 0, 0), eyev, normalv, false);
                                        WHEN("c2: lighting(m, light, point(1, 0, 0), eyev, normalv, false)") {
                                            Color c2 = Light::lighting(m, new Sphere(m), light, Point(1, 0, 0), eyev, normalv, false);

                                            THEN("c1 = color(1, 1, 1)") {
                                                REQUIRE(c1 == Color::white());
                                            }

                                            AND_THEN("c2 = color(0, 0, 0)") {
                                                REQUIRE(c2 == Color::black());
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
    }
}

SCENARIO("A glass sphere") {
    GIVEN("s: glass_sphere()") {
        Sphere s = Sphere::glassSphere();

        THEN("s.transform = identity_matrix") {
            REQUIRE(s.transform == Matrix::identity());
        }

        AND_THEN("s.material.transparency = 1") {
            REQUIRE(s.material.transparency == 1);
        }

        AND_THEN("s.material.refractiveIndex = 1.5") {
            REQUIRE(s.material.refractiveIndex == 1.5);
        }
    }
}
