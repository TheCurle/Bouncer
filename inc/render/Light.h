/***************
 * THECURLE    *
 *     BOUNCER *
 ***************/

#pragma once

#include <catch2/catch_test_macros.hpp>
#include <core/Matrix.h>
#include <core/Raster.hpp>
#include <render/Patterns.h>

namespace Light {

    // The types of lights that can be represented
    enum Type {
        NONE,
        POINT,
        DIRECTION,
        AMBIENT
    };

    // Represents a single point emitting light of a given type, with the given brightness and color.
    struct Light {
        Type type;
        Point position;
        Color intensity;

        Light(const Type ty, const Point &pos, Color intens) : type(ty), position(pos), intensity(intens) { }

        Light(const Light &light) : type(light.type), position(light.position), intensity(light.intensity) { }

        Light() : type(NONE), position({ 0, 0, 0 }), intensity({ 0, 0, 0}) {}

        Light &operator =(const Light &other) {
            type = other.type;
            position = other.position;
            intensity = other.intensity;

            return *this;
        }

        bool operator ==(const Light &other) const {
            return type == other.type && position == other.position && intensity == other.intensity;
        }
    };

    // Material encodes light response data.
    struct Material {
        Color color;
        Pattern::Pattern* pattern;
        double ambient;
        double diffuse;
        double specular;
        double shininess;
        double reflectivity;
        double transparency;
        double refractiveIndex;

        Material() : color({1, 1, 1}), pattern(nullptr), ambient(0.1), diffuse(0.9), specular(0.9), shininess(200.0),
                     reflectivity(0), transparency(0), refractiveIndex(1) { }

        Material(Color col, double amb, double diff, double spec, double shin, double reflec, double trans, double refr)
                : color(col), pattern(nullptr), ambient(amb), diffuse(diff), specular(spec), shininess(shin),
                  reflectivity(reflec), transparency(trans), refractiveIndex(refr) { }

        bool operator ==(const Material &other) const {
            return color == other.color &&
                   ambient == other.ambient &&
                   diffuse == other.diffuse &&
                   specular == other.specular &&
                   shininess == other.shininess;
        }
    };

}