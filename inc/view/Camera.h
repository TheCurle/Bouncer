/***************
 * THECURLE    *
 *     BOUNCER *
 ***************/

#include <catch2/catch_test_macros.hpp>
#include <utility>
#include <core/Matrix.h>

#pragma once

// Information on the origin of rays, and the view space we render to
struct Camera {
    int horizontalSize, verticalSize;
    double halfWidth, halfHeight;
    double pixelSize;
    double fieldOfView;
    Matrix transform;
    Matrix inverseTransform;

    Camera(int hSize, int vSize, double fov) {
        horizontalSize = hSize;
        verticalSize = vSize;
        fieldOfView = fov;

        double halfView = std::tan(fieldOfView / 2);
        double aspectRatio = (double) horizontalSize / (double) verticalSize;

        if (aspectRatio > 0) {
            halfWidth = halfView;
            halfHeight = halfView / aspectRatio;
        } else {
            halfWidth = halfView / aspectRatio;
            halfHeight = halfView;
        }

        pixelSize = (halfWidth * 2) / horizontalSize;

        transform = Matrix::identity();
        inverseTransform = Matrix::identity();
    }

    void setTransform(const Matrix& mat) {
        transform = mat;
        inverseTransform = Matrix::fastInverse(mat);
    }

    // Generate a view matrix that will place and orient the camera appropriately.
    static Matrix viewMatrix(const Point& start, const Point& end, Vector up) {
        Vector forward = end - start;
        forward = Vector(forward.normalize());

        Vector left = forward.cross(Vector(up.normalize()));
        Vector trueUp = left.cross(forward);

        Matrix orientation {
                {
                        { left.x, left.y, left.z, 0 },
                        { trueUp.x, trueUp.y, trueUp.z, 0 },
                        { -forward.x, -forward.y, -forward.z, 0 },
                        { 0, 0, 0, 1 }
                }
        };

        return orientation * Matrix::translation(-start.x, -start.y, -start.z);
    }
};