/***************
 * THECURLE    *
 *     BOUNCER *
 ***************/

#include <catch2/catch_test_macros.hpp>
#include <core/Matrix.h>
#include <render/Ray.h>
#include <render/Geometry.h>

using namespace RT;

SCENARIO("Multiplying by a translation matrix") {
    GIVEN("transform: translation(5, -3, 2)") {
        Matrix transform = Matrix::translation(5, -3, 2);

        AND_GIVEN("p: point(-3, 4, 5)") {
            Point p(-3, 4, 5);

            THEN("transform * p = point(2, 1, 7)") {
                Point result = Point(transform * p);
                REQUIRE(result == Point(2, 1, 7));
            }
        }
    }
}

SCENARIO("Multiplying by the inverse of a translation matrix") {
    GIVEN("transform: translation(5, -3, 2)") {
        Matrix transform = Matrix::translation(5, -3, 2);

        AND_GIVEN("inv: inverse(transform)") {
            Matrix inv = Matrix::inverse(transform);

            AND_GIVEN("p: point(-3, 4, 5)") {
                Point p(-3, 4, 5);

                THEN("inv * p = point(-8, 7, 3)") {
                    Point result = Point(inv* p);

                    REQUIRE(result == Point(-8, 7, 3));
                }
            }
        }
    }
}

SCENARIO("Translation does not affect vectors") {
    GIVEN("transform: translation(5, -3, 2)") {
        Matrix transform = Matrix::translation(5, -3, 2);

        AND_GIVEN("v: vector(-3, 4, 5)") {
            Vector v(-3, 4, 5);

            THEN("transform * v = v") {
                REQUIRE(Vector(transform * v) == v);
            }
        }
    }
}

SCENARIO("A scaling matrix applied to a point") {
    GIVEN("transform: scale(2, 3, 4)") {
        Matrix transform = Matrix::scaling(2, 3, 4);

        AND_GIVEN("p: point(-4, 6, 8)") {
            Point p(-4, 6, 8);

            THEN("transform * p = point(-8, 18, 32)") {
                Point result = Point(transform * p);
                REQUIRE(result == Point(-8, 18, 32));
            }
        }
    }
}

SCENARIO("Applying a scaling matrix to a vector") {
    GIVEN("transform: scale(2, 3, 4)") {
        Matrix transform = Matrix::scaling(2, 3, 4);

        AND_GIVEN("p: vector(-4, 6, 8)") {
            Vector v(-4, 6, 8);

            THEN("transform * v = vector(-8, 18, 32)") {
                REQUIRE(Vector(transform * v) == Vector(-8, 18, 32));
            }
        }
    }
}

SCENARIO("Multiplying by the inverse of a scaling matrix") {
    GIVEN("transform: scale(2, 3, 4)") {
        Matrix transform = Matrix::scaling(2, 3, 4);

        AND_GIVEN("inv: inverse(transform)") {
            Matrix inv = Matrix::inverse(transform);

            AND_GIVEN("p: point(-4, 6, 8)") {
                Point p(-4, 6, 8);

                THEN("inv * p = point(-2, 2, 2)") {
                    Point result = Point(inv * p);

                    REQUIRE(result == Point(-2, 2, 2));
                }
            }
        }
    }
}

SCENARIO("Reflection is the same as scaling by a negative value") {
    GIVEN("transform: scaling(-1, 1, 1)") {
        Matrix transform = Matrix::scaling(-1, 1, 1);
        AND_GIVEN("p: point(2, 3, 4)") {
            Point p(2, 3, 4);
            THEN("transform * p = Point(-2, 3, 4)") {
                REQUIRE(Point(transform * p) == Point(-2, 3, 4));
            }
        }
    }
}

SCENARIO("Rotating a point around the x axis") {
    GIVEN("p: point(0, 1, 0)") {
        Point p(0, 1, 0);
        AND_GIVEN("half_quarter: rotation_x(pi/4)") {
            Matrix half_quarter = Matrix::rotation_x(M_PI / 4);
            AND_GIVEN("full_quarter: rotation_x(pi/2)") {
                Matrix full_quarter = Matrix::rotation_x(M_PI / 2);

                THEN("half_quarter * p = point(0, sqrt(2) / 2, sqrt(2) / 2)") {
                    REQUIRE(Point(half_quarter * p) == Point(0, std::sqrt(2) / 2, std::sqrt(2) / 2));
                }

                AND_THEN("full_quarter * p = point(0, 0, 1)") {
                    REQUIRE(Point(full_quarter * p) == Point(0, 0, 1));
                }
            }
        }
    }
}

SCENARIO("The inverse of an x-rotation rotates in the opposite direction") {
    GIVEN("p: point(0, 1, 0)") {
        Point p(0, 1, 0);
        AND_GIVEN("half_quarter: rotation_x(pi/4)") {
            Matrix half_quarter = Matrix::rotation_x(M_PI / 4);
            AND_GIVEN("inv: inverse(half_quarter)") {
                Matrix inv = Matrix::inverse(half_quarter);

                THEN("inv * p = point(0, sqrt(2) / 2, -sqrt(2) / 2)") {
                    REQUIRE(Point(inv * p) == Point(0, std::sqrt(2) / 2, -std::sqrt(2) / 2));
                }

            }
        }
    }
}

SCENARIO("Rotating a point around the y axis") {
    GIVEN("p: point(0, 0, 1)") {
        Point p(0, 0, 1);
        AND_GIVEN("half_quarter: rotation_y(pi/4)") {
            Matrix half_quarter = Matrix::rotation_y(M_PI / 4);
            AND_GIVEN("full_quarter: rotation_y(pi/2)") {
                Matrix full_quarter = Matrix::rotation_y(M_PI / 2);

                THEN("half_quarter * p = point(sqrt(2) / 2, 0, sqrt(2) / 2)") {
                    REQUIRE(Point(half_quarter * p) == Point(std::sqrt(2) / 2, 0, std::sqrt(2) / 2));
                }

                AND_THEN("full_quarter * p = point(1, 0, 0)") {
                    REQUIRE(Point(full_quarter * p) == Point(1, 0, 0));
                }
            }
        }
    }
}

SCENARIO("Rotating a point around the z axis") {
    GIVEN("p: point(0, 1, 0)") {
        Point p(0, 1, 0);
        AND_GIVEN("half_quarter: rotation_z(pi/4)") {
            Matrix half_quarter = Matrix::rotation_z(M_PI / 4);
            AND_GIVEN("full_quarter: rotation_z(pi/2)") {
                Matrix full_quarter = Matrix::rotation_z(M_PI / 2);

                THEN("half_quarter * p = point(-sqrt(2) / 2, sqrt(2) / 2), 0") {
                    REQUIRE(Point(half_quarter * p) == Point(-std::sqrt(2) / 2, std::sqrt(2) / 2, 0));
                }

                AND_THEN("full_quarter * p = point(-1, 0, 0)") {
                    REQUIRE(Point(full_quarter * p) == Point(-1, 0, 0));
                }
            }
        }
    }
}

SCENARIO("A shearing transformation moves x in proportion to y") {
    GIVEN("transform: shearing(1, 0, 0, 0, 0, 0)") {
        Matrix transform = Matrix::shearing(1, 0, 0, 0, 0, 0);
        AND_GIVEN("p: point(2, 3, 4)") {
            Point p(2, 3, 4);
            THEN("transform * p = point(5, 3, 4)") {
                REQUIRE(Point(transform * p) == Point(5, 3, 4));
            }
        }
    }
}

SCENARIO("A shearing transformation moves x in proportion to z") {
    GIVEN("transform: shearing(0, 1, 0, 0, 0, 0)") {
        Matrix transform = Matrix::shearing(0, 1, 0, 0, 0, 0);
        AND_GIVEN("p: point(2, 3, 4)") {
            Point p(2, 3, 4);
            THEN("transform * p = point(6, 3, 4)") {
                REQUIRE(Point(transform * p) == Point(6, 3, 4));
            }
        }
    }
}

SCENARIO("A shearing transformation moves y in proportion to x") {
    GIVEN("transform: shearing(0, 0, 1, 0, 0, 0)") {
        Matrix transform = Matrix::shearing(0, 0, 1, 0, 0, 0);
        AND_GIVEN("p: point(2, 3, 4)") {
            Point p(2, 3, 4);
            THEN("transform * p = point(2, 5, 4)") {
                REQUIRE(Point(transform * p) == Point(2, 5, 4));
            }
        }
    }
}

SCENARIO("A shearing transformation moves y in proportion to z") {
    GIVEN("transform: shearing(0, 0, 0, 1, 0, 0)") {
        Matrix transform = Matrix::shearing(0, 0, 0, 1, 0, 0);
        AND_GIVEN("p: point(2, 3, 4)") {
            Point p(2, 3, 4);
            THEN("transform * p = point(2, 7, 4)") {
                REQUIRE(Point(transform * p) == Point(2, 7, 4));
            }
        }
    }
}

SCENARIO("A shearing transformation moves z in proportion to x") {
    GIVEN("transform: shearing(0, 0, 0, 0, 1, 0)") {
        Matrix transform = Matrix::shearing(0, 0, 0, 0, 1, 0);
        AND_GIVEN("p: point(2, 3, 4)") {
            Point p(2, 3, 4);
            THEN("transform * p = point(2, 3, 6)") {
                REQUIRE(Point(transform * p) == Point(2, 3, 6));
            }
        }
    }
}

SCENARIO("A shearing transformation moves z in proportion to y") {
    GIVEN("transform: shearing(0, 0, 0, 0, 0, 1)") {
        Matrix transform = Matrix::shearing(0, 0, 0, 0, 0, 1);
        AND_GIVEN("p: point(2, 3, 4)") {
            Point p(2, 3, 4);
            THEN("transform * p = point(2, 3, 7)") {
                REQUIRE(Point(transform * p) == Point(2, 3, 7));
            }
        }
    }
}

SCENARIO("Individual transformations applied in sequence") {
    GIVEN("p: point(1, 0, 1)") {
        Point p(1, 0, 1);
        AND_GIVEN("A: rotation_x(pi/2)") {
            Matrix A = Matrix::rotation_x(M_PI / 2);
            AND_GIVEN("B: scaling(5, 5, 5)") {
                Matrix B = Matrix::scaling(5, 5, 5);
                AND_GIVEN("C: translation(10, 5, 7)") {
                    Matrix C = Matrix::translation(10, 5, 7);

                    WHEN("p2 = A * p") {
                        Point p2 = Point(A * p);
                        THEN("p2 = point(1, -1, 0)") {
                            REQUIRE(p2 == Point(1, -1, 0));
                        }

                        AND_WHEN("p3 = B * p2") {
                            Point p3 = Point(B * p2);
                            THEN("p3 = point(5, -5, 0)") {
                                REQUIRE(p3 == Point(5, -5, 0));
                            }

                            AND_WHEN("p4 = C * p3") {
                                Point p4 = Point(C * p3);
                                THEN("p4 = point(15, 0, 7)") {
                                    REQUIRE(p4 == Point(15, 0, 7));
                                }
                            }
                        }
                    }

                }
            }
        }
    }
}

SCENARIO("Chained transformations must be applied in reverse order") {
    GIVEN("p: point(1, 0, 1)") {
        Point p(1, 0, 1);
        AND_GIVEN("A: rotation_x(pi/2)") {
            Matrix A = Matrix::rotation_x(M_PI / 2);
            AND_GIVEN("B: scaling(5, 5, 5)") {
                Matrix B = Matrix::scaling(5, 5, 5);
                AND_GIVEN("C: translation(10, 5, 7)") {
                    Matrix C = Matrix::translation(10, 5, 7);

                    WHEN("T: C * B * A") {
                        Matrix T = C * B * A;

                        THEN("T * p = point(15, 0, 7)") {
                            REQUIRE(Point(T * p) == Point(15, 0, 7));
                        }
                    }
                }
            }
        }
    }
}

/**********************************************************************************************************************/
/**********************************************************************************************************************/

SCENARIO("Translating a point") {
    GIVEN("p: point(1, 2, 3)") {
        Point p { 1, 2, 3 };
        AND_GIVEN("m: translation(3, 4, 5)") {
            Matrix m = Matrix::translation(3, 4, 5);
            WHEN("p2  = m * p") {
                Point p2 = Point(m * p);

                THEN("p2 = point(4, 6, 8)") {
                    REQUIRE(p2 == Point(4, 6, 8 ));
                }
            }
        }
    }
}

SCENARIO("Translating a vector") {
    GIVEN("v: vector(0, 1, 0)") {
        Vector v { 0, 1, 0 };
        AND_GIVEN("m: translation(3, 4, 5)") {
            Matrix m = Matrix::translation(3, 4, 5);
            WHEN("v2 = m * v") {
                Vector v2 = Vector(m *v);

                THEN("v2 = vector(0, 1, 0)") {
                    REQUIRE(v2 == Vector(0, 1, 0));
                }
            }
        }
    }
}

/**********************************************************************************************************************/
/**********************************************************************************************************************/

SCENARIO("Translating a ray") {
    GIVEN("r: ray( point(1, 2, 3), vector(0, 1, 0) )") {
        Ray r { { 1, 2, 3 }, { 0, 1, 0 } };
        AND_GIVEN("m: translation(3, 4, 5)") {
            Matrix m = Matrix::translation(3, 4, 5);
            WHEN("r2 = transform(r, m)") {
                Ray r2 = Ray::transform(r, m);

                THEN("r2.origin = point(4, 6, 8)") {
                    REQUIRE(r2.origin == Point(4, 6, 8 ));
                }

                AND_THEN("r2.direction = vector(0, 1, 0)") {
                    REQUIRE(r2.direction == Vector(0, 1, 0));
                }
            }
        }
    }
}

SCENARIO("Scaling a ray") {
    GIVEN("r: ray( point(1, 2, 3), vector(0, 1, 0) )") {
        Ray r { { 1, 2, 3 }, { 0, 1, 0 } };
        AND_GIVEN("m: scaling(2, 3, 4)") {
            Matrix m = Matrix::scaling(2, 3, 4);
            WHEN("r2 = transform(r, m)") {
                Ray r2 = Ray::transform(r, m);

                THEN("r2.origin = point(2, 6, 12)") {
                    REQUIRE(r2.origin == Point(2, 6, 12));
                }

                AND_THEN("r2.direction = vector(0, 3, 0)") {
                    REQUIRE(r2.direction == Vector(0, 3, 0));
                }
            }
        }
    }
}

SCENARIO("A sphere has a default transformation") {
    GIVEN("s: sphere()") {
        Sphere s;
        THEN("s.transform = identity_matrix") {
            REQUIRE(s.transform == Matrix::identity());
        }
    }
}

SCENARIO("Changing a sphere's transformation") {
    GIVEN("s: sphere()") {
        Sphere s;
        AND_GIVEN("t: translation(2, 3, 4)") {
            Matrix t = Matrix::translation(2, 3, 4);
            WHEN("s.setTransform(t)") {
                s.transform = t;

                THEN("s.transform = t") {
                    REQUIRE(s.transform == t);
                }
            }
        }
    }
}

SCENARIO("Intersecting a scaled sphere with a ray") {
    GIVEN("r: ray( point(0, 0, -5), vector(0, 0, 1) )") {
        Ray r { {0, 0, -5}, { 0, 0, 1 } };
        AND_GIVEN("s: sphere()") {
            Sphere s;
            WHEN("s.setTransform( scaling(2, 2, 2) )") {
                s.setMatrix(Matrix::scaling(2, 2, 2));
                AND_WHEN("xs: intersect(s, r)") {
                    std::vector<Intersection> sect;
                    s.intersect(r, sect);
                    Intersections xs ( sect.begin(), sect.end() );

                    THEN("xs.size = 2") {
                        REQUIRE(xs.size == 2);
                    }

                    AND_THEN("xs[0].t = 3") {
                        REQUIRE(xs[0].time == 3);
                    }

                    AND_THEN("xs[1].t = 7") {
                        REQUIRE(xs[1].time == 7);
                    }
                }
            }
        }
    }
}

SCENARIO("Intersecting a translated sphere with a ray") {
    GIVEN("r: ray( point(0, 0, -5), vector(0, 0, 1) )") {
        Ray r { {0, 0, -5}, { 0, 0, 1 } };
        AND_GIVEN("s: sphere()") {
            Sphere s;
            WHEN("s.setTransform( translation(5, 0, 0 )") {
                s.setMatrix(Matrix::translation(5, 0, 0));
                AND_WHEN("xs: intersect(s, r)") {
                    std::vector<Intersection> sect;
                    s.intersect(r, sect);
                    Intersections xs ( sect.begin(), sect.end() );

                    THEN("xs.size = 0") {
                        REQUIRE(xs.size == 0);
                    }
                }
            }
        }
    }
}

