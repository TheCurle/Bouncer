/***************
 * THECURLE    *
 *     BOUNCER *
 ***************/

#include <catch2/catch_test_macros.hpp>
#include <utility>
#include <core/Matrix.h>
#include <render/Light.h>

#pragma once
struct Geo;

namespace Pattern {
    Color colorAt(const Point& point, Geo* object);

    // Two-color pattern template.
    struct Pattern {
        Color a;
        Color b;
        Matrix transform;
        Matrix inverseTransform;

        // Default colors are black and white.
        Pattern() : a(Color::white()), b(Color::black()), transform(Matrix::identity()), inverseTransform(Matrix::identity()) {}

        virtual Color at(const Point& p) {
            (void) p;
            return a;
        }

        void setTransform(const Matrix& in) {
            transform = in;
            inverseTransform = Matrix::fastInverse(in);
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