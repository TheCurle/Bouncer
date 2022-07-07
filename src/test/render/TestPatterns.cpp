/***************
 * THECURLE    *
 *     BOUNCER *
 ***************/

#include <catch2/catch_test_macros.hpp>
#include <render/Geometry.h>
#include <render/Patterns.h>

SCENARIO("Creating a stripe pattern") {
    GIVEN("pattern: stripe_pattern(white, black)") {
        Pattern::Stripe pattern(Color::white(), Color::black());

        THEN("pattern.a = white") {
            REQUIRE(pattern.a == Color::white());
        }

        AND_THEN("pattern.b = black") {
            REQUIRE(pattern.b == Color::black());
        }
    }
}

SCENARIO("A stripe pattern is constant in y") {
    GIVEN("pattern: stripe_pattern(white, black)") {
        Pattern::Stripe pattern(Color::white(), Color::black());

        THEN("stripe_at(pattern, point(0, 0, 0)) = white") {
            REQUIRE(pattern.at({ 0, 0, 0 }) == Color::white());
        }

        AND_THEN("stripe_at(pattern, point(0, 1, 0)) = white") {
            REQUIRE(pattern.at({ 0, 1, 0 }) == Color::white());
        }

        AND_THEN("stripe_at(pattern, point(0, 2, 0)) = white") {
            REQUIRE(pattern.at({ 0, 2, 0 }) == Color::white());
        }
    }
}

SCENARIO("A stripe pattern is constant in z") {
    GIVEN("pattern: stripe_pattern(white, black)") {
        Pattern::Pattern* pattern;
        pattern = new Pattern::Stripe(Color::white(), Color::black());

        THEN("stripe_at(pattern, point(0, 0, 0)) = white") {
            REQUIRE(pattern->at({ 0, 0, 0 }) == Color::white());
        }

        AND_THEN("stripe_at(pattern, point(0, 0, 1)) = white") {
            REQUIRE(pattern->at({ 0, 0, 1 }) == Color::white());
        }

        AND_THEN("stripe_at(pattern, point(0, 0, 2)) = white") {
            REQUIRE(pattern->at({ 0, 0, 2 }) == Color::white());
        }
    }
}

SCENARIO("A stripe pattern alternates in x") {
    GIVEN("pattern: stripe_pattern(white, black)") {
        Pattern::Stripe pattern(Color::white(), Color::black());

        THEN("stripe_at(pattern, point(0, 0, 0)) = white") {
            REQUIRE(pattern.at({ 0, 0, 0 }) == Color::white());
        }

        AND_THEN("stripe_at(pattern, point(0.9, 0, 0)) = white") {
            REQUIRE(pattern.at({ 0.9, 0, 0 }) == Color::white());
        }

        AND_THEN("stripe_at(pattern, point(1, 0, 0)) = black") {
            REQUIRE(pattern.at({ 1, 0, 0 }) == Color::black());
        }

        AND_THEN("stripe_at(pattern, point(1.9, 0, 0)) = black") {
            REQUIRE(pattern.at({ 1.9, 0, 0 }) == Color::black());
        }

        AND_THEN("stripe_at(pattern, point(-0.2, 0, 0)) = black") {
            REQUIRE(pattern.at({ -0.2, 0, 0 }) == Color::black());
        }

        AND_THEN("stripe_at(pattern, point(-1, 0, 0)) = white") {
            REQUIRE(pattern.at({ -1, 0, 0 }) == Color::white());
        }

        AND_THEN("stripe_at(pattern, point(-1.1, 0, 0)) = white") {
            REQUIRE(pattern.at({ -1.1, 0, 0 }) == Color::white());
        }

    }
}

SCENARIO("Stripe with an object transformation") {
    GIVEN("object: sphere()") {
        Sphere object;
        AND_GIVEN("object.transform: scaling(2, 2, 2)") {
            object.setMatrix(Matrix::scaling(2, 2, 2));
            AND_GIVEN("pattern: stripe_pattern(white, black)") {
                Pattern::Stripe pattern(Color::white(), Color::black());
                object.material.pattern = &pattern;
                WHEN("c: colorAt(point(1.5, 0, 0), object)") {
                    Color c = Pattern::colorAt({ 1.5, 0, 0 }, &(Geo&)object);
                    THEN("c = white") {
                        REQUIRE(c == Color::white());
                    }
                }
            }
        }
    }
}

SCENARIO("Stripe with a pattern transformation") {
    GIVEN("object: sphere()") {
        Sphere object;
        AND_GIVEN("pattern: stripe_pattern(white, black)") {
            Pattern::Stripe pattern(Color::white(), Color::black());
            AND_GIVEN("pattern.transform: scaling(2, 2, 2)") {
                pattern.setTransform(Matrix::scaling(2, 2, 2));
                object.material.pattern = &pattern;
                WHEN("c: colorAt(point(1.5, 0, 0), object)") {
                    Color c = Pattern::colorAt({ 1.5, 0, 0 }, &(Geo&)object);
                    THEN("c = white") {
                        REQUIRE(c == Color::white());
                    }
                }
            }
        }
    }
}


SCENARIO("Stripe with all transformations") {
    GIVEN("object: sphere()") {
        Sphere object;
        AND_GIVEN("pattern: stripe_pattern(white, black)") {
            Pattern::Stripe pattern(Color::white(), Color::black());
            AND_GIVEN("object.transform: scaling(2, 2, 2)") {
                object.transform = Matrix::scaling(2, 2, 2);
                AND_GIVEN("pattern.transform: translation(0.5, 0, 0)") {
                    pattern.transform = Matrix::scaling(0.5, 0, 0);
                    object.material.pattern = &pattern;
                    WHEN("c: colorAt(point(2.5, 0, 0), object)") {
                        Color c = Pattern::colorAt({2.5, 0, 0}, &(Geo &) object);
                        THEN("c = white") {
                            REQUIRE(c == Color::white());
                        }
                    }
                }
            }
        }
    }
}