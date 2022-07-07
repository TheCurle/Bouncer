/***************
 * THECURLE    *
 *     BOUNCER *
 ***************/

#pragma once

#include <catch2/catch_test_macros.hpp>
#include <core/Matrix.h>
#include <core/Raster.hpp>
#include <render/Patterns.h>

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
    double reflectivity;
    double transparency;
    double refractiveIndex;

    Material() : color({ 1, 1, 1}), pattern(nullptr), ambient(0.1), diffuse(0.9), specular(0.9), shininess(200.0), reflectivity(0), transparency(0), refractiveIndex(1) {}
    Material(Color col, double amb, double diff, double spec, double shin, double reflec, double trans, double refr) : color(col), pattern(nullptr), ambient(amb), diffuse(diff), specular(spec), shininess(shin), reflectivity(reflec), transparency(trans), refractiveIndex(refr) {}

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

    /*
     * To keep in line with the One Definition Rule, the following functions are defined in the World.h header file.
     * This is because they require working with the World struct which has dependencies back to this file.
     *
     * double fresnelContribution(RT::IntersectionDetail& detail)
     * Color reflected(World& w, RT::IntersectionDetail details, int countdown)
     * Color refracted(World& w, RT::IntersectionDetail details, int countdown)
     * bool isInShadow(World& world, Point& point)
     * Color shadeHit(World& world, RT::IntersectionDetail& hit, int countdown)
     * Color at(World& w, RT::Ray r, int countdown)
     */

    // Phong Shading Lighting workhouse.
    inline Color lighting(Material m, Geo* object, PointLight light, const Point& position, const Vector& eyev, const Vector& normalv, bool inShadow) {
        Color materialColor = m.pattern != nullptr ? Pattern::colorAt(position, object) : m.color;

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

    inline Color lighting(Material m, Geo* object, const PointLight& light, const Point& position, const Vector& eyev, const Vector& normalv) {
        return lighting(m, object, light, position, eyev, normalv, false);
    }

}