# 复杂曲线练习 —— 9 个控制点的三次均匀 B 样条

在 **例题3-4**（4 个控制点的三次 B 样条）基础上做的推广练习。

## 和例题 3-4 的区别

| | 例题3-4 | 本练习 |
|---|---|---|
| 控制点数 | 4 | **9** |
| 节点向量 | `0,1,2,...,10` | `0,1,2,...,12`（随控制点数自动生成） |
| 有效参数区间 | `[3, 4]` | **`[3, 9]`** |
| 额外绘制 | 只有曲线 | 坐标轴 + **控制多边形** + **控制点** |

算法完全一样（de Boor-Cox 递归基函数），只是把写死的 4 个点换成 `NP` 个：

```cpp
static const int NP  = 9;   // 控制点个数
static const int DEG = 3;   // 次数

// 有效参数区间是 [t_DEG, t_NP] = [3, 9]
for (float t = DEG; t <= NP; t += ...) { ... }
```

## 能看出 B 样条的两个性质

1. **不经过控制点**：曲线只被"拉"向控制多边形，不穿过它
2. **局部支撑**：改动一个控制点，只影响它附近几段曲线（对比 Bezier——改一个点整条曲线都变）

## 编译运行

```bash
cd ~/cpp-graphics/homework/复杂曲线
g++ -std=c++20 main.cpp -lglut -lGLU -lGL -o curve
./curve          # 弹窗；ESC 退出，S 保存截图
```

依赖（本机已装）：`freeglut3-dev`、`libglu1-mesa-dev`、`libgl1-mesa-dev`

### 保存运行截图

程序启动后**第一次绘制完会自动存一张 `运行截图.tga`**，之后随时按 **`S`** 可再存。
用的是 `glReadPixels` 抓帧缓冲，再手写一个 18 字节 TGA 头存盘（不依赖任何截图工具）：

```cpp
glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, rgb.data());
// RGB -> BGR，写 TGA 头 + 像素数据
```

> `运行结果.png` 是同一条曲线用 Python 单独渲染的预览图（体积小，适合放仓库/报告）。

## 在 Windows / Visual Studio 里

代码里已经做了跨平台处理：

```cpp
#if defined(_WIN32) && __has_include("pch.h")
#include "pch.h"     // VS 工程启用预编译头时需要
#endif
#if defined(_WIN32)
#include <glut.h>        // 你原来的写法
#else
#include <GL/glut.h>
#endif
```

所以在 VS 里新建一个带 GLUT 配置的工程，把这个 `main.cpp` 放进去即可（和你例题3-3/3-4 的工程配置一样）。

## 可以继续玩的方向

- 把 `NP` 改成更大（比如 15），看曲线怎么变
- 改控制点坐标，观察**局部性**：只动中间一个点，看曲线哪一段变了
- 把 `DEG` 改成 2（二次）或 4（四次），看次数的效果
- 加一组**闭合（周期）B 样条**，首尾相接绕成一圈
