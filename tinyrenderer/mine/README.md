# mine —— 你的 tinyrenderer 实现

这里放你跟着教程自己写的代码。上级目录（`tinyrenderer/`）是 ssloy 的**参考实现**，别动。

## 编译运行

```bash
cd ~/cpp-graphics/tinyrenderer/mine
make          # 编译
make run      # 编译并运行（自动带上模型和地面参数）
```

产物是 `app`，输出 `framebuffer.tga`。看效果：

```bash
python3 ~/cpp-graphics/tga2png.py framebuffer.tga
```

## 说明

- `main.cpp` 已经放好了教程的**起点**代码（三个白点的 64×64 图），从这里开始改。
- `Makefile` 会自动编译本目录下所有 `.cpp`，**并自动链接上级目录的公共文件**
  （`tgaimage.cpp` / `model.cpp` / `our_gl.cpp`），所以不用把它们复制过来。
- 如果你要改 `tgaimage`、`geometry`、`our_gl` 这些公共文件的行为，
  就把它们复制到本目录自己改（注意别和上级的同名文件同时参与链接）。

## 对照作者实现

想看看作者在某一章是怎么写的：

```bash
cd ~/cpp-graphics/tinyrenderer
git show <分支名>:main.cpp          # 例如 git show hidden:main.cpp
```

分支名见 GitHub 仓库 <https://github.com/ssloy/tinyrenderer/branches>
（`bresenham` / `rasterization` / `hidden` / `camera` / `shading` / `textures` / `tangent` / `shadowmap` / `ssao` / `toon` …）。
本地那份 `.git` 为了精简已经删了，需要的话重新 clone 一个到别处即可。
