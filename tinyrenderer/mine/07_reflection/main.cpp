/*************************************************************
 @Description: 第 7 章 —— Phong 反射模型（着色）
               把渲染管线拆成「顶点着色器 + 光栅化 + 片元着色器」，
               用 our_gl.h/our_gl.cpp 里的框架，自己只写 shader。

   Phong 反射模型：
       I = k_a·1                         环境光 ambient
         + k_d·max(0, n·l)               漫反射 diffuse (Lambert)
         + k_s·(max(0, r·v))^shininess   镜面高光 specular

   其中：
       n = 片元法线（单位向量，眼睛坐标系）
       l = 指向光源的方向（单位向量）
       v = 片元指向相机的方向（单位向量）
       r = 光的反射方向 = 2n(n·l) - l

 @编译: make（会自动链接 our_gl.cpp / model.cpp / tgaimage.cpp）
 @运行: ./app ../../obj/african_head/african_head.obj
**************************************************************/

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <utility>

#include "our_gl.h"
#include "model.h"

/* our_gl.cpp 里定义的全局状态（头文件里没有 extern 声明，这里自己声明） */
extern mat<4, 4> ModelView, Perspective;
extern std::vector<double> zbuffer;

/* 光源方向（眼睛坐标系）。取右上前方，归一化后使用。 */
static const vec3 LIGHT_DIR = normalized(vec3{1.0, 1.0, 1.0});

/* ---------------- Phong 着色器 ---------------- */
struct PhongShader : IShader
{
    const Model &model;

    // 顶点着色器的输出（每个三角形 3 份），片元着色器用它们做插值
    vec3 tri_nrm[3]; // 顶点法线（眼睛坐标系）
    vec3 tri_eye[3]; // 顶点位置（眼睛坐标系）

    explicit PhongShader(const Model &m) : model(m) {}

    /* 顶点着色器：把顶点变到裁剪空间，同时保存法线和位置 */
    vec4 vertex(const int face, const int vert)
    {
        vec4 v = model.vert(face, vert); // 对象坐标，w = 1
        vec4 gl_Position = ModelView * v;
        tri_eye[vert] = gl_Position.xyz(); // 眼睛坐标

        // 法线的 w = 0，所以 ModelView 只会旋转它，不会平移
        vec4 n = model.normal(face, vert);
        tri_nrm[vert] = normalized((ModelView * n).xyz());

        return Perspective * gl_Position; // 裁剪坐标
    }

    /* 片元着色器：bar 是这个像素的重心坐标（已做透视校正） */
    std::pair<bool, TGAColor> fragment(const vec3 bar) const override
    {
        // ① 用重心坐标插值出本片元的法线和位置
        vec3 n = normalized(tri_nrm[0] * bar.x + tri_nrm[1] * bar.y + tri_nrm[2] * bar.z);
        vec3 p = tri_eye[0] * bar.x + tri_eye[1] * bar.y + tri_eye[2] * bar.z;

        // ② 视线方向：相机在眼睛坐标系的原点，所以是 normalize(-p)
        //    （geometry.h 没有一元 operator-，所以写成 p * -1.0）
        vec3 v = normalized(p * -1.0);

        // ③ Phong 三项
        constexpr double ka = 0.10; // 环境光系数
        constexpr double kd = 0.90; // 漫反射系数
        constexpr double ks = 0.50; // 高光系数
        constexpr double shininess = 64.0;

        double diff = std::max(0.0, n * LIGHT_DIR); // n·l，背面取 0

        vec3 r = normalized(2.0 * n * (n * LIGHT_DIR) - LIGHT_DIR); // 反射方向
        double spec = std::pow(std::max(0.0, r * v), shininess);    // (r·v)^e

        double I = ka + kd * diff + ks * spec;
        I = std::min(1.0, I); // 别超过 255

        unsigned char g = static_cast<unsigned char>(I * 255.0);
        return {false, TGAColor{g, g, g, 255}}; // 先画灰度
    }
};

/* ---------------- 主函数 ---------------- */
int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cerr << "用法: " << argv[0] << " obj/model.obj [更多模型...]\n";
        return 1;
    }

    constexpr int width = 800;
    constexpr int height = 800;

    constexpr vec3 eye{-1, 0, 2};   // 相机位置
    constexpr vec3 center{0, 0, 0}; // 看向点
    constexpr vec3 up{0, 1, 0};     // 相机上方向

    lookat(eye, center, up);
    init_perspective(norm(eye - center));
    init_viewport(width / 16, height / 16, width * 7 / 8, height * 7 / 8);
    init_zbuffer(width, height);

    TGAImage framebuffer(width, height, TGAImage::RGB, {177, 195, 209, 255});

    for (int m = 1; m < argc; m++)
    {
        Model model(argv[m]);
        PhongShader shader(model);

        for (int f = 0; f < model.nfaces(); f++)
        {
            // 图元装配：调用顶点着色器得到三个裁剪空间顶点
            Triangle clip = {shader.vertex(f, 0),
                             shader.vertex(f, 1),
                             shader.vertex(f, 2)};
            // 光栅化：内部会逐像素调用 shader.fragment()
            rasterize(clip, shader, framebuffer);
        }
    }

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}
