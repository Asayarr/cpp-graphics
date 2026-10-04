#include "model.h"
#include <cmath>
#include "tgaimage.h"

constexpr TGAColor white = {255, 255, 255, 255};
constexpr TGAColor green = {0, 255, 0, 255};
constexpr TGAColor red = {0, 0, 255, 255};
constexpr TGAColor blue = {255, 128, 64, 255};
constexpr TGAColor yellow = {0, 200, 255, 255};

void line(int ax, int ay, int bx, int by, TGAImage &framebuffer, TGAColor color)
{

    bool steep = std::abs(ax - bx) < std::abs(ay - by);
    if (steep)
    { // if the line is steep, we transpose the image
        std::swap(ax, ay);
        std::swap(bx, by);
    }

    if (ax > bx)
    { // make it left−to−right
        std::swap(ax, bx);
        std::swap(ay, by);
    }

    int y = ay;
    int ierror = 0;
    for (int x = ax; x <= bx; x++)
    {
        if (steep) // if transposed, de−transpose
            framebuffer.set(y, x, color);
        else
            framebuffer.set(x, y, color);

        ierror += 2 * std::abs(by - ay);
        y += (by > ay ? 1 : -1) * (ierror > bx - ax);
        ierror -= 2 * (bx - ax) * (ierror > bx - ax);
    }
}

int main(char **argv)
{
    constexpr int width = 800, height = 800;
    TGAImage framebuffer(width, height, TGAImage::RGB);

    Model model(argv[1]);

    for (int i = 0; i < model.nfaces(); i++)
    {
        vec4 v0 = model.vert(i, 0);
        vec4 v1 = model.vert(i, 1);
        vec4 v2 = model.vert(i, 2);

        int x0 = (v0.x + 1.) * width / 2;
        int y0 = (v0.y + 1.) * height / 2;

        int x1 = (v1.x + 1.) * width / 2;
        int y1 = (v1.y + 1.) * height / 2;

        int x2 = (v2.x + 1.) * width / 2;
        int y2 = (v2.y + 1.) * height / 2;

        line(x0, y0, x1, y1, framebuffer, red);
        line(x1, y1, x2, y2, framebuffer, red);
        line(x2, y2, x0, y0, framebuffer, red);
    }

    framebuffer.write_tga_file("framebuffer.tga");

    return 0;
}