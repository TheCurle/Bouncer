/***************
 * THECURLE    *
 *     BOUNCER *
 ***************/

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <core/Raster.hpp>
#include <fstream>
#include "render/Geometry.h"
#include "view/World.h"
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#define OLC_PGEX_QUICKGUI
#include "QuickGUI.h"

using namespace olc;
using namespace olc::QuickGUI;

int framewidth = 800;
int frameheight = 600;

class FramebufferView : public olc::PixelGameEngine {
    // Render Elements
    Framebuffer frame;
    World w;
    Camera cam;
    std::thread raytraceThread;

    // Should another frame start rendering as soon as the current is finished?
    bool rerender = false;
    // Is the current frame finished rendering?
    std::atomic_bool renderComplete = false;
    // Is this the first frame being rendered?
    bool firstFrame = true;

    // A copy of the last known Mouse X and Y values, since the engine doesn't keep track of this..
    int32_t mouseXLast = 0;
    int32_t mouseYLast = 0;

    // GUI elements
    QuickGUI::Manager gui;
    // Color Sliders
    Slider* colorRedSlider = nullptr;     Label* redLabel = nullptr;
    Slider* colorGreenSlider = nullptr;   Label* greenLabel = nullptr;
    Slider* colorBlueSlider = nullptr;    Label* blueLabel = nullptr;
    // Material Sliders
    Slider* reflectivitySlider = nullptr; Label* reflectivityLabel = nullptr;
    Slider* transparencySlider = nullptr; Label* transparencyLabel = nullptr;
    Slider* diffuseSlider = nullptr;      Label* diffuseLabel = nullptr;
    Slider* ambientSlider = nullptr;      Label* ambientLabel = nullptr;
    // World inspector
    std::vector<std::string> objectNames;
    ListBox* objectList = nullptr;

public:

    // The function called when a new frame is to be rendered.
    // Dispatches threads to do the actual work.
    void render() {
        renderComplete = false;
        w.renderRT(cam, frame, 0, 0, framewidth, frameheight);
        renderComplete = true;
    }

    FramebufferView() : frame(Framebuffer(1,1)) {
        // Name your application
        sAppName = "Bouncer Live View";

        // Set up world geometry; three spheres.
        auto* one = new Sphere;
        one->setMatrix(Matrix::scaling(100, 100, 100));
        auto* two = new Sphere;
        two->setMatrix(Matrix::translation(100, 100, 200) * Matrix::scaling(100, 100, 100));
        two->material.color = Color(1.0, 0, 0);
        auto* three = new Sphere;
        three->setMatrix(Matrix::translation(200, 200, 400) * Matrix::scaling(100, 100, 100));

        // Set up the world objects and a light.
        w = World(
                { one, two, three },
                { { -50, 100, -250 }, { 1, 1, 1 } }
        );

        // Set up a camera that can be manipulated.
        cam = Camera(framewidth, frameheight, M_PI / 3);
        cam.setTransform({ 0, 0, -1500 }, { 0, 0, 0 }, { 0, 1, 0 });

        frame = Framebuffer(cam.horizontalSize, cam.verticalSize);

        // Immediately start rendering the first frame.
        raytraceThread = std::thread(&FramebufferView::render, this);
    }

public:
    // Set up GUI elements such as the sliders and lists.
    bool OnUserCreate() override {
        colorRedSlider = new Slider(gui, { 150.0f, 30.0f }, { 246.0f, 30.0f }, 0, 255, 128);
        colorGreenSlider = new Slider(gui, { 150.0f, 50.0f }, { 246.0f, 50.0f }, 0, 255, 128);
        colorBlueSlider = new Slider(gui, { 150.0f, 70.0f }, { 246.0f, 70.0f }, 0, 255, 128);

        reflectivitySlider = new Slider(gui, { 150.0f, 100.0f }, { 246.0f, 100.0f }, 0, 100, 50);
        transparencySlider = new Slider(gui, { 150.0f, 120.0f }, { 246.0f, 120.0f }, 0, 100, 50);
        diffuseSlider = new Slider(gui, { 150.0f, 140.0f }, { 246.0f, 140.0f }, 0, 100, 50);
        ambientSlider = new Slider(gui, { 150.0f, 160.0f }, { 246.0f, 160.0f }, 0, 100, 50);

        redLabel = new Label(gui, "Red", { 80.0f, 22.0f }, { 50.0f, 16.0f });
        greenLabel = new Label(gui, "Green", { 80.0f, 42.0f }, { 50.0f, 16.0f });
        blueLabel = new Label(gui, "Blue", { 80.0f, 62.0f }, { 50.0f, 16.0f });

        blueLabel->nAlign = Label::Alignment::Right;
        greenLabel->nAlign = Label::Alignment::Right;
        redLabel->nAlign = Label::Alignment::Right;

        reflectivityLabel = new Label(gui, "Reflectivity", { 80.0f, 92.0f }, { 50.0f, 16.0f });
        transparencyLabel = new Label(gui, "Transparency", { 80.0f, 112.0f }, { 50.0f, 16.0f });
        diffuseLabel = new Label(gui, "Diffuse", { 80.0f, 132.0f }, { 50.0f, 16.0f });
        ambientLabel = new Label(gui, "Ambient", { 80.0f, 152.0f }, { 50.0f, 16.0f });

        reflectivityLabel->nAlign = Label::Alignment::Right;
        transparencyLabel->nAlign = Label::Alignment::Right;
        diffuseLabel->nAlign = Label::Alignment::Right;
        ambientLabel->nAlign = Label::Alignment::Right;

        // Calculate and add the names for every sphere, plane, etc in the world.
        for (size_t i = 0; i < w.numObjs; i++) {
            objectNames.push_back(std::string(w.objects[i]->getName()) + " (" + std::to_string(i) + ")");
        }

        objectList = new ListBox(gui, objectNames, { 80.0f, 180.0f }, { 200.0f, 80.0f });

       return true;
    }

    // Update the camera's angles using the azimuth and elevation sliders.
    void updateCameraAngles() {
        float deltaAngleX = (2 * M_PI / framewidth);
        float deltaAngleY = (M_PI / frameheight);
        float xAngle = (mouseXLast - GetMouseX()) * deltaAngleX;
        float yAngle = (mouseYLast - GetMouseY()) * deltaAngleY;

        Matrix xRot = Matrix::rotation_x(xAngle);
        cam.pos = Point((xRot * (cam.pos - cam.target)) + cam.target);

        Matrix yRot = Matrix::rotation_y(yAngle);
        cam.pos = Point((yRot * (cam.pos - cam.target)) + cam.target);

        cam.setTransform(cam.pos, cam.target, cam.upVec);

        rerender = true;
    }

    // Handle updating the sliders when a new item is selected, and updating the item when the sliders are updated.
    void updateSliders() {
        // Save the object for later.
        // since the list is generated from the objects in the world, selectedItem must always be a valid index.
        Geo* obj = w.objects[objectList->nSelectedItem];

        // First, check if the list box is updated:
        if (objectList->nSelectedItem != objectList->nPreviouslySelectedItem || firstFrame) {
            colorRedSlider->fValue = obj->material.color.red() * 255;
            colorGreenSlider->fValue = obj->material.color.green() * 255;
            colorBlueSlider->fValue = obj->material.color.blue() * 255;

            reflectivitySlider->fValue = obj->material.reflectivity * 100;
            transparencySlider->fValue = obj->material.transparency * 100;
            diffuseSlider->fValue = obj->material.diffuse * 100;
            ambientSlider->fValue = obj->material.ambient * 100;
        }

        // Tell the engine to rerender if any parameters have changed.
        rerender = colorRedSlider->changed || colorGreenSlider->changed
                || colorBlueSlider->changed || reflectivitySlider->changed
                || transparencySlider->changed || diffuseSlider->changed
                || ambientSlider->changed;

        // Next, set the new values from the sliders.
        // Because of the above checks, this shouldn't change anything even when switching to a new object.
        obj->material.color = Color(colorRedSlider->fValue / 255, colorGreenSlider->fValue / 255, colorBlueSlider->fValue / 255);
        obj->material.reflectivity = reflectivitySlider->fValue / 100;
        obj->material.transparency = transparencySlider->fValue / 100;
        obj->material.diffuse = diffuseSlider->fValue / 100;
        obj->material.ambient = ambientSlider->fValue / 100;

        // Reset all the sliders, so we don't get continuous updates.
        colorRedSlider->changed = false;
        colorGreenSlider->changed = false;
        colorBlueSlider->changed = false;
        reflectivitySlider->changed = false;
        transparencySlider->changed = false;
        diffuseSlider->changed = false;
        ambientSlider->changed = false;
    }

    // Called every frame of the window the user sees; separated from the actual image being generated.
    bool OnUserUpdate(float fElapsedTime) override {
        gui.Update(this);

        Clear(olc::BLACK);

        // Don't do any render-thread manipulation unless an image is finished; an async semaphore.
        if (renderComplete) {
            // If the current render job is finished, destroy its thread
            if (raytraceThread.joinable()) raytraceThread.join();
            // Now that nothing is reading the data of the sliders, we can update and change them.
            updateSliders();
            // If another frame needs to start,
            if (rerender) {
                // Start it immediately.
                raytraceThread = std::thread(&FramebufferView::render, this);
                rerender = false;
            }

            // Tell the rest of the program that the first frame is over
            if (firstFrame) firstFrame = false;
        }

        if ((GetMouseX() != mouseXLast || GetMouseY() != mouseYLast) && GetMouse(1).bPressed)
            updateCameraAngles();

        // Asynchronously from the actual image being rendered, we can still draw it to the screen.
        // This produces the stripe effect as the image updates.
        (void) fElapsedTime;
        for (size_t x = 0; x < frame.width; x++)
            for (size_t y = 0; y < frame.height; y++) {
                Draw(x, y, olc::Pixel(frame.at(x, y)));
            }

        // A shortcut; press enter to save a png of what's on screen.
        if (PixelGameEngine::GetKey(olc::Key::ENTER).bPressed) {
            frame.export_png("pic2.png");

            std::cout << "Image saved." << std::endl;
        }

        // A shortcut; press esc to close.
        if (PixelGameEngine::GetKey(olc::Key::ESCAPE).bPressed) {
            olc_Terminate();
        }

        gui.Draw(this);

        mouseXLast = GetMouseX();
        mouseYLast = GetMouseY();

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