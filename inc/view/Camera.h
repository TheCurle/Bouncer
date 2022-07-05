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

// Information on the origin of rays.
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

    [[nodiscard]] Ray rayForPixel(int x, int y) const {
        double xOffset = (x + 0.5) * pixelSize;
        double yOffset = (y + 0.5) * pixelSize;

        double worldX = halfWidth - xOffset;
        double worldY = halfHeight - yOffset;

        Point pixel = Point(inverseTransform * Point(worldX, worldY, -1));
        Point origin = Point(inverseTransform * Point(0, 0, 0));
        Vector dir = Vector((pixel - origin).normalize());

        return Ray { origin, dir };
    }
};