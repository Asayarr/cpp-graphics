# mine —— 你的 tinyrenderer 实现

这里放你跟着教程自己写的代码。上级目录（`tinyrenderer/`）是 ssloy 的**参考实现**，别动。

> ⚠️ **编译报错先看 [`fix.md`](fix.md)** —— 里面记了教程代码和当前仓库版本不匹配的坑
> （最常见的：`could not convert 'vec<4>' to 'vec<3>'`）。

## 目录约定：每一章一个子目录

```
mine/
├── 01_bresenham/
│   └── main.cpp        ← 第 1 章的代码，就写在这
├── 02_rasterization/
│   └── main.cpp
└── ...
```

- ⚠️ 目录名**不要有空格和引号**（终端里会很难受），用 `01_bresenham` 这种
- 每个子目录里**只能有一个 `main()`**
- 新开一章：复制上一章的 `main.cpp` 到新目录，接着改（这样每章都留档）

## 编译运行

**方式一：VS Code 里按 F5**
在 `main.cpp` 里按 F5，会自动编译（自动带上 `tinyrenderer/` 的公共文件）并启动 gdb。

**方式二：命令行**

```bash
cd ~/cpp-graphics/tinyrenderer/mine
make                     # 编译全部章节
make 01_bresenham/app    # 只编译某一章
make run                 # 编译并运行全部

# 或者进到某一章
cd 01_bresenham && make -C .. 01_bresenham/app && ./app ../../obj/diablo3_pose/diablo3_pose.obj
```

看渲染结果：

```bash
python3 ~/cpp-graphics/tga2png.py 01_bresenham/framebuffer.tga
```

## 说明

- 公共文件（`tgaimage` / `model` / `our_gl`）**不用复制**，构建时自动包含上级目录的。
- 如果你要改 `tgaimage`、`geometry`、`our_gl` 的行为，就把它们复制到本章目录自己改
  （注意别和上级的同名文件同时参与链接）。

## 对照作者实现

```bash
git -C ~/tinyrenderer-ref show origin/<分支>:main.cpp
```

分支：`bresenham` / `rasterization` / `hidden` / `camera` / `shading-flat` / `phong` /
`textures` / `tangent` / `shadowmap` / `ambient_occlusion` / `ssao` / `toon`
