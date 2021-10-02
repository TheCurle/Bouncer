/***************
 * THECURLE    *
 *     BOUNCER *
 ***************/

#include <type/Tuple.hpp>

#pragma once

/**
 * Represents a single R, G, B color.
 * 
 * Provides some basic mathematical operations to work with colors.
 */
class Color : public TupleContainer {
public:
    // Value constructor
    Color(double red, double green, double blue) {
        x = red; y = green; z = blue; w = 2;
    }

    // Return the red component
    double red() { return x; }

    // Return the green component
    double green() { return y; }

    // Return the blue compoent
    double blue() { return z; }

    // Color addition operator.
    Color operator+(const Color& other) {
        return Color(x + other.x, y + other.y, z + other.z);
    }

    // Color subtraction operator.
    Color operator-(const Color& other) {
        return Color(x - other.x, y - other.y, z - other.z);
    }

    // Color scalar multiplication operator.
    Color operator*(const double& other) {
        return Color(x * other, y * other, z * other);
    }

    Color operator*(const Color& other) {
        return Color(x * other.x, y * other.y, z * other.z);
    }
};