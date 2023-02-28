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

    // The objects and state of the world.
    World w;
    // The camera that is used to project things in the world.
    Camera cam;
    // A copy of the camera, used for the base position at the start of an arcball rotation.
    Camera camCached;

    // The thread that actually renders things to the active framebuffer, so that we can deal with user input and drawing things separately.
    std::thread raytraceThread;

    std::atomic_bool mouseReleased = false;
    // Should another frame start rendering as soon as the current is finished?
    std::atomic_bool rerender = false;
    // Is the current frame finished rendering?
    std::atomic_bool renderComplete = false;
    // Should a faster, simplified method of rendering be used?
    std::atomic_bool renderPreview = false;
    // Is this the first frame being rendered?
    bool firstFrame = true;

    // The x and y that the currently held click started at.
    float mouseXClickStart = 0;
    float mouseYClickStart = 0;

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
    // Position sliders
    Slider* positionXSlider = nullptr;    Label* positionXLabel = nullptr;
    Slider* positionYSlider = nullptr;    Label* positionYLabel = nullptr;
    Slider* positionZSlider = nullptr;    Label* positionZLabel = nullptr;

    // World inspector
    std::vector<std::string> objectNames;
    ListBox* objectList = nullptr;

public:

    // The function called when a new frame is to be rendered.
    // Dispatches threads to do the actual work.
    void render() {
        w.renderRT(cam, frame, 0, 0, framewidth, frameheight, renderPreview);
        renderComplete = true;
        rerender = false;
    }

    FramebufferView() : frame { Framebuffer(1, 1) } {
        // Name your application
        sAppName = "Bouncer Live View";

        // Set up world geometry; three spheres.
        auto* one = new Sphere;
        one->setMatrix(Matrix::scaling(100, 100, 100));
        one->material.color = Color(1.0, 0, 0);
        auto* two = new Sphere;
        two->setMatrix(Matrix::translation(100, 0, 200) * Matrix::scaling(100, 100, 100));
        auto* three = new Sphere;
        three->setMatrix(Matrix::translation(-100, 0, -200) * Matrix::scaling(100, 100, 100));
        auto* four = new Sphere;
        four->setMatrix(Matrix::translation(200, 200, 200) * Matrix::scaling(100, 100, 100));

        // Set up the world objects and a light.
        w = World(
                { one, two, three, four },
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

        redLabel = new Label(gui, "Red", { 80.0f, 22.0f }, { 50.0f, 16.0f });
        greenLabel = new Label(gui, "Green", { 80.0f, 42.0f }, { 50.0f, 16.0f });
        blueLabel = new Label(gui, "Blue", { 80.0f, 62.0f }, { 50.0f, 16.0f });

        blueLabel->nAlign = Label::Alignment::Right;
        greenLabel->nAlign = Label::Alignment::Right;
        redLabel->nAlign = Label::Alignment::Right;

        reflectivitySlider = new Slider(gui, { 150.0f, 100.0f }, { 246.0f, 100.0f }, 0, 100, 50);
        transparencySlider = new Slider(gui, { 150.0f, 120.0f }, { 246.0f, 120.0f }, 0, 100, 50);
        diffuseSlider = new Slider(gui, { 150.0f, 140.0f }, { 246.0f, 140.0f }, 0, 100, 50);
        ambientSlider = new Slider(gui, { 150.0f, 160.0f }, { 246.0f, 160.0f }, 0, 100, 50);

        reflectivityLabel = new Label(gui, "Reflectivity", { 80.0f, 92.0f }, { 50.0f, 16.0f });
        transparencyLabel = new Label(gui, "Transparency", { 80.0f, 112.0f }, { 50.0f, 16.0f });
        diffuseLabel = new Label(gui, "Diffuse", { 80.0f, 132.0f }, { 50.0f, 16.0f });
        ambientLabel = new Label(gui, "Ambient", { 80.0f, 152.0f }, { 50.0f, 16.0f });

        reflectivityLabel->nAlign = Label::Alignment::Right;
        transparencyLabel->nAlign = Label::Alignment::Right;
        diffuseLabel->nAlign = Label::Alignment::Right;
        ambientLabel->nAlign = Label::Alignment::Right;

        positionXSlider = new Slider(gui, { 150.0f, 180.0f }, { 246.0f, 180.0f }, -500, 500, 0);
        positionYSlider = new Slider(gui, { 150.0f, 200.0f }, { 246.0f, 200.0f }, -500, 500, 0);
        positionZSlider = new Slider(gui, { 150.0f, 220.0f }, { 246.0f, 220.0f }, -500, 500, 0);

        positionXLabel = new Label(gui, "X Position", { 80.0f, 172.0f }, { 50.0f, 16.0f });
        positionYLabel = new Label(gui, "Y Position", { 80.0f, 192.0f }, { 50.0f, 16.0f });
        positionZLabel = new Label(gui, "Z Position", { 80.0f, 212.0f }, { 50.0f, 16.0f });

        positionXLabel->nAlign = Label::Alignment::Right;
        positionYLabel->nAlign = Label::Alignment::Right;
        positionZLabel->nAlign = Label::Alignment::Right;

        // Calculate and add the names for every sphere, plane, etc in the world.
        size_t idx;
        for (idx = 0; idx < w.numObjs; idx++) {
            objectNames.push_back(std::string(w.objects[idx]->getName()) + " (" + std::to_string(idx) + ")");
        }
        // Add the point light as a manipulable object.
        objectNames.push_back(std::string("Point Light") + " (" + std::to_string(idx) + ")");

        objectList = new ListBox(gui, objectNames, { 80.0f, 250.0f }, { 200.0f, 80.0f });

       return true;
    }

    // Update the camera's angles using the mouse movement delta.
    void updateCameraAngles() {
        float ndcX = 2.0f * GetMouseX() / (float) framewidth - 1;
        float ndcY = 1.0f - 2.0f * GetMouseY() / (float) frameheight;

        Point currentMousePoint (ndcX, ndcY, 0);
        Point cachedMousePoint (mouseXClickStart, mouseYClickStart, 0);

        Vector dir = currentMousePoint - cachedMousePoint;
        // The matrix dealing with the rotation brought about by the X movement of the mouse.
        // To move horizontally, we rotate around the Y axis.
        Matrix xMat = Matrix::rotation_y(dir.x * M_PI);
        // The matrix dealing with the rotation brought about by the Y movement of the mouse.
        // To move vertically, we rotate around the X axis.
        Matrix yMat = Matrix::rotation_x(dir.y * M_PI);

        // To avoid accidentally rotating around the Z axis, we rotate the cached camera position rather than the current.
        // To ensure that the camera rotates in an arcball-like fashion, we rotate the X after the Y; otherwise we rotate the camera's yaw rather than the position.
        cam.setTransform(camCached.transform * yMat * xMat);
        rerender = true;
    }

    // Handle updating the sliders when a new item is selected, and updating the item when the sliders are updated.
    void updateSliders() {
        bool pointLightSelected = objectList->nSelectedItem >= w.numObjs;
        bool selectionChanged = objectList->nSelectedItem != objectList->nPreviouslySelectedItem || firstFrame;

        // Check for whether the point light is selected! The rest of this function assumes the index is valid.
        if (pointLightSelected) {
            if (selectionChanged) {
                colorRedSlider->fValue = (float) w.lightSource.intensity.red() * 255;
                colorGreenSlider->fValue = (float) w.lightSource.intensity.green() * 255;
                colorBlueSlider->fValue = (float) w.lightSource.intensity.blue() * 255;

                reflectivitySlider->fValue = 0;
                transparencySlider->fValue = 0;
                diffuseSlider->fValue = 0;
                ambientSlider->fValue = 0;

                positionXSlider->fValue = (float) w.lightSource.position.x;
                positionYSlider->fValue = (float) w.lightSource.position.y;
                positionZSlider->fValue = (float) w.lightSource.position.z;
            } else {
                w.lightSource.position = { positionXSlider->fValue, positionYSlider->fValue, positionZSlider->fValue };
                w.lightSource.intensity = { colorRedSlider->fValue, colorGreenSlider->fValue, colorBlueSlider->fValue };
            }
            return;
        }

        // Save the object for later.
        // since the list is generated from the objects in the world, selectedItem must always be a valid index.
        Geo* obj = w.objects[objectList->nSelectedItem];

        // First, check if the list box is updated:
        if (selectionChanged) {
            colorRedSlider->fValue = (float) obj->material.color.red() * 255;
            colorGreenSlider->fValue = (float) obj->material.color.green() * 255;
            colorBlueSlider->fValue = (float) obj->material.color.blue() * 255;

            reflectivitySlider->fValue = (float) obj->material.reflectivity * 100;
            transparencySlider->fValue = (float) obj->material.transparency * 100;
            diffuseSlider->fValue = (float) obj->material.diffuse * 100;
            ambientSlider->fValue = (float) obj->material.ambient * 100;

            positionXSlider->fValue = (float) obj->center.x;
            positionYSlider->fValue = (float) obj->center.y;
            positionZSlider->fValue = (float) obj->center.z;
            return;
        }

        // Tell the engine to rerender if any parameters have changed.
        rerender = rerender | (colorRedSlider->changed || colorGreenSlider->changed
                || colorBlueSlider->changed || reflectivitySlider->changed
                || transparencySlider->changed || diffuseSlider->changed
                || ambientSlider->changed || positionXSlider->changed
                || positionYSlider->changed || positionZSlider->changed);

        // Next, set the new values from the sliders.
        // Because of the above checks, this shouldn't change anything even when switching to a new object.
        obj->material.color = Color(colorRedSlider->fValue / 255, colorGreenSlider->fValue / 255, colorBlueSlider->fValue / 255);
        obj->material.reflectivity = reflectivitySlider->fValue / 100;
        obj->material.transparency = transparencySlider->fValue / 100;
        obj->material.diffuse = diffuseSlider->fValue / 100;
        obj->material.ambient = ambientSlider->fValue / 100;
        // Update the object's position with the object's matrix with an updated position.
        // A little roundabout, but that's the price we pay for performance.

        if (positionXSlider->changed || positionYSlider->changed || positionZSlider->changed)
            obj->setMatrix(obj->transform.setTranslation({ positionXSlider->fValue, positionYSlider->fValue, positionZSlider->fValue }));

        // Reset all the sliders, so we don't get continuous updates.
        colorRedSlider->changed = false;
        colorGreenSlider->changed = false;
        colorBlueSlider->changed = false;
        reflectivitySlider->changed = false;
        transparencySlider->changed = false;
        diffuseSlider->changed = false;
        ambientSlider->changed = false;
        positionXSlider->changed = false;
        positionYSlider->changed = false;
        positionZSlider->changed = false;
    }

    // Called every frame of the window the user sees; separated from the actual image being generated.
    bool OnUserUpdate(float fElapsedTime) override {
        gui.Update(this);

        // Cache the mouse released event, so that we can properly disable the preview mode and render a shaded image.
        if (GetMouse(0).bReleased)
            mouseReleased = true;

        // If the mouse is pressed, start rotating the camera.
        // We do this by tracking where the mouse started, and the camera's first position and rotation.
        // While rotating, we also start rendering in a faster, simpler preview of objects' colors and positions.
        if (GetMouse(0).bPressed) {
            mouseXClickStart = 2.0f * GetMouseX() / (float) framewidth - 1;
            mouseYClickStart = 1.0f - 2.0f * GetMouseY() / (float) frameheight;
            camCached = cam;
            renderPreview = true;
        }

        // Don't do any render-thread manipulation unless an image is finished; an async semaphore.
        if (renderComplete) {
            // If the current render job is finished, destroy its thread
            if (raytraceThread.joinable()) raytraceThread.join();

            // Update the camera when holding the mouse!
            if (GetMouse(0).bHeld || GetMouse(0).bReleased) {
                updateCameraAngles();
            }

            // If the mouse release "event" is waiting, handle it now.
            if (mouseReleased) {
                renderPreview = false;      // Stop rendering the preview
                rerender = true;            // Render a new, non-preview image now.
                mouseReleased = false;      // The event is handled, so reset it.
            }

            // Now that nothing is reading the data of the sliders, we can update and change them.
            updateSliders();

            // If another frame needs to start,
            if (rerender) {
                // Start it immediately.
                renderComplete = false;
                raytraceThread = std::thread(&FramebufferView::render, this);
            }

            // Tell the rest of the program that the first frame is over
            if (firstFrame) firstFrame = false;
        }


        // Asynchronously from the actual image being rendered, we can still draw it to the screen.
        // This should be somewhat seamless, as long as the image can be rendered fast enough.
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
        return true;
    }
};

// The tolerance for comparisons.
static constexpr double epsilon = 0.001;
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