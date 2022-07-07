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
    // Due to the One Definition Rule, this function is defined in Geometry.h
    Color colorAt(const Point& point, Geo* object);

    // Two-color pattern template.
    // Can be transformed to scale and move the pattern origin.
    struct Pattern {
        Color a;
        Color b;
        Matrix transform;
        Matrix inverseTransform;

        // Default colors are black and white.
        Pattern() : a(Color::white()), b(Color::black()), transform(Matrix::identity()), inverseTransform(Matrix::identity()) {}

        void setTransform(const Matrix& in) {
            transform = in;
            inverseTransform = Matrix::fastInverse(in);
        }

        virtual Color at(const Point& p) {
            (void) p;
            return a;
        }
    };

    // A recurring vertical stripe pattern.
    struct Stripe : public Pattern {
        Stripe(Color primary, Color secondary) {
            a = primary;
            b = secondary;
        }

        // p.x + 1 accounts for -1 being treated the same as 1 and thus creating a double-wide gap in the middle.
        Color at(const Point &p) override {
            return (p.x < 0 ? (int) p.x + 1 : (int) p.x) % 2 == 0 ? a : b;
        }
    };

    // A 3-dimensional checker pattern.
    struct Checker : public Pattern {
        Checker(Color primary, Color secondary) {
            a = primary;
            b = secondary;
        }

        Color at(const Point &p) override {
            return (p.x < 0 ? (int) p.x + 1 : (int) p.x +
                        p.y < 0 ? (int) p.y + 1 : (int) p.y +
                        p.z < 0 ? (int) p.z + 1 : (int) p.z)
                                                             % 2 == 0 ? a : b;
        }
    };

    // Returns the color based on the point of intersection.
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