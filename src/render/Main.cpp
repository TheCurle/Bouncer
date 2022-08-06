/***************
 * THECURLE    *
 *     BOUNCER *
 ***************/

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define GEO_RT
#include <core/Raster.hpp>
#include <fstream>
#define MATRIX_OPERATOR_OVERLOADS
#define GEOMETRY_OPERATOR_OVERLOADS
#include "render/Geometry.h"
#include "view/World.h"
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "render/RT/RTLighting.h"
#include "render/Raster/RLines.h"
#include "render/Raster/RTriangles.h"

int framewidth = 1280;
int frameheight = 720;

// Override base class with your custom functionality
class RasterView : public olc::PixelGameEngine
{
    Framebuffer frame;
    std::thread renderThread;
public:
    RasterView() : frame(Framebuffer(1,1)) {
        // Name your application
        sAppName = "Stencil Live View";
        renderThread = std::thread([&]() {
            frame = Framebuffer(framewidth, frameheight);
            auto startTime = std::chrono::system_clock::now();

            for (size_t y = 0; y < frameheight; y++)
                for (size_t x = 0; x < framewidth; x++)
                    frame.set(x, y, Color::white() );

            Raster::DrawWireframeTriangle(frame, { 200, 200, 0 }, { 500, 550, 0 }, { 420, 600, 0 }, { 1, 0, 1 } );
            Raster::DrawFilledTri(frame, { 200, 200, 0 }, { 500, 550, 0 }, { 420, 600, 0 }, { 0, 1, 1 } );

            Raster::DrawWireframeTriangle(frame, { 700, 200, 0 }, { 1000, 550, 0 }, { 920, 600, 0 }, { 1, 0, 1 } );
            Raster::DrawShadedTriangle(frame, { 700, 200, 0 }, { 1000, 550, 0 }, { 920, 600, 0 }, { 0, 1, 1 } );

            auto endTime = std::chrono::system_clock::now();
            std::cout << "Raster Timing data:" << std::endl <<
                      " Pixels rendered: " << framewidth * frameheight << std::endl <<
                      " Average time per pixel: " << std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime).count() / (framewidth * frameheight) << "ns" << std::endl <<
                      " Total render time: " << std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count() << "us (" << std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count() << "s)" << std::endl;

        });
    }

public:
    bool OnUserCreate() override {
        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override {
        (void) fElapsedTime;

        for (size_t x = 0; x < frame.width; x++)
            for (size_t y = 0; y < frame.height; y++) {
                Draw(x, y, olc::Pixel(frame.at(x, y)));
            }

        if (PixelGameEngine::GetKey(olc::Key::ESCAPE).bPressed) {
            renderThread.join();
            olc_Terminate();
        }

        return true;
    }
};

class RTView : public olc::PixelGameEngine
{
    Framebuffer frame;
    World w;
    std::thread raytraceThread;
public:
    RTView() : frame(Framebuffer(1,1)) {
        // Name your application
        sAppName = "Bouncer Live View";

        raytraceThread = std::thread([&]() {
            w = World::defaultWorld();

            Plane floor;
            floor.transform = Matrix::translation(0, 5, 0);
            floor.material.diffuse = 1;
            floor.material.transparency = 0.5;
            floor.material.refractiveIndex = 1.5;
            Sphere ball;
            ball.transform = Matrix::translation(0, -3.5, -0.5);
            ball.material.color = { 1, 0, 0 };
            ball.material.ambient = 0.5;
            w.addObjects( { &floor, &ball } );

            Camera cam(framewidth, frameheight, M_PI / 3);
            cam.setTransform(Camera::viewMatrix({ 0, 3, -3 }, { 0, -5, 0 }, { 0, 1, 0 }));

            frame = Framebuffer(cam.horizontalSize, cam.verticalSize);

            RT::render(w, cam, frame, 0, 0, framewidth, frameheight);
        });
    }

public:
    bool OnUserCreate() override {
        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override {
        (void) fElapsedTime;
        // Called once per frame, draws random coloured pixels
        for (size_t x = 0; x < frame.width; x++)
            for (size_t y = 0; y < frame.height; y++) {
                Draw(x, y, olc::Pixel(frame.at(x, y)));
            }

        if (PixelGameEngine::GetKey(olc::Key::ENTER).bPressed) {
            frame.export_png("pic2.png");

            std::cout << "Image saved." << std::endl;
            raytraceThread.join();
        }

        if (PixelGameEngine::GetKey(olc::Key::ESCAPE).bPressed) {
            olc_Terminate();
        }

        return true;
    }
};

// The tolerance for comparisons.
static constexpr double epsilon = 0.0001;
// Compare two doubles with tolerance.
bool safeCompare(double a, double b) {
    return abs(a - b) < epsilon;
}

int main(int argc, char* argv[]) {
    (void) argc;
    (void) argv;

    RTView rt;
    if (rt.Construct(framewidth, frameheight, 1, 1, false, true)) {
        rt.Start();
    }

    RasterView raster;
    if (raster.Construct(framewidth, frameheight, 1, 1, false, true)) {
        raster.Start();
    }

    return 0;
}