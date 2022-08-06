/***************
 * THECURLE    *
 *     BOUNCER *
 ***************/

#define GEO_RT
#include <catch2/catch_test_macros.hpp>
#include <render/Light.h>
#include <view/World.h>
#include "render/RT/RTWorld.h"

using namespace RT;

SCENARIO("Creating a world") {
    GIVEN("w: world()") {
        World w;

        THEN("w contains no objects") {
            REQUIRE(w.numObjs == 0);
        }

        AND_THEN("w contains no light source") {
            REQUIRE(w.lightSource.intensity == Color(0, 0, 0));
        }
    }
}

SCENARIO("The default world") {
    GIVEN("light: point_light( point(-10, 10, -10), color(1, 1, 1) )") {
        PointLight light { { -10, 10, -10 }, { 1, 1, 1 }};

        AND_GIVEN(R"--("s1: sphere() with:
| material.color    | (0.8, 1.0, 0.6)        |
| material.diffuse  | 0.7                    |
| material.specular | 0.2                    |
        )--") {
            Sphere s1;
            s1.material = Material({ 0.8, 1.0, 0.6 }, 0.1, 0.7, 0.2, 200, 0, 0, 1);

            AND_GIVEN(R"--("s2: sphere() with:
| transform         | scaling(0.5, 0.5, 0.5) |
        )--") {
                Sphere s2;
                s2.transform = Matrix::scaling(0.5, 0.5, 0.5);

                WHEN("w: defaultWorld()") {
                    World w = World::defaultWorld();

                    THEN("w.light = light") {
                        REQUIRE(w.lightSource == light);
                    }

                    AND_THEN("w contains s1") {
                        REQUIRE(*w.objects[0] == s1);
                    }

                    AND_THEN("w contains s2") {
                        REQUIRE(*w.objects[1] == s2);
                    }
                }
            }
        }
    }
}

SCENARIO("Intersecting a world with a ray") {
    GIVEN("w: default_world()") {
        World w = World::defaultWorld();
        AND_GIVEN("r: ray( point(0, 0, -5), vector(0, 0, 1) )") {
            Ray r { {0, 0, -5 }, { 0, 0, 1 } };
            WHEN("xs: intersect_world(w, r)") {
                Intersections xs = RT::intersect(w, r);

                THEN("xs.count = 4") {
                    REQUIRE(xs.size == 4);
                }

                AND_THEN("xs[0].t = 4") {
                    REQUIRE(xs[0].time == 4);
                }

                AND_THEN("xs[1].t = 4.5") {
                    REQUIRE(xs[1].time == 4.5);
                }

                AND_THEN("xs[2].t = 5.5") {
                    REQUIRE(xs[2].time == 5.5);
                }

                AND_THEN("xs[3].t = 6") {
                    REQUIRE(xs[3].time == 6);
                }
            }
        }
    }
}

SCENARIO("Color of a missed ray") {
    GIVEN("w: default_world()") {
        World w = World::defaultWorld();
        AND_GIVEN("r: ray( point(0, 0, -5), vector(0, 1, 0) )") {
            Ray r { { 0, 0, -5 }, { 0, 1, 0 } };
            WHEN("c: colorAt(w, r)") {
                Color c = Light::at(w, r);

                THEN("c = color(0, 0, 0)") {
                    REQUIRE(c == Color(0, 0, 0));
                }
            }
        }
    }
}

SCENARIO("Color of a hit ray") {
    GIVEN("w: default_world()") {
        World w = World::defaultWorld();
        AND_GIVEN("r: ray( point(0, 0, -5), vector(0, 0, 1) )") {
            Ray r { { 0, 0, -5 }, { 0, 0, 1 } };
            WHEN("c: colorAt(w, r)") {
                Color c = Light::at(w, r);

                THEN("c = color(0.38066, 0.47583, 0.2855)") {
                    REQUIRE(c == Color(0.38066, 0.47583, 0.2855));
                }
            }
        }
    }
}

SCENARIO("Transformation matrix for default orientation") {
    GIVEN("from: point(0, 0, 0)") {
        Point from { 0, 0, 0 };
        AND_GIVEN("to: point(0, 0, -1)") {
            Point to { 0, 0, -1 };
            AND_GIVEN("up: vector(0, 1, 0)") {
                Vector up { 0, 1, 0 };
                WHEN("t: view_transformation(from, to, up)") {
                    Matrix t = Camera::viewMatrix(from, to, up);

                    THEN("t = identity_matrix") {
                        REQUIRE(t == Matrix::identity());
                    }
                }
            }
        }
    }
}

SCENARIO("Transformation matrix looking backwards") {
    GIVEN("from: point(0, 0, 0)") {
        Point from { 0, 0, 0 };
        AND_GIVEN("to: point(0, 0, 1)") {
            Point to { 0, 0, 1 };
            AND_GIVEN("up: vector(0, 1, 0)") {
                Vector up { 0, 1, 0 };
                WHEN("t: view_transformation(from, to, up)") {
                    Matrix t = Camera::viewMatrix(from, to, up);

                    THEN("t = scaling(-1, 1, -1") {
                        REQUIRE(t == Matrix::scaling(-1, 1, -1));
                    }
                }
            }
        }
    }
}

SCENARIO("View transformation moves the world") {
    GIVEN("from: point(0, 0, 8)") {
        Point from { 0, 0, 8 };
        AND_GIVEN("to: point(0, 0, 0)") {
            Point to { 0, 0, 0 };
            AND_GIVEN("up: vector(0, 1, 0)") {
                Vector up { 0, 1, 0 };
                WHEN("t: view_transformation(from, to, up)") {
                    Matrix t = Camera::viewMatrix(from, to, up);

                    THEN("t = translation(0, 0, -8)") {
                        REQUIRE(t == Matrix::translation(0, 0, -8));
                    }
                }
            }
        }
    }
}