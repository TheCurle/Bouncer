/***************
 * THECURLE    *
 *     BOUNCER *
 ***************/

#include <type/Tuple.hpp>
#include <vector>

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

    // Shur product operator.
    Color operator*(const Color& other) {
        return Color(x * other.x, y * other.y, z * other.z);
    }
};

/**
 * Represents a pixel array that can be drawn to at will.
 * 
 * Contains several export functions for writing to disk as various image formats.
 */
class Framebuffer {
public:
    size_t width;
    size_t height;

    // Value constructor.
    Framebuffer(size_t w, size_t h) : width(w), height(h) {
        // Assign the buffer array to a 2-dimensional array of white.
        buffer.assign(height, std::vector<Color>(width, Color(0, 0, 0)));
    }

    // Get the Color of the specified pixel.
    Color at(size_t x, size_t y) {
        assert(x <= width);
        assert(y <= height);

        return buffer[y][x];
    }

    // Set the Color of the specified pixel.
    void set(size_t x, size_t y, Color col) {
        assert(x <= width);
        assert(y <= height);

        buffer[y][x] = col;
    }


private:
    // Internal backing storage for the framebuffer's pixel grid.
    std::vector<std::vector<Color>> buffer;
};