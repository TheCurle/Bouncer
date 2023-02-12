#include <render/Patterns.h>
#include <render/Geometry.h>

namespace Pattern {
    Color colorAt(const Point& point, Geo* object) {
        Point objectPoint = Point(object->inverseTransform * point);
        Point patternPoint = Point(object->material.pattern->inverseTransform * objectPoint);

        return object->material.pattern->at(patternPoint);
    }
}