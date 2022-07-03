/***************
 * THECURLE    *
 *     BOUNCER *
 ***************/

#pragma once

#include <catch2/catch_test_macros.hpp>
#include <type/Matrix.h>
#include "type/Raster.hpp"
#include "Patterns.h"

struct World;

// Represents a single point emitting light of a given brightness.
struct PointLight {
    Point position;
    Color intensity;

    PointLight(const Point &pos, Color intens) : position(pos), intensity(intens) {}
    PointLight(const PointLight &light) : position(light.position), intensity(light.intensity) {}

    PointLight& operator=(const PointLight& other) {
        position = other.position;
        intensity = other.intensity;

        return *this;
    }

    bool operator==(const PointLight& other) const {
        return position == other.position && intensity == other.intensity;
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

    Material() : color({ 1, 1, 1}), pattern(nullptr), ambient(0.1), diffuse(0.9), specular(0.9), shininess(200.0) {}
    Material(Color col, double amb, double diff, double spec, double shin) : color(col), pattern(nullptr), ambient(amb), diffuse(diff), specular(spec), shininess(shin) {}

    bool operator==(const Material& other) const {
        return color == other.color &&
                ambient == other.ambient &&
                diffuse == other.diffuse &&
                specular == other.specular &&
                shininess == other.shininess;
    }
};

// A holder for generic lighting calculations
namespace Light {

    // Phong Shading Lighting workhouse.
    inline Color lighting(Material m, PointLight light, const Point& position, const Vector& eyev, const Vector& normalv, bool inShadow) {
        Color materialColor = m.pattern != nullptr ? m.pattern->at(position) : m.color;

        Color effectiveColor = materialColor * light.intensity;
        Vector lightV = Vector((light.position - position).normalize());

        Color ambient = effectiveColor * m.ambient;
        Color diffuse(0, 0, 0);
        Color specular(0, 0, 0);

        double lDotNorm = lightV * normalv;
        if (lDotNorm > 0 && !inShadow) {
            diffuse = effectiveColor * m.diffuse * lDotNorm;

            Vector reflectV = (-lightV).reflect(normalv);
            double reflectDot = reflectV * eyev;

            if (reflectDot > 0) {
                double factor = std::pow(reflectDot, m.shininess);
                specular = light.intensity * m.specular * factor;
            }
        }

        return ambient + diffuse + specular;
    }

    inline Color lighting(Material m, const PointLight& light, const Point& position, const Vector& eyev, const Vector& normalv) {
        return lighting(m, light, position, eyev, normalv, false);
    }
}