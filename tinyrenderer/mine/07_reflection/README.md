# 第 7 章：Phong 反射模型

用 `our_gl.h` / `our_gl.cpp` 的渲染管线框架，**只写一个着色器**（shader）。

## 渲染管线

```
顶点数据 → [顶点着色器 vertex()] → 图元装配 → [光栅化] → [片元着色器 fragment()] → 帧缓冲
              ↑ 你写                                ↑ 库写             ↑ 你写
```

- `our_gl.cpp` 里是"硬连线"的部分：`lookat` / `init_perspective` / `init_viewport` /
  `init_zbuffer` / `rasterize`（对应 GPU 里不可编程的部分）
- 你只需要实现 `IShader` 的两个虚函数：`vertex()` 和 `fragment()`

## Phong 反射模型

每个片元的亮度 = 三项相加：

```
I = k_a · 1                          环境光 ambient（常数，模拟间接光）
  + k_d · max(0, n·l)                漫反射 diffuse（Lambert，面越正对光越亮）
  + k_s · (max(0, r·v))^shininess    镜面高光 specular（越接近镜面反射方向越亮）
```

| 符号 | 含义 |
|---|---|
| `n` | 片元法线（单位向量，**眼睛坐标系**） |
| `l` | 指向光源的方向（单位向量） |
| `v` | 片元指向相机的方向（单位向量） |
| `r` | 光的反射方向：`r = 2n(n·l) − l` |
| `shininess` | 高光锐利度：越大越"金属/抛光"，越小越"哑光" |

### 三个系数的效果

| 参数 | 本文件取值 | 调大会怎样 |
|---|---|---|
| `ka` 环境光 | 0.10 | 暗部变亮，整体变平 |
| `kd` 漫反射 | 0.90 | 明暗对比更强 |
| `ks` 高光 | 0.50 | 高光更亮 |
| `shininess` | 64 | 高光更小更锐（像抛光金属） |

## 代码结构（`main.cpp`）

```cpp
struct PhongShader : IShader {
    vec3 tri_nrm[3], tri_eye[3];   // 顶点着色器 → 片元着色器 传递的数据

    vec4 vertex(int face, int vert) {
        // 模型 → 眼睛坐标（ModelView），保存位置和法线
        // 眼睛坐标 → 裁剪坐标（Perspective）
    }

    std::pair<bool,TGAColor> fragment(const vec3 bar) const {
        // bar = 该像素的重心坐标（光栅化器已做好透视校正）
        // 插值出 n 和 p，算 Phong 三项
    }
};
```

注意：
- `model.normal()` 返回的 `vec4` 的 `w = 0`，所以 `ModelView * n` **只旋转、不平移**
- 相机在眼睛坐标系的原点，所以视线方向是 `normalize(-p)`
- `geometry.h` 没有一元 `operator-`，所以写成 `p * -1.0`

## 编译运行

```bash
cd ~/cpp-graphics/tinyrenderer/mine/07_reflection
./app ../../obj/african_head/african_head.obj
python3 ~/cpp-graphics/tga2png.py framebuffer.tga
```

或者：

```bash
trun 07 african_head
```

## 可以玩什么

1. **改 `shininess`**：试试 4 / 16 / 64 / 256，看高光从"一片亮"变成"一个小点"
2. **关掉某一项**：把 `ks` 设成 0（只看漫反射）、`kd` 设成 0（只看高光）
3. **换光的方向**：`LIGHT_DIR` 改成 `{0,0,1}` 或 `{-1,1,0}`
4. **彩色光照**：把 `TGAColor{g,g,g,255}` 换成 `{g*0.8, g, g*1.2, 255}`（注意 BGRA 顺序）
5. **换模型**：`./app ../../obj/diablo3_pose/diablo3_pose.obj`

## 下一步

第 8 章「纹理」会把 `diffuse()` 贴图采样加进 `fragment()` —— 也就是给这个 Phong 模型"上色"，
同时用 `uv()` 插值纹理坐标。管线完全一样，只是 `fragment()` 里多几行。
