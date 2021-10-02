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
};