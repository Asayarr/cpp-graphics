// 第 5 章（camera-naive）：旋转 + 中心（透视）投影
// 并修复教程作业里的 bug：8 位深度图溢出
//
// ─── 作业：找出 bug ─────────────────────────────────────────
// 现象：旋转 + 透视投影后，恶魔靠近镜头的那只手会被"切掉"一块。
//
// 原因（教程的三个提示）：
//   1. z-buffer 是 8 位灰度图，只能存 0..255
//   2. 模型本来被认为落在 [-1,1]³ 里，但旋转 + 透视之后不成立了
//   3. 透视：z' = z / (1 - z/3)。旋转后 z 可到 0.93 → z' = 1.35
//      视口映射 (z'+1)*255/2 = 300 > 255
//      → 存进 unsigned char 时溢出（300 & 0xFF = 44），深度瞬间变成"很近"
//      → 那一片像素被错误地覆盖/遮挡
//
// 修复：深度缓冲改用 std::vector<double>，需要可视化时再单独转成图片。
// ────────────────────────────────────────────────────────────
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <tuple>
#include <vector>

#include "geometry.h"
#include "model.h"
#include "tgaimage.h"

constexpr int width  = 800;
constexpr int height = 800;

constexpr TGAColor white = {255, 255, 255, 255};

double signed_triangle_area(int ax, int ay, int bx, int by, int cx, int cy)
{
    return .5 * ((by - ay) * (bx + ax) + (cy - by) * (cx + bx) + (ay - cy) * (ax + cx));
}

/* ---------------- 三角形光栅化 + 浮点 z-buffer ---------------- */
void triangle(int ax, int ay, int az, int bx, int by, int bz, int cx, int cy, int cz,
              std::vector<double> &zbuffer, TGAImage &framebuffer, TGAColor color)
{
    //  换用裸数组后必须自己裁剪包围盒，否则越界访问会 segfault
    //   （TGAImage::set() 以前帮你静默忽略了越界坐标）
    int bbminx = std::max(0, std::min(std::min(ax, bx), cx));
    int bbminy = std::max(0, std::min(std::min(ay, by), cy));
    int bbmaxx = std::min(width - 1, std::max(std::max(ax, bx), cx));
    int bbmaxy = std::min(height - 1, std::max(std::max(ay, by), cy));

    double total_area = signed_triangle_area(ax, ay, bx, by, cx, cy);
    if (total_area < 1)
        return;

    for (int x = bbminx; x <= bbmaxx; x++)
    {
        for (int y = bbminy; y <= bbmaxy; y++)
        {
            double alpha = signed_triangle_area(x, y, bx, by, cx, cy) / total_area;
            double beta  = signed_triangle_area(x, y, cx, cy, ax, ay) / total_area;
            double gamma = signed_triangle_area(x, y, ax, ay, bx, by) / total_area;
            if (alpha < 0 || beta < 0 || gamma < 0)
                continue;

            // ★ 关键：用 double 保存，不会溢出（原来是 unsigned char）
            double z = alpha * az + beta * bz + gamma * cz;

            if (z <= zbuffer[x + y * width])
                continue;
            zbuffer[x + y * width] = z;
            framebuffer.set(x, y, color);
        }
    }
}

/* ---------------- 模型视图变换：绕 Y 轴转 30° ---------------- */
vec4 rot(const vec4 &v)
{
    constexpr double a = M_PI / 6;
    const double c = std::cos(a);
    const double s = std::sin(a);
    const mat<3, 3> Ry = {{{c, 0, s}, {0, 1, 0}, {-s, 0, c}}};
    vec3 r = Ry * v.xyz();
    return {r.x, r.y, r.z, 1.0};
}

/* ---------------- 中心（透视）投影 ----------------
   相机在 (0,0,c)，把点投影到 z=0 平面：
       (x', y') = (x, y) / (1 - z/c)
   这样近大远小。*/
vec4 persp(const vec4 &v)
{
    constexpr double c = 3.0;
    double d = 1.0 - v.z / c;
    return {v.x / d, v.y / d, v.z / d, 1.0};
}

/* ---------------- 正交投影到屏幕 ---------------- */
std::tuple<int, int, int> project(const vec4 &v)
{
    return {(v.x + 1.) * width / 2,
            (v.y + 1.) * height / 2,
            (v.z + 1.) * 255. / 2}; // 注意：可能 >255，所以 zbuffer 必须用 double
}

/* ---------------- 把深度数组可视化成 PNG/TGA ---------------- */
void write_zbuffer(const std::vector<double> &zb, const char *filename)
{
    double lo = std::numeric_limits<double>::max();
    double hi = -std::numeric_limits<double>::max();
    for (double v : zb)
        if (v > -1e300) { lo = std::min(lo, v); hi = std::max(hi, v); }

    TGAImage img(width, height, TGAImage::GRAYSCALE);
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
        {
            double v = zb[x + y * width];
            unsigned char c = 0;
            if (v > -1e300 && hi > lo)
                c = static_cast<unsigned char>((v - lo) / (hi - lo) * 255.0);
            img.set(x, y, {c});
        }
    img.write_tga_file(filename);
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

    // 修复：用 double 数组当深度缓冲，初值设成"无限远"
    std::vector<double> zbuffer(width * height, -std::numeric_limits<double>::max());

    for (int i = 0; i < model.nfaces(); i++)
    {
        auto [ax, ay, az] = project(persp(rot(model.vert(i, 0))));
        auto [bx, by, bz] = project(persp(rot(model.vert(i, 1))));
        auto [cx, cy, cz] = project(persp(rot(model.vert(i, 2))));

        TGAColor rnd;
        for (int c = 0; c < 3; c++)
            rnd[c] = std::rand() % 255;

        triangle(ax, ay, az, bx, by, bz, cx, cy, cz, zbuffer, framebuffer, rnd);
    }

    framebuffer.write_tga_file("framebuffer.tga");
    write_zbuffer(zbuffer, "zbuffer.tga");
    return 0;
}
