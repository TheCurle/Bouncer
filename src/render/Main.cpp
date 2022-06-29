/***************
 * THECURLE    *
 *     BOUNCER *
 ***************/

#include <type/Tuple.hpp>
#include <type/Raster.hpp>
#include <fstream>
#include <cstdio>
#define MATRIX_OPERATOR_OVERLOADS
#define GEOMETRY_OPERATOR_OVERLOADS
#include "render/Geometry.h"
#include "render/Ray.h"

// The tolerance for comparisons.
static constexpr double epsilon = 0.0001;
// Compare two doubles with tolerance.
bool safeCompare(double a, double b) {
    return abs(a - b) < epsilon;
}

int main(int argc, char* argv[]) {
    (void) argc;
    (void) argv;

    Color ind(1, 0, 0);

    Point origin(0, 0, -5);
    int wallZ = 10;
    double wallSize = 7;
    int canvasSize = 100;
    double worldPixelSize = wallSize / canvasSize;
    double halfWall = wallSize / 2;

    Framebuffer frame(canvasSize, canvasSize);
    Sphere s;

    for (int i = 0; i < canvasSize - 1; i++) {
        double worldY = halfWall - worldPixelSize * i;

        for (int j = 0; j < canvasSize - 1; j++) {
            double worldX = halfWall - worldPixelSize * j;

            Point pos(worldX, worldY, wallZ);
            Ray r(origin, Vector((pos - origin).normalize()));

            Intersections xs = Ray::intersect(s, r);

            if (!xs.hit().isEmpty()) {
                frame.set(j, i, ind);
            }

        }
    }

    std::ofstream out("pic.ppm");
    std::string text = frame.export_ppm();
    out.write(text.c_str(), text.size());
    out.close();
    printf("Done\n");

    return 0;
}