/***************
 * THECURLE    *
 *     BOUNCER *
 ***************/

#include <type/Tuple.hpp>
#include <type/Raster.hpp>
#include <fstream>
#include <cstdio>

class Environment {
    public:

    Environment(Vector grav, Vector wnd) : gravity(grav), wind(wnd) {}

    Vector gravity;
    Vector wind;
};

class Projectile { 
    public:
    Point position;
    Vector velocity;

    Projectile(Point pos, Vector vel) : position(pos), velocity(vel) {}

    void tick(Environment env) {
        position = position + velocity;
        velocity = velocity + env.gravity + env.wind;
    }
};

int main(int argc, char* argv[]) {
    (void) argc;
    (void) argv;

    Projectile proj(Point(0, 1, 0), Vector(1, 1.8, 0).normalize() * 11.25);
    Environment env(Vector(0, -0.1, 0), Vector(-0.01, 0, 0));
    Framebuffer frame(900, 550);
    Color ind(1, 1, 1);

    size_t TickCount = 0;
    while(proj.position.y > 0) {
        proj.tick(env);
        frame.set(proj.position.x, frame.height - proj.position.y, ind);
        TickCount++;
    }

    printf("Projectile took %zu ticks to hit the ground.\n", TickCount);

    std::ofstream out("pic.ppm");
    std::string text = frame.export_ppm();
    out.write(text.c_str(), text.size());
    out.close();
    printf("Done\n");

    return 0;
}