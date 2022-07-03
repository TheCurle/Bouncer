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

            // Level Geometry
            Plane floor;
            floor.material.reflectivity = 0.6;

            // Objects

            Sphere middle;
            middle.transform = Matrix::translation(-0.5, 1, 0.5);
            middle.material = Material();
            middle.material.pattern = new Pattern::Stripe({ 1, 0, 1}, { 0, 0, 1 });
            middle.material.pattern->transform = Matrix::scaling(0.33, 0.33, 0.33);
            middle.material.diffuse = 0.7;
            middle.material.specular = 0.3;
            middle.material.reflectivity = 0.5;

            Sphere right;
            right.transform = Matrix::translation(1.5, 0.5, -0.5) * Matrix::scaling(0.5, 0.5, 0.5);
            right.material = Material();
            right.material.color = Color(0.5, 1, 0.1);
            right.material.diffuse = 0.7;
            right.material.specular = 0.3;
            right.material.reflectivity = 0.5;

            Sphere left;
            left.transform = Matrix::translation(-1.5, 0.33, -0.75) * Matrix::scaling(0.33, 0.33, 0.33);
            left.material = Material();
            left.material.color = Color(1, 0.8, 1);
            left.material.diffuse = 0.7;
            left.material.specular = 0.3;
            left.material.reflectivity = 0.5;

            w.lightSource = PointLight { { -10, 10, -10 }, { 1, 1, 1 } };
            w.objects = { &floor, &middle, &right, &left };

            Camera cam(framewidth, frameheight, M_PI / 3);
            cam.transform = World::viewMatrix({ 0, 1.5, -5 }, { 0, 1, 0 }, { 0, 1, 0 });

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
    if (view.Construct(framewidth, frameheight, 1, 1)) {
        view.Start();
    }

    return 0;
}