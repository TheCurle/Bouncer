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