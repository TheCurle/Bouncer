/***************
 * THECURLE    *
 *     BOUNCER *
 ***************/

#include <catch2/catch_test_macros.hpp>
#include <core/Raster.hpp>
#define LIGHT_OPERATOR_OVERLOADS
#include <render/Light.h>
#include <view/World.h>

using namespace RT;

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
                            Color result = Light::lighting(m, new Sphere(m), light, position, eyev, normalv);

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
                            Color result = Light::lighting(m, new Sphere(m), light, position, eyev, normalv);

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
                            Color result = Light::lighting(m, new Sphere(m), light, position, eyev, normalv);

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
                            Color result = Light::lighting(m, new Sphere(m), light, position, eyev, normalv);

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
                            Color result = Light::lighting(m, new Sphere(m), light, position, eyev, normalv);

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

SCENARIO("Shading an intersection with the world") {
    GIVEN("w: default_world()") {
        World w = World::defaultWorld();
        AND_GIVEN("r: ray( point(0, 0, -5), vector(0, 0, 1) )") {
            Ray r { { 0, 0, -5 }, { 0, 0, 1 } };
            AND_GIVEN("shape: first shape in w") {
                Geo* shape = w.objects[0];
                AND_GIVEN("i: intersection(4, shape)") {
                    Intersection i { 4, shape };
                    WHEN("detail: fillDetail(i, r)") {
                        Intersections xs { i };
                        IntersectionDetail detail = Intersection::fillDetail(i, r, xs);
                        AND_WHEN("c: shadeHit(w, detail)") {
                            Color c = Light::shadeHit(w, detail, 1);

                            THEN("c = color( 0.38066, 0.47583, 0.2855 )") {
                                REQUIRE(c == Color(0.38066, 0.47583, 0.2855));
                            }
                        }
                    }
                }
            }
        }
    }
}

SCENARIO("Shading an internal intersection") {
    GIVEN("w: default_world()") {
        World w = World::defaultWorld();
        AND_GIVEN("w.light: point_light( point(0, 0.25, 0), color(1, 1, 1) )") {
            w.lightSource = PointLight { { 0, 0.25, 0 }, { 1, 1, 1 } };
            AND_GIVEN("r: ray( point(0, 0, 0), vector(0, 0, 1) )") {
                Ray r { { 0, 0, 0 }, { 0, 0, 1 } };
                AND_GIVEN("shape: second shape in w") {
                    Geo* shape = w.objects[1];
                    AND_GIVEN("i: intersection(0.5, shape)") {
                        Intersection i { 0.5, shape };
                        WHEN("detail: fillDetail(i, r)") {
                            Intersections xs { i };
                            IntersectionDetail detail = Intersection::fillDetail(i, r, xs);
                            AND_WHEN("c: shadeHit(w, detail)") {
                                Color c = Light::shadeHit(w, detail, 1);

                                THEN("c = color( 0.90498, 0.90498, 0.90498 )") {
                                    REQUIRE(c == Color(0.90498, 0.90498, 0.90498 ));
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

SCENARIO("Lighting a surface in a shadow") {
    GIVEN("m: material()") {
        Material m;
        AND_GIVEN("position: point(0, 0, 0)") {
            Point position {0, 0, 0};
            GIVEN("eyev: vector(0, 0, -1)") {
                Vector eyev {0,
                             0,
                             -1};
                AND_GIVEN("normalv: vector(0, 0, -1)") {
                    Vector normalv {0, 0, -1};
                    AND_GIVEN("light: point_light( point(0, 0, -10), color(1, 1, 1) )") {
                        PointLight light {{0, 0, -10},
                                          {1, 1, 1}};
                        AND_GIVEN("in_shadow: true") {
                            bool inShadow = true;
                            WHEN("result: lighting(m, light, position, eyev, normalv, inShadow)") {
                                Color result = Light::lighting(m, new Sphere(m), light, position, eyev, normalv, inShadow);
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
}

SCENARIO("There is no shadow when point can see the light") {
    GIVEN("w: default_world()") {
        World w = World::defaultWorld();
        AND_GIVEN("p: point(0, 10, 0)") {
            Point p { 0, 10, 0 };
            THEN("isShadowed(w, p) = false") {
                REQUIRE(Light::isInShadow(w, p) == false);
            }
        }
    }
}

SCENARIO("There is a shadow with an object between the point and the light") {
    GIVEN("w: default_world()") {
        World w = World::defaultWorld();
        AND_GIVEN("p: point(10, -10, 10)") {
            Point p { 10, -10, 10 };
            THEN("isShadowed(w, p) = true") {
                REQUIRE(Light::isInShadow(w, p) == true);
            }
        }
    }
}

SCENARIO("There is no shadow with an object behind the light") {
    GIVEN("w: default_world()") {
        World w = World::defaultWorld();
        AND_GIVEN("p: point(-20, 20, -20)") {
            Point p { -20, 20, -20 };
            THEN("isShadowed(w, p) = false") {
                REQUIRE(Light::isInShadow(w, p) == false);
            }
        }
    }
}

SCENARIO("There is no shadow with an object behind the point") {
    GIVEN("w: default_world()") {
        World w = World::defaultWorld();
        AND_GIVEN("p: point(-2, 2, -2)") {
            Point p { -2, 2, -2 };
            THEN("isShadowed(w, p) = false") {
                REQUIRE(Light::isInShadow(w, p) == false);
            }
        }
    }
}

SCENARIO("Shading a hit in the shadow of an object") {
    GIVEN("w: world()") {
        AND_GIVEN("w.light: point_light( point(0, 0, -10), color(1, 1, 1) )") {
            PointLight light { { 0, 0, -10 }, { 1, 1, 1 } };
            AND_GIVEN("s1: sphere()") {
                Sphere s1;
                AND_GIVEN("s1 is added to w") {
                    AND_GIVEN("s2: sphere() with transform: translation(0, 0, 10)") {
                        Sphere s2;
                        s2.transform = Matrix::translation(0, 0, 10);
                        AND_GIVEN("s2 is added to w") {
                            World w { { &s1, &s2 }, light };
                            AND_GIVEN("r: ray( point(0, 0, 5), vector(0, 0, 1) )") {
                                Ray r { { 0, 0, 5 }, { 0, 0, 1 } };
                                AND_GIVEN("i: intersection(4, s2)") {
                                    Intersection i { 4, &s2 };
                                    WHEN("detail: fillDetail(i, r)") {
                                        Intersections xs { i };
                                        IntersectionDetail detail = Intersection::fillDetail(i, r, xs);
                                        AND_WHEN("c: shade_hit(w, detail)") {
                                            Color c = Light::shadeHit(w, detail, 1);

                                            THEN("c: color(0.1, 0.1, 0.1)") {
                                                REQUIRE(c == Color(0.1, 0.1, 0.1));
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

SCENARIO("The hit should offset the point, to avoid ray acne") {
    GIVEN("r: ray( point(0, 0, -5), vector(0, 0, 1) )") {
        Ray r { { 0, 0, -5 }, { 0, 0, 1 } };
        AND_GIVEN("shape: sphere() with transform: translation(0, 0, 1)") {
            Sphere shape;
            shape.setMatrix(Matrix::translation(0, 0, 1));
            AND_GIVEN("i: intersection(5, shape)") {
                Intersection i { 5, &shape };
                WHEN("detail: fillDetail(i, r)") {
                    Intersections xs { i };
                    IntersectionDetail detail = Intersection::fillDetail(i, r, xs);

                    THEN("detail.overPoint.z < -EPSILON / 2") {
                        REQUIRE(detail.overPoint.z < -0.001 / 2);
                    }

                    AND_THEN("detail.point.z > detail.overPoint.z") {
                        REQUIRE(detail.point.z > detail.overPoint.z);
                    }
                }
            }
        }
    }
}

SCENARIO("Reflected color of a nonreflective material") {
    GIVEN("w: default_world()") {
        World w = World::defaultWorld();
        AND_GIVEN("r: ray( point(0, 0, 0), vector(0, 0, 1) )") {
            Ray r { { 0, 0, 0 }, { 0, 0, 1} };
            AND_GIVEN("shape: the second object in w") {
                Geo* shape = w.objects[1];
                AND_GIVEN("shape.material.ambient: 1") {
                    shape->material.ambient = 1;
                    AND_GIVEN("i: intersection(1, shape)") {
                        Intersection i { 1, shape };
                        WHEN("detail: fillDetail(i, r)") {
                            Intersections xs { i };
                            IntersectionDetail detail = Intersection::fillDetail(i, r, xs);
                            AND_WHEN("color: reflected_color(w, detail)") {
                                Color color = Light::reflected(w, detail, 1);

                                THEN("color = black") {
                                    REQUIRE(color == Color::black());
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

SCENARIO("Reflected color of a reflective surface") {
    GIVEN("w: default_world()") {
        World w = World::defaultWorld();
        AND_GIVEN("shape: a plane with material.reflectivity: 0.5 and transform: translation(0, -1, 0)") {
            Plane shape;
            shape.material.reflectivity = 0.5;
            shape.setMatrix(Matrix::translation(0, -1, 0));
            AND_GIVEN("shape is added to w") {
                w.addObjects({ &shape });
                AND_GIVEN("r: ray( point(0, 0, -3), vector(0, -sqrt(2) / 2, sqrt(2) / 2) )") {
                    Ray r {{0, 0,                 -3},
                           {0, -std::sqrt(2) / 2, std::sqrt(2) / 2}};
                    AND_GIVEN("i: intersection(sqrt(2), shape)") {
                        Intersection i {std::sqrt(2), &shape};
                        WHEN("detail: fillDetail(i, r)") {
                            Intersections xs { i };
                            IntersectionDetail detail = Intersection::fillDetail(i, r, xs);
                            AND_WHEN("color: reflected_color(w, detail)") {
                                Color color = Light::reflected(w, detail, 2);

                                THEN("color = color(0.19049, 0.2381, 0.14286)") {
                                    REQUIRE(color == Color(0.19049, 0.2381, 0.14286 ));
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

SCENARIO("Blended color of a reflective surface") {

    GIVEN("w: default_world()") {
        World w = World::defaultWorld();
        AND_GIVEN("shape: a plane with material.reflectivity: 0.5 and transform: translation(0, -1, 0)") {
            Plane shape;
            shape.material.reflectivity = 0.5;
            shape.setMatrix(Matrix::translation(0, -1, 0));
            AND_GIVEN("shape is added to w") {
                w.addObjects({ &shape });
                AND_GIVEN("r: ray( point(0, 0, -3), vector(0, -sqrt(2) / 2, sqrt(2) / 2) )") {
                    Ray r {{0, 0,                 -3},
                           {0, -std::sqrt(2) / 2, std::sqrt(2) / 2}};
                    AND_GIVEN("i: intersection(sqrt(2), shape)") {
                        Intersection i {std::sqrt(2), &shape};
                        WHEN("detail: fillDetail(i, r)") {
                            Intersections xs { i };
                            IntersectionDetail detail = Intersection::fillDetail(i, r, xs);
                            AND_WHEN("color: reflected_color(w, detail)") {
                                Color color = Light::shadeHit(w, detail, 1);

                                THEN("color = color(0.87677, 0.92436, 0.82918)") {
                                    REQUIRE(color == Color(0.87677, 0.92436, 0.82918));
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

SCENARIO("Infinitely recursive reflections") {
    GIVEN("w: world()") {
        World w;
        AND_GIVEN("w.light: point_light( point(0, 0, 0), color(1, 1, 1) )") {
            w.lightSource = { { 0, 0, 0 }, { 1, 1, 1 } };
            AND_GIVEN("lower: plane() with reflectivity: 1 and transform: translation(0, -1, 0)") {
                Plane lower;
                lower.material.reflectivity = 1;
                lower.transform = Matrix::translation(0, -1, 0);
                AND_GIVEN("upper: plane() with reflectivity: 1 and transform: translation(0, 1, 0)") {
                    Plane upper;
                    upper.material.reflectivity = 1;
                    upper.transform = Matrix::translation(0, 1, 0);
                    AND_GIVEN("upper is added to w") {
                        AND_GIVEN("lower is added to w") {
                            w.addObjects({ &lower, &upper });
                            AND_GIVEN("r: ray( point(0, 0, 0), vector(0, 1, 0) )") {
                                Ray r { { 0, 0, 0 }, { 0, 1, 0 } };

                                THEN("colorAt(w, r) should terminate") {

                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

SCENARIO("Reflections at maximum depth") {
    GIVEN("w: default_world()") {
        World w = World::defaultWorld();
        AND_GIVEN("shape: a plane with material.reflectivity: 0.5 and transform: translation(0, -1, 0)") {
            Plane shape;
            shape.material.reflectivity = 0.5;
            shape.transform = Matrix::translation(0, -1, 0);
            AND_GIVEN("shape is added to w") {
                w.addObjects({ &shape });
                AND_GIVEN("r: ray( point(0, 0, -3), vector(0, -sqrt(2) / 2, sqrt(2) / 2) )") {
                    Ray r {{0, 0,                 -3},
                           {0, -std::sqrt(2) / 2, std::sqrt(2) / 2}};
                    AND_GIVEN("i: intersection(sqrt(2), shape)") {
                        Intersection i {std::sqrt(2), &shape};
                        WHEN("detail: fillDetail(i, r)") {
                            Intersections xs { i };
                            IntersectionDetail detail = Intersection::fillDetail(i, r, xs);
                            AND_WHEN("color: reflected_color(w, detail)") {
                                Color color = Light::reflected(w, detail, 0);

                                THEN("color = black)") {
                                    REQUIRE(color == Color::black());
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}