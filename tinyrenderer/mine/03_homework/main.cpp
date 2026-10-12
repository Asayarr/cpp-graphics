#include <cmath>
#include "tgaimage.h"

constexpr TGAColor green = {0, 255, 0, 255};
constexpr TGAColor red = {0, 0, 255, 255};
constexpr TGAColor blue = {255, 128, 64, 255};

double signed_triangle_area(int ax, int ay, int bx, int by, int cx, int cy)
{
    return .5 * ((by - ay) * (bx + ax) + (cy - by) * (cx + bx) + (ay - cy) * (ax + cx));
}

void triangle(int ax, int ay, int bx, int by, int cx, int cy, TGAColor colorA, TGAColor colorB, TGAColor colorC, TGAImage &framebuffer)
{
    int bbminx = std::min(std::min(ax, bx), cx); // bounding box for the triangle
    int bbminy = std::min(std::min(ay, by), cy); // defined by its top left and bottom right corners
    int bbmaxx = std::max(std::max(ax, bx), cx);
    int bbmaxy = std::max(std::max(ay, by), cy);
    double total_area = signed_triangle_area(ax, ay, bx, by, cx, cy);
    if (total_area < 1)
        return; // backface culling + discarding triangles that cover less than a pixel

#pragma omp parallel for
    for (int x = bbminx; x <= bbmaxx; x++)
    {
        for (int y = bbminy; y <= bbmaxy; y++)
        {
            double alpha = signed_triangle_area(x, y, bx, by, cx, cy) / total_area;
            double beta = signed_triangle_area(x, y, cx, cy, ax, ay) / total_area;
            double gamma = signed_triangle_area(x, y, ax, ay, bx, by) / total_area;
            if (alpha < 0 || beta < 0 || gamma < 0)
                continue; // negative barycentric coordinate => the pixel is outside the triangle
            TGAColor c;
            for (int k = 0; k < 3; k++){
                c[k] = static_cast<unsigned char>(alpha * colorA[k] + beta * colorB[k] + gamma * colorC[k]);
            }
                framebuffer.set(x, y, c);
        }
    }
}

int main(int argc, char **argv)
{
    constexpr int width = 64;
    constexpr int height = 64;
    TGAImage framebuffer(width, height, TGAImage::RGB);

    int ax = 17, ay = 4;
    int bx = 55, by = 39;
    int cx = 23, cy = 59;

    triangle(ax, ay, bx, by, cx, cy,red, green, blue, framebuffer);

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}