/***************
 * THECURLE    *
 *     BOUNCER *
 ***************/

#include <type/Tuple.hpp>
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
        buffer.assign(w, std::vector<Color>(h, Black));
    }

    // Get the Color of the specified pixel.
    Color at(size_t x, size_t y) {
        if (x >= width || y >= height)
            return Black;

        return buffer[x][y];
    }

    // Set the Color of the specified pixel.
    void set(size_t x, size_t y, Color col) {
        if (x >= width)
            return;
        if (y >= height)
            return;

        buffer[x][y] = col;
    }

    void export_png(const std::string& fileName) {
        uint32_t pixelBuf[width * height];
        for (size_t i = 0; i < height; i++) {
            for (size_t j = 0; j < width; j++) {
                Color col = at(j, i);
                pixelBuf[i * width + j] = col.pack();
            }
        }

        stbi_write_png(fileName.c_str(), width, height, 4, pixelBuf, width * 4);
    }

    // Export the Framebuffer to a Portable PixMap formatted string, ready for writing to disk.
    std::string export_ppm() {
        std::string ppm;
        // PPM header. Version 3, width height, pixel limit.
        ppm.append("P3\n");
        ppm.append(std::to_string(width)).append(" ");
        ppm.append(std::to_string(height)).append("\n");
        ppm.append(std::to_string(export_pixel_limit)).append("\n");

        for(size_t i = 0; i < height; i++) {
            // Line length counter. Must never exceed 70.
            size_t lineLength = 0;
            for(size_t j = 0; j < width; j++) {
                // Retrieve the pixel we want
                Color col = at(j, i);

                // For every color red, green, blue
                for(size_t k = 0; k < 3; k++) {
                    // Get the value
                    std::string pixel(std::to_string(clamp((int) std::round(col.value(k) * export_pixel_limit))));
                    lineLength += pixel.length() + 1;

                    // Make sure we don't run over length
                    if (lineLength >= 70) {
                        // If we do, remove the last character (which is always a space)
                        ppm = ppm.substr(0, ppm.size() - 1);
                        // Put a newline before this string
                        pixel = std::string("\n").append(pixel);
                        // Reset the line counter
                        lineLength = 0;
                    }

                    pixel.append(" ");
                    ppm.append(pixel);
                }
            }

            // Remove the trailing space.
            ppm = ppm.substr(0, ppm.size() - 1);
            // Move to the next line. Make sure the file always ends in newline, so no checks.
            ppm.append("\n");
        }
        return ppm;
    }   


private:
    // Internal backing storage for the framebuffer's pixel grid.
    std::vector<std::vector<Color>> buffer;

    // The maximum value of an exported pixel (currently only used for PPM export)
    size_t export_pixel_limit = 255;

    // The default Black color.
    Color Black = Color(0, 0, 0);
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