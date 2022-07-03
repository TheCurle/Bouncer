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
#include "view/World.h"

// The tolerance for comparisons.
static constexpr double epsilon = 0.0001;
// Compare two doubles with tolerance.
bool safeCompare(double a, double b) {
    return abs(a - b) < epsilon;
}

int main(int argc, char* argv[]) {
    (void) argc;
    (void) argv;

    auto totalStartTime = std::chrono::system_clock::now();

    World w;

    // Level Geometry
    Sphere floor;
    floor.transform = Matrix::scaling(10, 0.01, 10);
    floor.material = Material();
    floor.material.color = Color(1, 0.9, 0.9);
    floor.material.specular = 0;

    Sphere leftWall;
    leftWall.transform = Matrix::translation(0, 0, 5) *
            Matrix::rotation_y(-M_PI / 4) * Matrix::rotation_x(M_PI / 2) *
            Matrix::scaling(10, 0.01, 10);
    leftWall.material = floor.material;

    Sphere rightWall;
    rightWall.transform = Matrix::translation(0, 0, 5) *
            Matrix::rotation_y(M_PI / 4) * Matrix::rotation_x(M_PI / 2) *
            Matrix::scaling(10, 0.01, 10);
    rightWall.material = floor.material;

    // Objects

    Sphere middle;
    middle.transform = Matrix::translation(-0.5, 1, 0.5);
    middle.material = Material();
    middle.material.color = Color(0.1, 1, 0.5);
    middle.material.diffuse = 0.7;
    middle.material.specular = 0.3;

    Sphere right;
    right.transform = Matrix::translation(1.5, 0.5, -0.5) * Matrix::scaling(0.5, 0.5, 0.5);
    right.material = Material();
    right.material.color = Color(0.5, 1, 0.1);
    right.material.diffuse = 0.7;
    right.material.specular = 0.3;

    Sphere left;
    left.transform = Matrix::translation(-1.5, 0.33, -0.75) * Matrix::scaling(0.33, 0.33, 0.33);
    left.material = Material();
    left.material.color = Color(1, 0.8, 1);
    left.material.diffuse = 0.7;
    left.material.specular = 0.3;

    w.lightSource = PointLight { { -10, 10, -10 }, { 1, 1, 1 } };
    w.objects = { floor, rightWall, leftWall, middle, right, left };

    Camera cam(100, 50, M_PI / 3);
    cam.transform = World::viewMatrix({ 0, 1.5, -5 }, { 0, 1, 0 }, { 0, 1, 0 });

    Framebuffer frame = w.render(cam);

    std::ofstream out("pic.ppm");
    std::string text = frame.export_ppm();
    out.write(text.c_str(), text.size());
    out.close();

    auto totalEndTime = std::chrono::system_clock::now();
    std::cout << " Total processing time: " << std::chrono::duration_cast<std::chrono::nanoseconds>(totalEndTime - totalStartTime).count() << "ns (" << std::chrono::duration_cast<std::chrono::seconds>(totalEndTime - totalStartTime).count() << "s)" << std::endl;

    return 0;
}