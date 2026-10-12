/*************************************************************
 @Description: 最简单的例子 —— 用 4 段三次 Bezier 曲线画一个爱心
               每条曲线 4 个控制点，一共 16 个点，没有别的了。
               同时画出控制多边形（灰虚线）和控制点（小圆点），
               可以直观看到「曲线是怎么被控制点"拉"出来的」。

                  B(t) = (1-t)^3 P0 + 3(1-t)^2 t P1 + 3(1-t) t^2 P2 + t^3 P3

 @编译: g++ -std=c++20 main.cpp -lglut -lGLU -lGL -o heart
 @运行: ./heart        （ESC 退出，S 保存截图）
**************************************************************/

#define GL_SILENCE_DEPRECATION

#if defined(_WIN32) && __has_include("pch.h")
#include "pch.h"
#endif

#if defined(_WIN32)
#include <glut.h>
#else
#include <GL/glut.h>
#endif

#include <cmath>
#include <cstdio>
#include <vector>

struct P
{
    double x, y;
};

/* ---------------- 数据：4 段三次 Bezier，拼成一个爱心 ----------------
   顺序是首尾相接的：第 1 段的终点 = 第 2 段的起点，依此类推，
   最后一段的终点又回到第 1 段的起点 → 闭合。 */
static const P HEART[4][4] = {
    {{0.0, -0.76}, {-0.6, -0.30}, {-0.9, 0.10}, {-0.9, 0.40}}, // 左下
    {{-0.9, 0.40}, {-0.9, 0.76}, {-0.4, 0.90}, {0.0, 0.40}},   // 左上（左边那个圆）
    {{0.0, 0.40},  {0.4, 0.90},  {0.9, 0.76},  {0.9, 0.40}},   // 右上（右边那个圆）
    {{0.9, 0.40},  {0.9, 0.10},  {0.6, -0.30}, {0.0, -0.76}},  // 右下（回到尖）
};

/* ---------------- 三次 Bezier 求值 ---------------- */
P bezier(const P c[4], double t)
{
    double u = 1.0 - t;
    double b0 = u * u * u;
    double b1 = 3.0 * u * u * t;
    double b2 = 3.0 * u * t * t;
    double b3 = t * t * t;

    P r;
    r.x = b0 * c[0].x + b1 * c[1].x + b2 * c[2].x + b3 * c[3].x;
    r.y = b0 * c[0].y + b1 * c[1].y + b2 * c[2].y + b3 * c[3].y;
    return r;
}

/* ---------------- 保存窗口画面为 TGA ---------------- */
void save_screenshot(const char *filename)
{
    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);
    if (w <= 0 || h <= 0)
        return;

    std::vector<unsigned char> rgb((size_t)w * h * 3);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, rgb.data());

    FILE *fp = fopen(filename, "wb");
    if (!fp)
        return;

    unsigned char header[18] = {0};
    header[2] = 2;
    header[12] = (unsigned char)(w & 0xFF);
    header[13] = (unsigned char)(w >> 8);
    header[14] = (unsigned char)(h & 0xFF);
    header[15] = (unsigned char)(h >> 8);
    header[16] = 24;
    header[17] = 0;
    fwrite(header, 1, 18, fp);

    for (int i = 0; i < w * h; i++)
    {
        unsigned char bgr[3] = {rgb[(size_t)i * 3 + 2],
                                rgb[(size_t)i * 3 + 1],
                                rgb[(size_t)i * 3 + 0]};
        fwrite(bgr, 1, 3, fp);
    }
    fclose(fp);
    printf("已保存截图: %s  (%dx%d)\n", filename, w, h);
}

/* ---------------- 绘图 ---------------- */
void draw()
{
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    gluOrtho2D(-1.3, 1.3, -1.3, 1.3);

    // ① 控制多边形（灰色虚线感：用短线画）
    glColor3d(0.75, 0.75, 0.75);
    glLineWidth(1.0);
    glBegin(GL_LINES);
    for (int k = 0; k < 4; k++)
        for (int i = 0; i < 3; i++)
        {
            glVertex2d(HEART[k][i].x, HEART[k][i].y);
            glVertex2d(HEART[k][i + 1].x, HEART[k][i + 1].y);
        }
    glEnd();

    // ② 控制点（小灰圆点）
    const int SEG = 24;
    for (int k = 0; k < 4; k++)
        for (int i = 0; i < 4; i++)
        {
            glColor3d(0.45, 0.45, 0.45);
            glBegin(GL_POLYGON);
            for (int m = 0; m < SEG; m++)
            {
                double a = 2 * 3.1415926535897 * m / SEG;
                glVertex2d(HEART[k][i].x + 0.025 * cos(a),
                           HEART[k][i].y + 0.025 * sin(a));
            }
            glEnd();
        }

    // ③ 爱心曲线（红色，粗一点）
    glColor3d(0.90, 0.15, 0.25);
    glLineWidth(3.0);
    for (int k = 0; k < 4; k++)
    {
        glBegin(GL_LINE_STRIP);
        for (int i = 0; i <= 200; i++)
        {
            P p = bezier(HEART[k], (double)i / 200);
            glVertex2d(p.x, p.y);
        }
        glEnd();
    }

    glFlush();

    static bool saved = false;
    if (!saved)
    {
        saved = true;
        save_screenshot("运行截图.tga");
    }
}

/* ---------------- 键盘 ---------------- */
void keyboard(unsigned char key, int, int)
{
    if (key == 27)
        exit(0);
    if (key == 's' || key == 'S')
        save_screenshot("运行截图.tga");
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA);
    glutInitWindowPosition(200, 120);
    glutInitWindowSize(600, 600);
    glutCreateWindow("Heart made of 4 cubic Bezier curves");
    glutDisplayFunc(&draw);
    glutKeyboardFunc(&keyboard);
    glutMainLoop();
    return 0;
}
