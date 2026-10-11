#include <cmath>
#include <tuple>
#include "geometry.h"
#include "model.h"
#include "tgaimage.h"

constexpr TGAColor white = {255, 255, 255, 255};
constexpr TGAColor green = {0, 255, 0, 255};
constexpr TGAColor red = {0, 0, 255, 255};
constexpr TGAColor blue = {255, 128, 64, 255};
constexpr TGAColor yellow = {0, 200, 255, 255};
constexpr int width = 140;
constexpr int height = 140;

void triangle(int ax, int ay, int bx, int by, int cx, int cy, TGAImage &framebuffer, TGAColor color)
{
    if (ay > by)
    {
        std::swap(ax, bx);
        std::swap(ay, by);
    }
    if (ay > cy)
    {
        std::swap(ax, cx);
        std::swap(ay, cy);
    }
    if (by > cy)
    {
        std::swap(bx, cx);
        std::swap(by, cy);
    }
    int total_height = cy - ay;

    if (ay != by)
    { // if the bottom half is not degenerate
        int segment_height = by - ay;
        for (int y = ay; y <= by; y++)
        { // sweep the horizontal line from ay to by
            int x1 = ax + ((cx - ax) * (y - ay)) / total_height;
            int x2 = ax + ((bx - ax) * (y - ay)) / segment_height;
            for (int x = std::min(x1, x2); x < std::max(x1, x2); x++) // draw a horizontal line
                framebuffer.set(x, y, color);
        }
    }

    if (by != cy)
    { // if the upper half is not degenerate
        int segment_height = cy - by;
        for (int y = by; y <= cy; y++)
        { // sweep the horizontal line from by to cy
            int x1 = ax + ((cx - ax) * (y - ay)) / total_height;
            int x2 = bx + ((cx - bx) * (y - by)) / segment_height;
            for (int x = std::min(x1, x2); x < std::max(x1, x2); x++) // draw a horizontal line
                framebuffer.set(x, y, color);
        }
    }
}

int main(int argc, char **argv)
{
    TGAImage framebuffer(width, height, TGAImage::RGB);
    triangle(7, 45, 35, 100, 45, 60, framebuffer, red);
    triangle(120, 35, 90, 5, 45, 110, framebuffer, white);
    triangle(115, 83, 80, 90, 85, 120, framebuffer, green);
    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}
