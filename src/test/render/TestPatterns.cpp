/***************
 * THECURLE    *
 *     BOUNCER *
 ***************/

#include <catch2/catch_test_macros.hpp>
#include "render/Geometry.h"
#include "render/Patterns.h"

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
        Pattern::Stripe pattern(Color::white(), Color::black());

        THEN("stripe_at(pattern, point(0, 0, 0)) = white") {
            REQUIRE(pattern.at({ 0, 0, 0 }) == Color::white());
        }

        AND_THEN("stripe_at(pattern, point(0, 0, 1)) = white") {
            REQUIRE(pattern.at({ 0, 0, 1 }) == Color::white());
        }

        AND_THEN("stripe_at(pattern, point(0, 0, 2)) = white") {
            REQUIRE(pattern.at({ 0, 0, 2 }) == Color::white());
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

        AND_THEN("stripe_at(pattern, point(-0.1, 0, 0)) = black") {
            REQUIRE(pattern.at({ -0.1, 0, 0 }) == Color::black());
        }

        AND_THEN("stripe_at(pattern, point(-1, 0, 0)) = black") {
            REQUIRE(pattern.at({ -1, 0, 0 }) == Color::black());
        }

        AND_THEN("stripe_at(pattern, point(-1.1, 0, 0)) = black") {
            REQUIRE(pattern.at({ -1.1, 0, 0 }) == Color::black());
        }

    }
}