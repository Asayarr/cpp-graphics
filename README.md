# cpp-graphics

![C++](https://img.shields.io/badge/C%2B%2B-20-00599C?logo=cplusplus&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-3.16%2B-064F8C?logo=cmake&logoColor=white)
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20macOS%20%7C%20Windows-informational)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

> 面向 C++ 与计算机图形学入门的可运行示例集。
> A collection of minimal, runnable C++ examples for learning computer graphics.

本仓库面向 **具备 C 语言基础、熟悉 Python / C#**，希望快速掌握 C++ 基础语法并进入计算机图形学的学习者。

每个示例都是一个可独立编译运行的最小程序，内容从语言基础逐步过渡到渲染循环骨架，
并着重介绍图形学中高频使用的 C++ 特性（引用与值语义、RAII、运算符重载、模板与特化、智能指针、多态）。

`examples/` 为教程示例，`mycode/` 用于存放跟随学习时自己编写的练习代码。

## 目录

- [特性](#特性)
- [环境要求](#环境要求)
- [快速开始](#快速开始)
- [项目结构](#项目结构)
- [示例说明](#示例说明)
- [构建](#构建)
- [调试](#调试)
- [工具](#工具)
- [延伸学习](#延伸学习)
- [许可证](#许可证)

## 特性

- **递进式示例**：8 个由浅入深的 C++ 示例，每个均可单独编译运行。
- **图形学导向**：覆盖图形学中最常用的语言特性，而非泛泛的语言教程。
- **手写 vec3**：通过实现向量类型理解 `glm` 等数学库的运算符重载原理。
- **渲染循环骨架**：给出典型实时渲染主循环的结构，便于衔接 OpenGL / TinyRenderer 等后续内容。
- **零配置构建**：同时提供 Makefile 与 CMake 两种构建方式，新增源文件会被自动纳入构建。
- **配套工具**：内置零依赖的 TGA → PNG 转换脚本，便于查看软件渲染输出。
- **编辑器集成**：内置 VS Code 的构建、运行与调试配置。

## 环境要求

| 依赖 | 版本 | 说明 |
|------|------|------|
| C++ 编译器 | 支持 C++20 | GCC 11+ / Clang 14+ / MSVC 19.3+ |
| GNU Make | 任意 | 使用 Makefile 构建时需要 |
| CMake | 3.16+ | 可选，使用 CMake 构建时需要 |
| Python | 3.6+ | 可选，仅用于 `tga2png.py`（纯标准库，无第三方依赖） |

## 快速开始

```bash
git clone https://github.com/Asayarr/cpp-graphics.git
cd cpp-graphics

make            # 编译全部示例
make run        # 编译并运行
```

也可以使用 CMake：

```bash
cmake -B build
cmake --build build
```

## 项目结构

```text
cpp-graphics/
├── examples/                     # 教程示例
│   ├── 01_hello.cpp
│   ├── ...
│   ├── 08_template_specialization.cpp
│   └── multifile/                # 多文件编译示范（.h / .cpp / main.cpp）
├── mycode/                       # 你自己的练习代码
│   └── README.md
├── CMakeLists.txt
├── Makefile
├── tga2png.py                    # TGA → PNG 转换脚本
├── setup-tinyrenderer.sh         # TinyRenderer 教程项目初始化脚本
├── .vscode/                      # 编辑器配置
└── README.md
```

## 示例说明

| 文件 | 主题 | 主要内容 |
|------|------|----------|
| `examples/01_hello.cpp` | 基础 I/O | 编译模型、`std::cout` / `std::cin`、命名空间 |
| `examples/02_basics.cpp` | 语言基础 | 初始化、`auto`、引用与指针、`const`、容器、范围 `for`、传参方式 |
| `examples/03_classes.cpp` | 类与对象 | 构造 / 析构、成员初始化列表、RAII、运算符重载、`const` 成员函数 |
| `examples/04_vec3.cpp` | 数学类型 | 手写 `vec3`：点积、叉积、归一化、完整运算符重载 |
| `examples/05_polymorphism.cpp` | 资源与多态 | `unique_ptr` / `shared_ptr`、继承、虚函数、运行期多态 |
| `examples/06_lambdas_templates.cpp` | 现代特性 | 函数模板、类模板、lambda 捕获、`std::function` |
| `examples/07_graphics_skeleton.cpp` | 渲染骨架 | 典型渲染主循环结构（窗口、资源、更新、绘制） |
| `examples/08_template_specialization.cpp` | 模板进阶 | 非类型模板参数、模板特化 —— 理解 `geometry.h` 的关键 |
| `examples/multifile/` | 多文件工程 | 头文件 / 源文件分离与编译链接流程 |

运行单个示例：

```bash
make examples/04_vec3
./examples/04_vec3
```

## 构建

### Makefile

```bash
make                        # 编译 examples/ 与 mycode/ 下全部 .cpp
make run                    # 编译并运行
make examples/04_vec3       # 仅编译指定文件
make mycode/<your_file>     # 编译你自己的代码
make clean                  # 清理构建产物
```

Makefile 会自动发现 `examples/` 与 `mycode/` 下的新 `.cpp` 文件，无需修改。

### CMake

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

`examples/` 与 `mycode/` 下的每个 `.cpp` 会生成一个同名可执行文件；
多文件示范生成 `multifile_demo`。新增文件后重新执行一次 configure 即可。

## 调试

仓库内置 VS Code 配置（`.vscode/`）：

- **F5**：编译并调试当前打开的源文件（GDB）；
- **Ctrl+Shift+B**：编译全部示例；
- 任务面板中提供 `build current file` 与 `run current file`。

在其他环境下，可直接使用命令行：

```bash
g++ -std=c++20 -Wall -Wextra -g examples/02_basics.cpp -o /tmp/02_basics
gdb /tmp/02_basics
```

## 工具

`tga2png.py` 用于将 TGA 图像转换为 PNG（纯 Python 标准库实现，无需 Pillow / ImageMagick），
常用于查看软件渲染器的输出：

```bash
python3 tga2png.py framebuffer.tga            # 生成 framebuffer.png
python3 tga2png.py framebuffer.tga output.png # 指定输出文件名
```

## 延伸学习

- [TinyRenderer](https://github.com/ssloy/tinyrenderer) —— 用 500 行 C++ 实现软件渲染器，理解 OpenGL 的底层机制
- [LearnOpenGL](https://learnopengl.com/) —— 系统性的现代 OpenGL 教程（[中文版](https://learnopengl-cn.github.io/)）
- [glm](https://github.com/g-truc/glm) —— OpenGL 数学库，本仓库 `04_vec3` 可视为其简化版
- [cppreference](https://en.cppreference.com/) —— C++ 标准库参考

## 许可证

本项目基于 [MIT License](LICENSE) 开源。
