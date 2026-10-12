/*************************************************************
 @Description: 练习 —— 任意个控制点的三次均匀 B 样条曲线
               例题3-4 只有 4 个控制点；这里推广到 9 个，用同一套
               de Boor-Cox 递归基函数求值。
               同时画出「控制多边形 + 控制点」，可以直观看到 B 样条
               的两个重要性质：
                 1) 曲线不经过控制点（只被"拉"向控制多边形）
                 2) 局部支撑 —— 改动一个控制点，只影响附近几段曲线
 @编译: g++ -std=c++20 main.cpp -lglut -lGLU -lGL -o curve
 @运行: ./curve        （弹出窗口；按 ESC 或关闭窗口退出）
**************************************************************/

#define GL_SILENCE_DEPRECATION

/* ---------- 跨平台头文件 ---------- */
#if defined(_WIN32) && __has_include("pch.h")
#include "pch.h" // Windows VS 环境（若工程启用了预编译头）
#endif

#if defined(_WIN32)
#include <glut.h> // 你原来在 VS 里的写法
#else
#include <GL/glut.h> // Linux / macOS
#endif

#include <cmath>
#include <cstdio>
#include <vector>

#define RGB 255
#define PI 3.1415926535897

/* ---------------- 参数 ---------------- */
static const int NP = 9;  // 控制点个数（例题3-4 是 4）
static const int DEG = 3; // 曲线次数：三次

struct point
{
    float x, y;
};

/* ---------------- 曲线类 ---------------- */
class B_line
{
private:
    point control[NP];            // 控制顶点
    float t_[NP + DEG + 1];       // 均匀节点向量：t_[i] = i
public:
    void control_point_input();   // 输入控制点 + 生成节点
    float N_cal(int k, int i, float t); // de Boor-Cox 基函数 N_{i,k}(t)
    point p_cal(float t);               // 求参数 t 处的曲线点
    friend void draw();
};

B_line problem;
void draw();
void keyboard(unsigned char key, int, int);
void save_screenshot(const char *filename);

/* ---------------- 保存当前窗口画面为 TGA 图片 ----------------
   按 S 键（或在第一次绘制后自动）保存，方便写实验报告 */
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
    header[2] = 2; // 未压缩真彩色
    header[12] = (unsigned char)(w & 0xFF);
    header[13] = (unsigned char)(w >> 8);
    header[14] = (unsigned char)(h & 0xFF);
    header[15] = (unsigned char)(h >> 8);
    header[16] = 24; // 24 位
    header[17] = 0;  // 左下角为原点（与 glReadPixels 的行顺序一致）
    fwrite(header, 1, 18, fp);

    // glReadPixels 给的是 RGB，TGA 要 BGR
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

/* ---------------- 基函数 ----------------
   例题3-4 的原式，补上了分母为 0 的保护（重复节点时才需要） */
float B_line::N_cal(int k, int i, float t)
{
    if (k == 0)
        return (t >= t_[i] && t < t_[i + 1]) ? 1.0f : 0.0f;

    float d1 = t_[i + k] - t_[i];
    float d2 = t_[i + k + 1] - t_[i + 1];

    float a = (d1 > 0) ? (t - t_[i]) * N_cal(k - 1, i, t) / d1 : 0.0f;
    float b = (d2 > 0) ? (t_[i + k + 1] - t) * N_cal(k - 1, i + 1, t) / d2 : 0.0f;
    return a + b;
}

/* ---------------- 曲线求值 ---------------- */
point B_line::p_cal(float t)
{
    point r;
    r.x = 0.0f;
    r.y = 0.0f;
    for (int i = 0; i < NP; i++)
    {
        float b = N_cal(DEG, i, t); // 第 i 个控制点的权重
        r.x += b * control[i].x;
        r.y += b * control[i].y;
    }
    return r;
}

/* ---------------- 控制点数据 ---------------- */
void B_line::control_point_input()
{
    // 9 个控制点，摆成一个上下起伏的造型（"W" 形）
    control[0].x = -5.5f; control[0].y =  0.0f;
    control[1].x = -4.5f; control[1].y =  2.5f;
    control[2].x = -3.0f; control[2].y = -2.5f;
    control[3].x = -1.5f; control[3].y =  2.5f;
    control[4].x =  0.0f; control[4].y = -2.5f;
    control[5].x =  1.5f; control[5].y =  2.5f;
    control[6].x =  3.0f; control[6].y = -2.5f;
    control[7].x =  4.5f; control[7].y =  2.5f;
    control[8].x =  5.5f; control[8].y =  0.0f;

    // 均匀节点向量：0, 1, 2, ... , NP+DEG
    for (int i = 0; i < NP + DEG + 1; i++)
        t_[i] = (float)i;
}

/* ---------------- 绘图 ---------------- */
void draw()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // 白底
    glClear(GL_COLOR_BUFFER_BIT);
    gluOrtho2D(-7, 7, -6, 6); // 设置可视范围

    // ① 坐标轴（浅灰）
    glColor3f(0.85f, 0.85f, 0.85f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    glVertex2f(-100.0f, 0.0f);
    glVertex2f(100.0f, 0.0f);
    glVertex2f(0.0f, -100.0f);
    glVertex2f(0.0f, 100.0f);
    glEnd();

    // ② 控制多边形（灰）
    glColor3f(0.55f, 0.55f, 0.55f);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < NP; i++)
        glVertex2f(problem.control[i].x, problem.control[i].y);
    glEnd();

    // ③ 控制点（小黑圆点）
    const int SEG = 40;
    for (int i = 0; i < NP; i++)
    {
        glColor3f(0.15f, 0.15f, 0.15f);
        glBegin(GL_POLYGON);
        for (int m = 0; m < SEG; m++)
        {
            float a = 2 * PI * m / SEG;
            glVertex2f(0.10f * cos(a) + problem.control[i].x,
                       0.10f * sin(a) + problem.control[i].y);
        }
        glEnd();
    }

    // ④ B 样条曲线（蓝色）
    //    有效参数区间是 [t_DEG, t_NP] = [3, 9]，超出这段的基函数和为 0
    glColor3f(0.0f, 0.15f, 0.85f);
    glLineWidth(2.5f);
    glBegin(GL_LINE_STRIP);
    const int STEPS = 2000;
    float t0 = (float)DEG;
    float t1 = (float)NP;
    for (int s = 0; s <= STEPS; s++)
    {
        float t = t0 + (t1 - t0) * s / STEPS;
        point pt = problem.p_cal(t);
        glVertex2f(pt.x, pt.y);
    }
    glEnd();

    glFlush();

    // 第一次绘制完成后自动存一张图，方便直接拿去做实验报告
    static bool saved = false;
    if (!saved)
    {
        saved = true;
        save_screenshot("运行截图.tga");
    }
}

/* ---------------- 按 ESC 退出 / 按 S 存图 ---------------- */
void keyboard(unsigned char key, int, int)
{
    if (key == 27) // ESC
        exit(0);
    if (key == 's' || key == 'S')
        save_screenshot("运行截图.tga");
}

/* ---------------- 主函数 ---------------- */
int main(int argc, char *argv[])
{
    problem.control_point_input();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA);
    glutInitWindowPosition(150, 100);
    glutInitWindowSize(700, 600);
    glutCreateWindow("Practice: cubic B-spline with 9 control points");
    glutDisplayFunc(&draw);
    glutKeyboardFunc(&keyboard);
    glutMainLoop(); // 进入事件循环（后面的代码不会执行）
    return 0;
}
