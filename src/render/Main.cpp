/***************
 * THECURLE    *
 *     BOUNCER *
 ***************/

#include <type/Tuple.hpp>
#include <type/Raster.hpp>
#include <fstream>
#define MATRIX_OPERATOR_OVERLOADS
#define GEOMETRY_OPERATOR_OVERLOADS
#include "render/Geometry.h"
#include "view/World.h"
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

int framewidth = 300;
int frameheight = 150;

// Override base class with your custom functionality
class FramebufferView : public olc::PixelGameEngine
{
    Framebuffer frame;
    World w;
    std::thread raytraceThread;
public:
    FramebufferView() : frame(Framebuffer(1,1))
    {
        // Name your application
        sAppName = "Bouncer Live View";

        raytraceThread = std::thread([&]() {
            w = World::defaultWorld();
            Plane wall;
            wall.transform = Matrix::rotation_x(M_PI/2) * Matrix::translation(0, 0, -5);
            wall.material.pattern = new Pattern::Checker(Color::white(), { 1, 0, 0 });
            w.objects.emplace_back(&wall);
            Plane floor;
            floor.transform = Matrix::translation(0, -1, 0);
            floor.material.transparency = 0.5;
            floor.material.refractiveIndex = 1.5;
            w.objects.emplace_back(&floor);
            Sphere ball;
            ball.transform = Matrix::translation(0, -3.5, -0.5);
            ball.material.color = { 1, 0, 0 };
            ball.material.ambient = 0.5;
            w.objects.emplace_back(&ball);

            Camera cam(framewidth, frameheight, M_PI / 3);
            cam.transform = World::viewMatrix({ 0, 0, -7 }, { 0, 0, 0 }, { 0, 1, 0 });

            frame = Framebuffer(cam.horizontalSize, cam.verticalSize);

            w.render(cam, frame);
        });
    }

public:
    bool OnUserCreate() override
    {
        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {
        (void) fElapsedTime;
        // Called once per frame, draws random coloured pixels
        for (int x = 0; x < frame.width; x++)
            for (int y = 0; y < frame.height; y++) {
                Color c = frame.at(x, y);
                Draw(x, y, olc::Pixel(c.red() * 255, c.green() * 255, c.blue() * 255, 255));
            }

        if (PixelGameEngine::GetKey(olc::Key::ENTER).bPressed) {
            std::ofstream out("pic2.ppm");
            std::string text = frame.export_ppm();
            out.write(text.c_str(), text.size());
            out.close();

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

    FramebufferView view;
    if (view.Construct(framewidth, frameheight, 1, 1, false, true)) {
        view.Start();
    }

    return 0;
}