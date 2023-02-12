/***************
 * THECURLE    *
 *     BOUNCER *
 ***************/

#include <catch2/catch_test_macros.hpp>
#include <utility>
#include <core/Matrix.h>
#include <render/Geometry.h>
#include <render/Ray.h>
#include <render/Light.h>

#pragma once

// Information on the origin of rays, and the view space we render to
struct Camera {
    int horizontalSize {}, verticalSize {};
    double halfWidth {}, halfHeight {};
    double pixelSize {};
    double fieldOfView {};
    Matrix transform;
    Matrix inverseTransform;

    // A copy of the data used to derive the transform matrices, in case it needs to be changed (ie. arcball)
    Point pos { 0, 0, 0 };
    Point target { 0, 0, 0 };
    Vector upVec { 0, 1, 0 };

    Camera() = default;

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

    void setTransform(const Point& origin, const Point& lookAt, Vector up) {
        pos = origin; target = lookAt; upVec = up;
        setTransform(viewMatrix(origin, lookAt, up));
    }

    // Create a an RT Ray that will render the given pixel on the screen.
    [[nodiscard]] RT::Ray rayForPixel(int x, int y) const {
        // Start in the middle of the pixel; x+0.5,y+0.5
        double xOffset = (x + 0.5) * pixelSize;
        double yOffset = (y + 0.5) * pixelSize;

        // Project this point into the world, taking into account the field of view.
        double worldX = halfWidth - xOffset;
        double worldY = halfHeight - yOffset;

        // The pixel we're trying to cast through is now a known point in the world.
        Point pixel = Point(inverseTransform * Point(worldX, worldY, -1));
        // We move the camera around the origin according to its translation and rotation,
        Point origin = Point(inverseTransform * Point(0, 0, 0));
        // And cast a ray from the camera's origin to the point we're trying to render.
        Vector dir = Vector((pixel - origin).normalize());

        return RT::Ray { origin, dir };
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