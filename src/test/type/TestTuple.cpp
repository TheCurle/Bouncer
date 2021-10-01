/***************
 * THECURLE    *
 *     BOUNCER *
 ***************/

#include <catch2/catch_test_macros.hpp>
#include <type/Tuple.hpp>

SCENARIO("A tuple with w = 1 is a point.", "[Tuple]") {

    GIVEN("a <- tuple(4, -3, 2, 1") {
        Tuple a(4, -3, 2, 1);

        THEN("a.x = 4") {
            REQUIRE(a.x == 4);
        }

        AND_THEN("a.y = -3") {
            REQUIRE(a.y == -3);
        }

        AND_THEN("a.z == 2") {
            REQUIRE(a.z == 2);
        }

        AND_THEN("a.w == 1") {
            REQUIRE(a.w == 1);
        }

        AND_THEN("a is a point") {
            REQUIRE(a.isPoint());
        }

        AND_THEN("a is not a vector") {
            REQUIRE(!a.isVector());
        }
    }
}

SCENARIO("A tuple with w = 0 is a vector.", "[Tuple]") {

    GIVEN("a <- tuple(4, -3, 2, 0") {
        Tuple a(4, -3, 2, 0);

        THEN("a.x = 4") {
            REQUIRE(a.x == 4);
        }

        AND_THEN("a.y = -3") {
            REQUIRE(a.y == -3);
        }

        AND_THEN("a.z == 2") {
            REQUIRE(a.z == 2);
        }

        AND_THEN("a.w == 0") {
            REQUIRE(a.w == 0);
        }

        AND_THEN("a is not a point") {
            REQUIRE(!a.isPoint());
        }

        AND_THEN("a is a vector") {
            REQUIRE(a.isVector());
        }
    }
}

SCENARIO("point() creates tuples with w = 1", "[Tuple]") {

    GIVEN("a <- point(4, -4, 1)") {
        Point a(4, -4, 1);

        THEN("a = tuple(4, -4, 3, 1)") {
            Tuple b(4, -4, 1, 1);

            REQUIRE((a == b));
        }
    }
}

SCENARIO("vector() creates tuples with w = 0", "[Tuple]") {

    GIVEN("a <- vector(4, -4, 1)") {
        Vector a(4, -4, 1);

        THEN("a = tuple(4, -4, 1, 0)") {
            Tuple b(4, -4, 1, 0);

            REQUIRE((a == b));
        }
    }
}

SCENARIO("Adding two tuples", "[Tuple]") {
    
    GIVEN("a <- tuple(3, -2, 5, 1)") {
        Tuple a(3, -2, 5, 1);

        AND_GIVEN("b <- tuple(-2, 3, 1, 0)") {
            Tuple b(-2, 3, 1, 0);

            THEN("a + b = tuple(1, 1, 6, 1)") {
                Tuple c = a + b;
                Tuple c2(1, 1, 6, 1);

                REQUIRE(c == c2);
                REQUIRE(c.isPoint());
                REQUIRE(!c.isVector());
            }
        }
    }
}

SCENARIO("Subtracting two tuples", "[Tuple]") {

    GIVEN("a <- point(3, 2, 1)") {
        Point a(3, 2, 1);

        AND_GIVEN("b <- point(5, 6, 7)") {
            Point b(5, 6, 7);

            THEN("a - b = vector(-2, -4, -6)") {
                Vector c(-2, -4, -6);

                REQUIRE(((a - b) == c));
            }
        }
    }
}

SCENARIO("Subtracting a vector from a point", "[Tuple]") {

    GIVEN("p <- point(3, 2, 1)") {
        Point p(3, 2, 1);

        AND_GIVEN("v <- vector(5, 6, 7)") {
            Vector v(5, 6, 7);

            THEN("p - v = point(-2, -4, -6)") {
                Point a(-2, -4, -6);

                Point res = p - v;

                REQUIRE((res == a));
            }
        }
    }
}

SCENARIO("Subtracting two vectors", "[Tuple]") {

    GIVEN("a <- vector(3, 2, 1)") {
        Vector a(3, 2, 1);

        AND_GIVEN("b <- vector(5, 6, 7)") {
            Vector b(5, 6, 7);

            THEN("a - b = vector(-2, -4, -6)") {
                Vector c(-2, -4, -6);

                REQUIRE(((a - b) == c));
            }
        }
    }
}

SCENARIO("Subtracting a vector from the zero vector", "[Tuple]") {

    GIVEN("zero <- vector(0, 0, 0)") {
        Vector zero(0, 0, 0);

        AND_GIVEN("v <- vector(1, -2, 3)") {
            Vector v(1, -2, 3);

            THEN("zero - v = vector(-1, 2, -3)") {
                Vector res(-1, 2, -3);

                REQUIRE(((zero - v) == res));
            }
        }
    }
}

SCENARIO("Negating a tuple", "[Tuple]") {

    GIVEN("a <- tuple(1, 2, 3, 4)") {
        Tuple a(1, 2, 3, 4);

        THEN("-a = tuple(-1, -2, -3, -4)") {
            Tuple b(-1, -2, -3, -4);

            REQUIRE((-a == b));
        }
    }
}

SCENARIO("Multiplying a tuple by a scalar", "[Tuple]") {
    GIVEN("a <- tuple(1, -2, 3, -4)") { 
        Tuple a(1, -2, 3, -4);

        THEN("a * 3.5 = tuple(3.5, -7, 10.5, -14)") {
            Tuple res(3.5, -7, 10.5, -14);

            REQUIRE((a * 3.5 == res));
        }
    }
}

SCENARIO("Multiplying a tuple by a fraction", "[Tuple]") {
    GIVEN("a <- tuple(1, -2, 3, -4)") { 
        Tuple a(1, -2, 3, -4);

        THEN("a * 0.5 = tuple(0.5, -1, 1.5, -2)") {
            Tuple res(0.5, -1, 1.5, -2);

            REQUIRE((a * 0.5 == res));
        }
    }
}

SCENARIO("Dividing a tuple by a scalar", "[Tuple]") {
    GIVEN("a <- tuple(1, -2, 3, -4)") { 
        Tuple a(1, -2, 3, -4);

        THEN("a / 2= tuple(0.5, -1, 1.5, -2)") {
            Tuple res(0.5, -1, 1.5, -2);

            REQUIRE((a / 2 == res));
        }
    }
}