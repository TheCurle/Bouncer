/***************
 * THECURLE    *
 *     BOUNCER *
 ***************/

#include <view/World.h>

#pragma once

namespace RT {
    namespace Light {
        Color at(World& w, RT::Ray r, int countdown = 10);
    }

    // Get a list of intersections that the given ray will have.
    // Iterates every object in the world and checks whether the ray will intersect.
    // TODO: this is a vicious hotspot, how can we remove the Vector from this?
    inline RT::Intersections intersect(World& w, RT::Ray& r) {
        std::vector<RT::Intersection> isects;
        isects.reserve(5);

        for (size_t i = 0; i < w.numObjs; i++) {
            w.objects[i]->intersect(r, isects);
        }

        return { isects.begin(), isects.end() };
    }

    // Render this world using Ray Tracing, onto the given canvas.
    inline void render(World& w, const Camera& cam, Framebuffer& canvas, int fromX, int fromY, int toX, int toY) {
        auto startTime = std::chrono::system_clock::now();

        #pragma omp parallel for default(none) shared(w, canvas, cam, fromX, fromY, toX, toY)
        for (int y = fromY; y < toY; y++) {
            for (int x = fromX; x < toX; x++) {
                RT::Ray r = rayForPixel(cam, x, y);
                Color pix = Light::at(w, r);
                canvas.set(x, y, pix);
            }
        }

        // Some performance detail.
        auto endTime = std::chrono::system_clock::now();
        std::cout << "RT Timing data:" << std::endl <<
                  " Pixels rendered: " << cam.horizontalSize * cam.verticalSize << std::endl <<
                  " Average time per pixel: " << std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime).count() / (cam.horizontalSize * cam.verticalSize) << "ns" << std::endl <<
                  " Total render time: " << std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count() << "us (" << std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count() << "s)" << std::endl;
    }
}