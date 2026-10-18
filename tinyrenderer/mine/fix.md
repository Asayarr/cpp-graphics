# fix.md —— 教程代码适配当前仓库的注意事项

> 一句话：**教程页 / 老分支的代码是按 `vec3` 写的，本仓库的 `model.h` 返回 `vec4`。**
> 看到 `could not convert 'vec<4>' to 'vec<3>'`，就是这个问题。

---

## 1. 为什么会这样

作者在后续章节（相机、齐次坐标、MVP 矩阵）把 `Model` 改成了用 **4 维齐次坐标**，
但教程页面上的示例代码和早期分支还停留在 `vec3` 版本。

| 来源 | `Model::vert()` 返回 |
|---|---|
| 教程页上的代码 | `vec3`（旧） |
| `origin/bresenham` 等早期分支 | `vec3`（该分支自带的 `model.h` 也是 vec3） |
| **本仓库当前 `model.h`** | **`vec4`（新）** |

所以：**教程代码 + 当前 model.h = 类型不匹配**。

---

## 2. 当前仓库的真实签名

先记住这张表（随时用 `grep` 自查）：

```bash
grep -nE 'vec[234] |int (nverts|nfaces)' ~/cpp-graphics/tinyrenderer/model.h
```

```cpp
class Model {
public:
    Model(const std::string filename);

    int  nverts() const;                        // 顶点数
    int  nfaces() const;                        // 三角形数

    vec4 vert(const int i) const;               // 第 i 个顶点        → vec4
    vec4 vert(const int iface, const int nth) const;   // 面 iface 的第 nth 个顶点 → vec4

    vec4 normal(const int iface, const int nth) const; // 顶点法线          → vec4
    vec4 normal(const vec2 &uv) const;                 // 法线贴图采样      → vec4

    vec2 uv(const int iface, const int nth) const;     // 纹理坐标          → vec2

    const TGAImage& diffuse() const;
    const TGAImage& specular() const;
};
```

---

## 3. 通用改法（三条）

### ① 自定义函数收 `const vec4&`

```cpp
// 旧
std::tuple<int,int> project(vec3 v) { ... }

// 新
std::tuple<int,int> project(const vec4& v) { ... }
```

### ② 确实需要 3 维时，用 `.xyz()`

`vec<4>` 在 `geometry.h` 里提供了成员函数：

```cpp
vec<2> xy()  const { return {x, y};    }
vec<3> xyz() const { return {x, y, z}; }
```

```cpp
vec3 p = model.vert(i).xyz();     // vec4 → vec3
vec2 t = model.uv(i, 0);          // 本来就是 vec2
```

### ③ 需要转回 vec4 时，手动补 `w`

```cpp
vec3 r = Ry * v.xyz();            // 3×3 矩阵乘 vec3，没问题
return {r.x, r.y, r.z, 1.0};      // 补上齐次坐标 w=1
```

---

## 4. 报错 → 原因 → 改法

| 报错 | 原因 | 改法 |
|---|---|---|
| `could not convert 'vec<4>' to 'vec<3>'` | 函数参数是 `vec3`，传入了 `vec4` | 参数改 `const vec4&`，或调用加 `.xyz()` |
| `no matching function for call to 'std::vector<vec<4>>::push_back(vec3&)'` | 你复制了**旧版 `model.cpp`** 的构造代码 | 别复制 `model.cpp`，直接用仓库自带的 |
| `no declaration matches 'vec3 Model::vert(int) const'` | 同上：头文件是 vec4，实现是 vec3 | 同上 |
| `multiple definition of 'Model::Model(...)'` | 你把 `Model` 的实现也写进了 `main.cpp`，而 Makefile 已经链接了 `model.cpp` | 删掉重复实现 |
| `undefined reference to 'main'` | 你复制的是 `model.cpp`（没有 main）当成 `main.cpp` | 确认复制的是 `main.cpp` |
| `undefined reference to 'main'`（多个 main 时是另一条） | 同目录有两个 `main()` | 一个目录只能有一个 `main()` |

---

## 5. 三个真实踩坑记录

### 案例 A：`01_answer`（作者的 Bresenham 解答）

```cpp
// 报错
std::tuple<int, int> project(vec3 v) { ... }
auto [ax, ay] = project(model.vert(i, 0));   // vert 返回 vec4

// 修改
std::tuple<int, int> project(const vec4& v) { ... }
vec4 v = model.vert(i);                     // 原来写的是 vec3 v
```

### 案例 B：`04_z-buffer`

```cpp
// 报错
std::tuple<int,int,int> project(vec3 v) { ... }

// 修改
std::tuple<int,int,int> project(const vec4 &v) { ... }
```

### 案例 C：`05_rotating`（绕 Y 轴旋转）

```cpp
// 报错
vec3 rot(vec3 v) {
    constexpr mat<3,3> Ry = {...};
    return Ry * v;
}

// 修改
vec4 rot(const vec4 &v) {
    constexpr mat<3,3> Ry = {...};
    vec3 r = Ry * v.xyz();        // 丢掉 w，用 3×3 旋转
    return {r.x, r.y, r.z, 1.0};  // 补回 w
}
```

---

## 6. 相关坑（顺带记一下）

### 6.1 `model.h` / `model.cpp` 是各章共用的，**不要改、不要复制**

- 每章的 `Makefile` 都会自动链接 `tinyrenderer/model.cpp`
- 你把 `Model` 的实现抄进自己的 `main.cpp` → `multiple definition`
- 想给某一章加功能（比如排序）→ **写在 `main.cpp` 里**，参考 `04_sort_hidden_faces/main.cpp`

### 6.2 一个章节目录只能有一个 `main()`

复制上一章时最容易忘：旧的 `main()` 没删，下面又加新的 →
`conflicting declaration of C function 'int main(int, char**)'`

### 6.3 编译产物位置

`app`、`framebuffer.tga`、`zbuffer.tga` 都**写在你运行程序的当前目录**。
建议进到对应章节目录再运行：

```bash
cd ~/cpp-graphics/tinyrenderer/mine/<章节>
./app ../../obj/diablo3_pose/diablo3_pose.obj
```

### 6.4 `constexpr` 里不能用 `std::cos` / `std::sin`

**报错**：`C2131: 表达式必须含有常量值`（MSVC / VS Code IntelliSense）

**原因**：`std::cos` / `std::sin` 在 **C++26 之前都不是 `constexpr`**，
不能出现在 `constexpr` 变量的初始化里。

> 坑点：GCC 新版对 constexpr 数学有扩展，**g++ 可能能编过**，
> 但 MSVC 和 IntelliSense 会报错。所以别依赖它。

```cpp
// ❌ 错（MSVC / IntelliSense 报 C2131）
constexpr double a = M_PI / 6;
constexpr mat<3,3> Ry = {{{std::cos(a), 0, std::sin(a)}, ...}};

// ✅ 对：矩阵降成 const，运行时算一次
constexpr double a = M_PI / 6;
const double c = std::cos(a);
const double s = std::sin(a);
const mat<3,3> Ry = {{{c, 0, s}, {0, 1, 0}, {-s, 0, c}}};
```

如果确实需要编译期常量，可以用字面量（例如 30°）：

```cpp
constexpr double c = 0.8660254037844386;  // cos(30°)
constexpr double s = 0.5;                 // sin(30°)
constexpr mat<3,3> Ry = {{{c, 0, s}, {0, 1, 0}, {-s, 0, c}}};
```

---

## 7. 速查卡

```
Model::vert()   → vec4    ──┬─ .xyz() → vec3
Model::normal() → vec4    ──┘
Model::uv()     → vec2

vec3 → vec4：{v.x, v.y, v.z, 1.0}
vec4 → vec3：v.xyz()
vec4 → vec2：v.xy()

3×3 矩阵 × vec3  ✅ 支持
4×4 矩阵 × vec4  ✅ 支持
```

**报错关键词**：`could not convert 'vec<4>' to 'vec<3>'` → 就是本文档说的问题。
