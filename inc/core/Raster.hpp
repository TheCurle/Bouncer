/***************
 * THECURLE    *
 *     BOUNCER *
 ***************/

#include <core/Tuple.hpp>
#include <vector>
#include <string>
#include <algorithm>
#include "stb_image_write.h"

#pragma once

inline size_t clamp(int val) {
    if(val < 0)
        val = 0;
    if(val > 255)
        val = 255;
    return val;
}

/**
 * Represents a single R, G, B color.
 * 
 * Provides some basic mathematical operations to work with colors.
 */
class Color : public TupleContainer {
public:
    // Value constructor
    Color(double red, double green, double blue) : TupleContainer() {
        x = red; y = green; z = blue; w = 2;
    }

    static Color black() {
        static Color black = { 0, 0, 0 };
        return black;
    }

    static Color white() {
        static Color white = { 1, 1, 1 };
        return white;
    }

    // Return the red component
    double red() { return x; }

    // Return the green component
    double green() { return y; }

    // Return the blue compoent
    double blue() { return z; }

    // Fetch the specified [red, green, blue] index.
    double value(size_t idx) {
        return idx == 0 ? red() : 
                idx == 1 ? green() :
                 idx == 2 ? blue() :
                    0;
    }

    uint32_t pack() {
        uint8_t red = clamp((int) std::round(x * 255));
        uint8_t green = clamp((int) std::round(y * 255));
        uint8_t blue = clamp((int) std::round(z * 255));
        return (uint32_t) (red | (green << 8) | (blue << 16) | (255 << 24));
    }

    // Color addition operator.
    Color operator+(const Color& other) {
        return {x + other.x, y + other.y, z + other.z};
    }

    // Color subtraction operator.
    Color operator-(const Color& other) {
        return {x - other.x, y - other.y, z - other.z};
    }

    // Color scalar multiplication operator.
    Color operator*(const double& other) {
        return {(x * other) > 1 ? 1 : (x * other), (y * other) > 1 ? 1 : (y * other), (z * other) > 1 ? 1 : (z * other)};
    }

    // Shur product operator.
    Color operator*(const Color& other) {
        return { (x * other.x) > 1 ? 1 : (x * other.x), (y * other.y) > 1 ? 1 : (y * other.y), (z * other.z) > 1 ? 1 : (z * other.z) };
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
        // Assign the buffer array to a 2-dimensional array of black.
        buffer = std::make_unique<uint32_t[]>(w * h);
        memset(buffer.get(), 0, w * h);
    }

    // Get the Color of the specified pixel.
    uint32_t at(size_t x, size_t y) {
        if (x >= width || y >= height)
            return 0;

        return buffer[y * width + x];
    }

    // Set the Color of the specified pixel.
    void set(size_t x, size_t y, Color& col) {
        if (x >= width)
            return;
        if (y >= height)
            return;

        buffer[y * width + x] = col.pack();
    }

    void export_png(const std::string& fileName) {
        stbi_write_png(fileName.c_str(), width, height, 4, buffer.get(), width * 4);
    }

private:
    // Internal backing storage for the framebuffer's pixel grid.
    std::unique_ptr<uint32_t[]> buffer;
};


namespace Catch {
    template < >
    struct StringMaker<Color> {
        static std::string convert(Color const &p) {
            std::stringstream buf;
            buf << "Color (" << p.x << ", " << p.y << ", " << p.z << ")";
            return buf.str();
        }
    };
}