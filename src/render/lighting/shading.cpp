#include <view/World.h>

namespace Light {

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

    // A wrapper that redirects to the other `lighting` function, with shadows disabled.
    // Exists to expose lighting to tests.
    inline Color lighting(Material m, Geo* object, const PointLight& light, const Point& position, const Vector& eyev, const Vector& normalv) {
        return lighting(m, object, light, position, eyev, normalv, false);
    }

    // Generate and calculate the color of a reflection from the given point.
    inline Color reflected(World& w, RT::IntersectionDetail details, int countdown) {
        if (details.object.material.reflectivity == 0) return Color::black();
        if (countdown < 1) return Color::black();

        RT::Ray reflectRay { details.overPoint, details.reflectv };
        Color color = Light::at(w, reflectRay, countdown - 1);

        return color * details.object.material.reflectivity;
    }

    // Generate and calculate the color of a refraction from the given point.
    inline Color refracted(World& w, RT::IntersectionDetail details, int countdown) {
        if (details.object.material.transparency == 0) return Color::black();
        if (countdown < 1) return Color::black();

        // Check for Total Internal Reflection; derived from Snell's Law
        double ratio = details.refractiveIdxIncoming / details.refractiveIdxOutgoing;
        // cos_i = cosine of the angle between the incoming ray and the surface.
        double cosi = details.eyev * details.normalv;
        // sin^2_t =
        double sin2t = (ratio*ratio) * (1 - (cosi*cosi));

        if(sin2t > 1)
            return Color::black();

        // Cast the refracted ray
        double cost = std::sqrt((double) 1.0 - sin2t);
        Vector dir = details.normalv * (ratio * cosi - cost) - details.eyev * ratio;

        RT::Ray refract { details.underPoint, dir };
        Color col = Light::at(w, refract, countdown - 1);

        return col * details.object.material.transparency;
    }

    // Trace a ray from the point to the nearest light source, and determine whether there is something blocking it.
    inline bool isInShadow(World& world, Point& point) {
        Vector v = world.lightSource.position - point;
        double distance = v.magnitude();
        Vector direction = Vector(v.normalize());

        RT::Ray r { point, direction };
        RT::Intersections sections = world.intersect(r);

        RT::Intersection hit = sections.hit();
        return (!hit.isEmpty() && hit.time < distance);
    }

    // Using the Schlick approximation of the Fresnel effect.
    // The returned double is the ratio between reflection and refraction in the final ray.
    inline double fresnelContribution(RT::IntersectionDetail& detail) {
        double cos = detail.eyev * detail.normalv;

        if (detail.refractiveIdxIncoming > detail.refractiveIdxOutgoing) {
            double ratio = detail.refractiveIdxIncoming / detail.refractiveIdxOutgoing;
            double sin2t = ratio * ratio * ( 1 - cos * cos);
            if (sin2t > 1) return 1;

            double cost = std::sqrt(1 - sin2t);
            cos = cost;
        }

        double r0 = std::pow((detail.refractiveIdxIncoming - detail.refractiveIdxOutgoing) / (detail.refractiveIdxIncoming + detail.refractiveIdxOutgoing), 2);
        return std::pow(r0 + (1 - r0) * (1-cos), 5);
    }

    // Calculate the final color of an interection by summing up the Phong lighting, the reflections, and refractions of a point.
    inline Color shadeHit(World& world, RT::IntersectionDetail& hit, int countdown) {
        Color rayTarget = lighting(hit.object.material, &hit.object, world.lightSource, hit.overPoint, hit.eyev, hit.normalv, Light::isInShadow(world, hit.overPoint));
        Color reflectedRay = Light::reflected(world, hit, countdown);
        Color refractedRay = Light::refracted(world, hit, countdown);

        Material mat = hit.object.material;
        if (mat.reflectivity > 0 && mat.transparency > 0) {
            double reflectance = fresnelContribution(hit);
            return rayTarget + (reflectedRay * reflectance) + (refractedRay * (1 - reflectance));
        } else {
            return refractedRay + rayTarget + reflectedRay;
        }
    }

    // Calculate the color at the intersection between the ray and the world.
    Color at(World& w, RT::Ray r, int countdown, bool simpleMode) {
        RT::Intersections isections = w.intersect(r);
        RT::Intersection hit = isections.hit();

        if (hit.time == -1 && hit.object == nullptr) return Color::black();

        RT::IntersectionDetail detail = RT::Intersection::fillDetail(hit, r, isections);

        if (simpleMode)
            return detail.object.material.color;
        else
            return shadeHit(w, detail, countdown);
    }
}