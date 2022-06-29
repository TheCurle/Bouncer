/***************
 * THECURLE    *
 *     BOUNCER *
 ***************/

#pragma once

#include <catch2/catch_test_macros.hpp>
#include <type/Matrix.h>
#include "type/Raster.hpp"

// Represents a single point emitting light of a given brightness.
struct PointLight {
    Point position;
    Color intensity;
};

// Material encodes light response data.
struct Material {
    Color color;
    double ambient;
    double diffuse;
    double specular;
    double shininess;

    Material() : color({ 1, 1, 1}), ambient(0.1), diffuse(0.9), specular(0.9), shininess(200.0) {}

    bool operator==(const Material& other) const {
        return color == other.color &&
                ambient == other.ambient &&
                diffuse == other.diffuse &&
                specular == other.specular &&
                shininess == other.shininess;
    }
};