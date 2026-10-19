# cpp-graphics

![C++](https://img.shields.io/badge/C%2B%2B-20-00599C?logo=cplusplus&logoColor=white)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

跟随 [TinyRenderer](https://haqr.eu/tinyrenderer/) 教程，用 C++ 从零实现一个**软件渲染器**。

仓库里既有原作者（[ssloy](https://github.com/ssloy/tinyrenderer)）的参考实现与素材，
也有我按章节自己写的实现，以及过程中踩坑的记录。

## 目录

- [项目简介](#项目简介)
- [目录结构](#目录结构)
- [环境要求](#环境要求)
- [快速开始](#快速开始)
- [章节说明](#章节说明)
- [工具](#工具)
- [踩坑记录](#踩坑记录)
- [参考](#参考)
- [许可证](#许可证)

## 项目简介

从「画一条直线」开始，一步步实现：

```
Bresenham 直线 → 三角形光栅化 → 重心坐标插值 → z-buffer
→ 相机与透视投影 → MVP 矩阵 → Phong 反射模型
```

全程**不依赖任何图形库**（不用 OpenGL / Vulkan），图像直接写进内存并保存为 TGA。
目的是理解 GPU 渲染管线每一步到底在做什么。

## 目录结构

```text
cpp-graphics/
├── tinyrenderer/                  # 原作者的参考实现与素材
│   ├── main.cpp / our_gl.* / model.* / tgaimage.* / geometry.h
│   ├── obj/                       # 模型与贴图（african_head、diablo3_pose、floor）
│   └── mine/                      # ★ 我自己按章节写的实现
│       ├── 01_bresenham/ … 07_reflection/
│       ├── Makefile
│       ├── README.md
│       └── fix.md                 # 教程代码适配本仓库的踩坑记录
├── tools/
│   ├── trun.sh                    # 编译 + 运行 + 转 PNG，一条命令
│   └── build-current.sh           # VS Code F5 用的构建助手
├── tga2png.py                     # TGA → PNG（彩色 / 灰度都支持）
├── Makefile                       # 根入口，转发到 tinyrenderer/mine
├── .vscode/                       # 编辑器配置（任务 / 调试 / IntelliSense）
├── LICENSE
└── README.md
```

## 环境要求

| 依赖 | 版本 | 说明 |
|---|---|---|
| C++ 编译器 | 支持 C++20 | GCC 11+ / Clang 14+ / MSVC 19.3+ |
| GNU Make | 任意 | 构建章节 |
| Python | 3.6+ | 仅 `tga2png.py` 使用（纯标准库） |

渲染结果是 TGA，多数看图软件能直接打开；想转 PNG 用 `tga2png.py`。

## 快速开始

```bash
git clone https://github.com/Asayarr/cpp-graphics.git
cd cpp-graphics

# 编译 + 运行 + 转 PNG，一条命令
./tools/trun.sh 07_reflection african_head

# 或者编译 / 运行全部章节
make
make run
```

在 VS Code 里：

- 打开某个章节的 `main.cpp`，按 **F5** 编译并调试
- `make all` 任务等价于根目录 `make`

## 章节说明

| 目录 | 内容 |
|------|------|
| `01_bresenham` | Bresenham 直线绘制 |
| `01_wireframe` | 线框渲染（第 1 章作业） |
| `01_answer` | 作者的线框解答（参考） |
| `02_triangle_rasterization` | 三角形光栅化：先画轮廓 |
| `02_modern_rasterization` | 有向面积 + 重心坐标填充 |
| `02_homework` | 模型渲染（第 2 章作业） |
| `03_barycentric_coordinates` | 重心坐标插值（灰度三角形） |
| `03_homework` | 彩色渐变三角形（第 3 章作业） |
| `04_sort_hidden_faces` | 画家算法：按深度排序 |
| `04_z_buffer` | z-buffer（8 位灰度版） |
| `05_rotating` | 绕 Y 轴旋转模型 |
| `05_central_projection` | 中心（透视）投影 |
| `05_camera_naive` | 旋转 + 透视 + 浮点 z-buffer（修复 8 位溢出 bug） |
| `06_better_camera` | MVP 矩阵 + 相机 |
| `07_refactoring` | 渲染管线重构（顶点 / 片元着色器拆分） |
| `07_reflection` | Phong 反射模型（环境光 + 漫反射 + 高光） |

每个目录都是**独立的可执行程序**，互不依赖。

## 工具

### `tools/trun.sh`

```bash
./tools/trun.sh 07_reflection              # 默认用人脸模型
./tools/trun.sh 07_reflection diablo3_pose # 换模型
./tools/trun.sh 04_z                       # 前缀匹配
./tools/trun.sh                            # 最近修改的一章
```

自动完成：编译 → 带模型路径运行 → 把 `framebuffer.tga` / `zbuffer.tga` 转成 PNG。

### `tga2png.py`

```bash
python3 tga2png.py framebuffer.tga            # → framebuffer.png
python3 tga2png.py zbuffer.tga out.png        # 灰度深度图也支持
```

纯 Python 标准库实现，不依赖 Pillow / ImageMagick。

### `tools/build-current.sh`

VS Code 的 F5 会调用它，自动判断"当前文件属于单文件还是多文件项目"，
并自动链接 `tinyrenderer/` 下的 `tgaimage.cpp` / `model.cpp` / `our_gl.cpp`。

## 踩坑记录

[`tinyrenderer/mine/fix.md`](tinyrenderer/mine/fix.md) 记录了教程代码与本仓库版本不一致时的各种坑：

- `Model::vert()` 返回 `vec4` 而不是 `vec3`
- `constexpr` 里不能用 `std::cos` / `std::sin`（C2131）
- `multiple definition of 'lookat'`（和 `our_gl.cpp` 撞车）
- 一个目录只能有一个 `main()`
- `geometry.h` 没有一元 `operator-`

## 参考

- TinyRenderer 教程：<https://haqr.eu/tinyrenderer/>
- 原仓库：<https://github.com/ssloy/tinyrenderer>
- LearnOpenGL：<https://learnopengl.com/>（[中文版](https://learnopengl-cn.github.io/)）

## 许可证

自写部分基于 [MIT License](LICENSE) 开源。

`tinyrenderer/` 目录来自 [ssloy/tinyrenderer](https://github.com/ssloy/tinyrenderer)，
同样基于 MIT License（原始许可证见 `tinyrenderer/LICENSE.txt`）。
