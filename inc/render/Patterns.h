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
            return std::abs((int) p.x) % 2 == 0 ? a : b;
        }
    };

    struct Checker : public Pattern {
        Checker(Color primary, Color secondary) {
            a = primary;
            b = secondary;
        }

        Color at(const Point &p) override {
            return ((int) p.x + (int) p.y + (int) p.z) % 2 == 0 ? a : b;
        }
    };

    struct Debug : public Pattern {
        Debug() {
            a = { 1, 0, 0 };
            b = { 0, 0, 1 };
        }

        Color at(const Point &p) override {
            return { p.x, p.y, p.z };
        }
    };
}