/***************
 * THECURLE    *
 *     BOUNCER *
 ***************/

#include <catch2/catch_test_macros.hpp>
#include <utility>
#include <type/Matrix.h>
#include <render/Geometry.h>
#include <render/Ray.h>
#include "render/Light.h"

#pragma once
using namespace std;

// Information on the origin of rays.
struct Camera {
    int horizontalSize, verticalSize;
    double fieldOfView;
    Matrix transform;

    Camera(int hSize, int vSize, double fov) {
        horizontalSize = hSize;
        verticalSize = vSize;
        fieldOfView = fov;

        transform = Matrix::identity();
    }
};