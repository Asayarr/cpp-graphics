// 第 4 章（上半）：画家算法 —— 用排序解决前后遮挡
//
// 思路：不剔除背面，而是把三角形按深度「从远到近」排序，远的先画、近的后画，
//       这样近处的三角形会覆盖远处的，视觉上就正确了。
//
// 注意：Model 类（model.h / model.cpp）是各章共用的，不要改它。
//       排序放在 main() 里做。
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <tuple>
#include <utility>
#include <vector>

#include "geometry.h"
#include "model.h"
#include "tgaimage.h"

constexpr int width  = 800;
constexpr int height = 800;

constexpr TGAColor white = {255, 255, 255, 255};
constexpr TGAColor red   = {  0,   0, 255, 255};

double signed_triangle_area(int ax, int ay, int bx, int by, int cx, int cy)
{
    return .5 * ((by - ay) * (bx + ax) + (cy - by) * (cx + bx) + (ay - cy) * (ax + cx));
}

void triangle(int ax, int ay, int bx, int by, int cx, int cy, TGAImage &framebuffer, TGAColor color)
{
    int bbminx = std::min(std::min(ax, bx), cx);
    int bbminy = std::min(std::min(ay, by), cy);
    int bbmaxx = std::max(std::max(ax, bx), cx);
    int bbmaxy = std::max(std::max(ay, by), cy);

    double total_area = signed_triangle_area(ax, ay, bx, by, cx, cy);
    if (total_area == 0)
        return; // 退化三角形，避免除零

    for (int x = bbminx; x <= bbmaxx; x++)
    {
        for (int y = bbminy; y <= bbmaxy; y++)
        {
            double alpha = signed_triangle_area(x, y, bx, by, cx, cy) / total_area;
            double beta  = signed_triangle_area(x, y, cx, cy, ax, ay) / total_area;
            double gamma = signed_triangle_area(x, y, ax, ay, bx, by) / total_area;
            if (alpha < 0 || beta < 0 || gamma < 0)
                continue;
            framebuffer.set(x, y, color);
        }
    }
}

std::tuple<int, int> project(const vec4 &v)
{
    return {(v.x + 1.) * width / 2,
            (v.y + 1.) * height / 2};
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "用法: " << argv[0] << " obj/model.obj\n";
        return 1;
    }

    Model model(argv[1]);
    TGAImage framebuffer(width, height, TGAImage::RGB);

    // ---- ① 给每个三角形算一个深度：取三个顶点 z 里最小的（最远的那一点）----
    std::vector<std::pair<double, int>> order; // {深度, 三角形编号}
    order.reserve(model.nfaces());
    for (int i = 0; i < model.nfaces(); i++)
    {
        double z = std::min({model.vert(i, 0).z,
                             model.vert(i, 1).z,
                             model.vert(i, 2).z});
        order.emplace_back(z, i);
    }

    // ---- ② 从远到近排序（z 小的先画）----
    std::sort(order.begin(), order.end());

    // ---- ③ 按排序后的顺序画 ----
    // 每个三角形随机颜色：这样才能看出“前面的盖住后面的”
    for (const auto &item : order)
    {
        const int i = item.second;
        auto [ax, ay] = project(model.vert(i, 0));
        auto [bx, by] = project(model.vert(i, 1));
        auto [cx, cy] = project(model.vert(i, 2));

        TGAColor rnd;
        for (int c = 0; c < 3; c++)
            rnd[c] = std::rand() % 255;
        triangle(ax, ay, bx, by, cx, cy, framebuffer, rnd);
    }

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}
