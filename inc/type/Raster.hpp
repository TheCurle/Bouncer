/***************
 * THECURLE    *
 *     BOUNCER *
 ***************/

#include <type/Tuple.hpp>
#include <vector>
#include <algorithm>

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
        buffer.assign(w, std::vector<Color>(h, Color(0, 0, 0)));
    }

    // Get the Color of the specified pixel.
    Color at(size_t x, size_t y) {
        assert(x <= width);
        assert(y <= height);

        return buffer[x][y];
    }

    // Set the Color of the specified pixel.
    void set(size_t x, size_t y, Color col) {
        assert(x <= width);
        assert(y <= height);

        buffer[x][y] = col;
    }

    // Export the Framebuffer to a Portable PixMap formatted string, ready for writing to disk.
    std::string export_ppm() {
        std::string ppm;
        // PPM header. Version 3, width height, pixel limit.
        ppm.append("P3\n");
        ppm.append(std::to_string(width)).append(" ");
        ppm.append(std::to_string(height)).append("\n");
        ppm.append(std::to_string(export_pixel_limit)).append("\n");

        size_t lineLength = 0;
        for(size_t i = 0; i < height; i++) {
            for(size_t j = 0; j < width; j++) {
                Color col = at(j, i);
                std::string red(std::to_string(clamp((int) std::round(col.red() * export_pixel_limit))));
                red.append(" ");
                
                std::string green(std::to_string(clamp((int) std::round(col.green() * export_pixel_limit))));
                green.append(" ");
                
                std::string blue(std::to_string(clamp((int) std::round(col.blue() * export_pixel_limit))));
                blue.append(" ");

                size_t newSize = red.size() + green.size() + blue.size();

                lineLength += newSize;
                if(lineLength >= 70) {
                    ppm.append("\n/");
                }

                ppm.append(red).append(green).append(blue);
            }

            ppm = ppm.substr(0, ppm.size() - 1);
            lineLength = 0;
            ppm.append("\n");
        }
        return ppm;
    }   


private:
    // Internal backing storage for the framebuffer's pixel grid.
    std::vector<std::vector<Color>> buffer;

    // The maximum value of an exported pixel (currently only used for PPM export)
    size_t export_pixel_limit = 255;

    size_t clamp(int val) {
        if(val < 0)
            val = 0;
        if(val > 255)
            val = 255;
        return val;
    }
};