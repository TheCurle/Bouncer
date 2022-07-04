/***************
 * THECURLE    *
 *     BOUNCER *
 ***************/

#include <catch2/catch_test_macros.hpp>
#include <render/Light.h>
#include "render/Geometry.h"
#include "view/World.h"

SCENARIO("Finding n1 and n2 at various intersections") {
    GIVEN("a: glass_sphere() with transform: scaling(2, 2, 2) and material.refractiveIndex: 1.5") {
        Sphere a = Sphere::glassSphere();
        a.transform = Matrix::scaling(2, 2, 2);
        a.material.refractiveIndex = 1.5;
        AND_GIVEN("b: glass_sphere() with transform: translation(0, 0, -0.25) and material.refractiveIndex: 2.0") {
            Sphere b = Sphere::glassSphere();
            b.transform = Matrix::translation(0, 0, -0.25);
            b.material.refractiveIndex = 2.0;
            AND_GIVEN("c: glass_sphere() with transform: translation(0, 0, 0.25) and material.refractiveIndex: 2.5") {
                Sphere c = Sphere::glassSphere();
                c.transform = Matrix::translation(0, 0, 0.25);
                c.material.refractiveIndex = 2.5;
                AND_GIVEN("r: ray( point(0, 0, -4), vector(0, 0, 1) )") {
                    Ray r {{0, 0, -4},
                           {0, 0, 1}};
                    AND_GIVEN("xs: intersections(2:A, 2.75:B, 3.25:C, 4.75:B, 5.25:C, 6:A)") {
                        Intersections xs {{2,    &a},
                                          {2.75, &b},
                                          {3.25, &c},
                                          {4.75, &b},
                                          {5.25, &c},
                                          {6,    &a}};

                        WHEN("detail: fillDetail(xs[0], r)") {
                            IntersectionDetail detail = Intersection::fillDetail(xs[0], r, xs);
                            THEN("detail.n1 = 1.0") {
                                REQUIRE(detail.refractiveIdxIncoming == 1);
                            }

                            AND_THEN("detail.n2 = 1.5") {
                                REQUIRE(detail.refractiveIdxOutgoing == 1.5);
                            }
                        }

                        WHEN("detail: fillDetail(xs[1], r)") {
                            IntersectionDetail detail = Intersection::fillDetail(xs[1], r, xs);
                            THEN("detail.n1 = 1.5") {
                                REQUIRE(detail.refractiveIdxIncoming == 1.5);
                            }

                            AND_THEN("detail.n2 = 2.0") {
                                REQUIRE(detail.refractiveIdxOutgoing == 2.0);
                            }
                        }

                        WHEN("detail: fillDetail(xs[2], r)") {
                            IntersectionDetail detail = Intersection::fillDetail(xs[2], r, xs);
                            THEN("detail.n1 = 2.0") {
                                REQUIRE(detail.refractiveIdxIncoming == 2.0);
                            }

                            AND_THEN("detail.n2 = 2.5") {
                                REQUIRE(detail.refractiveIdxOutgoing == 2.5);
                            }
                        }

                        WHEN("detail: fillDetail(xs[3], r)") {
                            IntersectionDetail detail = Intersection::fillDetail(xs[3], r, xs);
                            THEN("detail.n1 = 2.5") {
                                REQUIRE(detail.refractiveIdxIncoming == 2.5);
                            }

                            AND_THEN("detail.n2 = 2.5") {
                                REQUIRE(detail.refractiveIdxOutgoing == 2.5);
                            }
                        }

                        WHEN("detail: fillDetail(xs[4], r)") {
                            IntersectionDetail detail = Intersection::fillDetail(xs[4], r, xs);
                            THEN("detail.n1 = 2.5") {
                                REQUIRE(detail.refractiveIdxIncoming == 2.5);
                            }

                            AND_THEN("detail.n2 = 1.5") {
                                REQUIRE(detail.refractiveIdxOutgoing == 1.5);
                            }
                        }

                        WHEN("detail: fillDetail(xs[5], r)") {
                            IntersectionDetail detail = Intersection::fillDetail(xs[5], r, xs);
                            THEN("detail.n1 = 1.5") {
                                REQUIRE(detail.refractiveIdxIncoming == 1.5);
                            }

                            AND_THEN("detail.n2 = 1.0") {
                                REQUIRE(detail.refractiveIdxOutgoing == 1.0);
                            }
                        }
                    }
                }
            }
        }
    }
}

SCENARIO("Refracted color of an opaque surface") {
    GIVEN("w: default_world()") {
        World w = World::defaultWorld();
        AND_GIVEN("shape: the first shape in w") {
            Geo* shape = w.objects[0];
            AND_GIVEN("r: ray( point(0, 0, -5), vector(0, 0, 1) )") {
                Ray r { { 0, 0, -5 }, { 0, 0, 1 } };
                AND_GIVEN("xs: intersections(4:shape, 6:shape)") {
                    Intersections xs { { 4, shape }, { 6, shape } };
                    WHEN("detail: prepareDetail(i, r, xs)") {
                        IntersectionDetail detail = Intersection::fillDetail(xs[0], r, xs);
                        AND_WHEN("c: refractedColor(w, detail, 5)") {
                            Color c = Light::refracted(w, detail, 5);
                            THEN("c = black") {
                                REQUIRE(c == Color::black());
                            }
                        }
                    }
                }
            }
        }
    }
}

SCENARIO("Refracted color at maximum recursion") {
    GIVEN("w: default_world()") {
        World w = World::defaultWorld();
        AND_GIVEN("shape: the first shape in w") {
            Geo* shape = w.objects[0];
            AND_GIVEN("shape has material.transparency: 1 and material.refractiveIndex: 1.5") {
                shape->material.transparency = 1;
                shape->material.refractiveIndex = 1.5;
                AND_GIVEN("r: ray( point(0, 0, -5), vector(0, 0, 1) )") {
                    Ray r { { 0, 0, -5 }, { 0, 0, 1 } };
                    AND_GIVEN("xs: intersections(4:shape, 6:shape)") {
                        Intersections xs { { 4, shape }, { 6, shape } };
                        WHEN("detail: prepareDetail(i, r, xs)") {
                            IntersectionDetail detail = Intersection::fillDetail(xs[0], r, xs);
                            AND_WHEN("c: refractedColor(w, detail, 0)") {
                                Color c = Light::refracted(w, detail, 0);
                                THEN("c = black") {
                                    REQUIRE(c == Color::black());
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

SCENARIO("Refracted color under total internal reflection") {
    GIVEN("w: default_world()") {
        World w = World::defaultWorld();
        AND_GIVEN("shape: the first shape in w") {
            Geo* shape = w.objects[0];
            AND_GIVEN("shape has material.transparency: 1 and material.refractiveIndex: 1.5") {
                shape->material.transparency = 1;
                shape->material.refractiveIndex = 1.5;
                AND_GIVEN("r: ray( point(0, 0, sqrt(2) / 2), vector(0, 1, 0) )") {
                    Ray r { { 0, 0, std::sqrt(2) / 2 }, { 0, 1, 0 } };
                    AND_GIVEN("xs: intersections(-sqrt(2)/2:shape, sqrt(2)/2:shape)") {
                        Intersections xs { { -std::sqrt(2)/2, shape }, { std::sqrt(2)/2, shape } };
                        WHEN("detail: prepareDetail(i, r, xs)") {
                            IntersectionDetail detail = Intersection::fillDetail(xs[1], r, xs);
                            AND_WHEN("c: refractedColor(w, detail, 5)") {
                                Color c = Light::refracted(w, detail, 5);
                                THEN("c = black") {
                                    REQUIRE(c == Color::black());
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

SCENARIO("Refracted color under normal conditions") {
    GIVEN("w: default_world()") {
        World w = World::defaultWorld();
        AND_GIVEN("A: the first shape in w") {
            Geo* A = w.objects[0];
            AND_GIVEN("A has ambient: 1 and pattern: Debug") {
                A->material.ambient = 1;
                A->material.pattern = new Pattern::Debug();
                AND_GIVEN("B: the second shape in w") {
                    Geo* B = w.objects[1];
                    AND_GIVEN("B has material.transparency: 1 and material.refractiveIndex: 1.5") {
                        B->material.transparency = 1;
                        B->material.refractiveIndex = 1.5;
                        AND_GIVEN("r: ray( point(0, 0, 0.1), vector(0, 1, 0) )") {
                            Ray r { { 0, 0, 0.1 }, { 0, 1, 0 } };
                            AND_GIVEN("xs: intersections(-0.9899:A -0.4899:B, 0.4899:B, 0.9899:A)") {
                                Intersections xs { { -0.9899, A }, { -0.4899, B }, { 0.4899, B }, { 0.9899, A } };
                                WHEN("detail: prepareDetail(i, r, xs)") {
                                    IntersectionDetail detail = Intersection::fillDetail(xs[2], r, xs);
                                    AND_WHEN("c: refractedColor(w, detail, 5)") {
                                        Color c = Light::refracted(w, detail, 5);
                                        THEN("c = color(0, 0.99888, 0.04725") {
                                            REQUIRE(c == Color(0, 0.99888, 0.04725));
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

SCENARIO("Shade hit with transparency") {
    GIVEN("w: default_world()") {
        World w = World::defaultWorld();
        AND_GIVEN("floor: plane() with transform: translation(0, -1, 0) and transparency: 0.5 and refractiveIndex: 1.5") {
            Plane floor;
            floor.transform = Matrix::translation(0, -1, 0);
            floor.material.refractiveIndex = 1.5;
            AND_GIVEN("floor is added to w") {
                w.objects.emplace_back(&floor);
                AND_GIVEN("ball: sphere() with color: (1, 0, 0) and ambient: 0.5 and transform: translation(0, -3.5, -0.5)") {
                    Sphere ball;
                    ball.transform = Matrix::translation(0, -3.5, -0.5);
                    ball.material.color = { 1, 0, 0 };
                    ball.material.ambient = 0.5;
                    AND_GIVEN("ball is added to w") {
                        w.objects.emplace_back(&ball);
                        AND_GIVEN("r: ray( point(0, 0, -3), vector(0, -sqrt(2)/2, sqrt(2)/2) )") {
                            Ray r { { 0, 0, -3 }, { 0, -std::sqrt(2) / 2, std::sqrt(2) / 2 } };
                            AND_GIVEN("xs: intersections(sqrt(2):floor)") {
                                Intersections xs { { std::sqrt(2), &floor } };
                                WHEN("detail: prepareDetail(i, r, xs)") {
                                    IntersectionDetail detail = Intersection::fillDetail(xs[0], r, xs);
                                    AND_WHEN("c: refractedColor(w, detail, 5)") {
                                        Color c = Light::refracted(w, detail, 5);
                                        THEN("c = color(0.93642, 0.68642, 0.68642)") {
                                            REQUIRE(c == Color(0.93642, 0.68642, 0.68642));
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