/***************
 * THECURLE    *
 *     BOUNCER *
 ***************/

#include <catch2/catch_test_macros.hpp>
#include <utility>
#include <type/Matrix.h>
#include "Light.h"

#pragma once
struct Geo;

namespace Pattern {
    Color colorAt(const Point& point, Geo* object);

    // Two-color pattern template.
    struct Pattern {
        Color a;
        Color b;
        Matrix transform;

        // Default colors are black and white.
        Pattern() : a(Color::white()), b(Color::black()), transform(Matrix::identity()) {}

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
            return (int) p.x % 2 == 0 ? a : b;
        }
    };
}