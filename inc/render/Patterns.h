/***************
 * THECURLE    *
 *     BOUNCER *
 ***************/

#include <catch2/catch_test_macros.hpp>
#include <utility>
#include <type/Matrix.h>
#include "Light.h"
#include "Ray.h"

#pragma once

namespace Pattern {
    // Two-color pattern template.
    struct Pattern {
        Color a;
        Color b;

        // Default colors are black and white.
        Pattern() : a(Color::white()), b(Color::black()) {}

        virtual Color at(const Point& p) {
            (void) p;
            return a;
        }
    };

    struct Stripe : public Pattern {
        Stripe(Color primary, Color secondary) {
            a = primary;
            b = secondary;
        }

        Color at(const Point &p) override {
            return std::fmod(p.x, (double) 2) == 0 ? a : b;
        }
    };
}