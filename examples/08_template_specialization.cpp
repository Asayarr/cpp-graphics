// 08_template_specialization.cpp
// -----------------------------------------------------------------------------
// tinyrenderer 里最“吓人”的文件是 geometry.h。它本质上是用模板写的一个
// 迷你线性代数库。这个例子把它的关键语法精简到最小，逐块解释：
//   非类型模板参数 / 模板特化 / 运算符重载 / 类型别名 / 普通函数模板
// 看懂这个文件，再回头看真正的 geometry.h 就不会慌。
// 编译: g++ -std=c++20 08_template_specialization.cpp -o 08_template_specialization
// -----------------------------------------------------------------------------
#include <cassert>
#include <cmath>
#include <iostream>

// 【1】非类型模板参数：n 是一个编译期整数，不是类型！
//      Vec<2>、Vec<3>、Vec<4> 是三个不同的类型，编译器各生成一份代码。
template <int n>
struct Vec {
    double data[n] = {0};        // 成员自带默认初始化

    // 【2】同一个 operator[] 写两份：const 对象只能调 const 版本
    double& operator[](int i)       { assert(i >= 0 && i < n); return data[i]; }
    double  operator[](int i) const { assert(i >= 0 && i < n); return data[i]; }
};

// 【3】模板化的“自由”运算符：两个 Vec<n> 相乘 → 点积
//     写成非成员函数，是为了左右操作数类型一样、写起来对称。
template <int n>
double operator*(const Vec<n>& a, const Vec<n>& b) {
    double r = 0;
    for (int i = 0; i < n; ++i) r += a[i] * b[i];
    return r;
}

// 【4】模板特化：当 n==3 时，不用通用版本，改用这个专门版本。
//     好处是可以直接用 .x .y .z，而且物理布局和真实 GLM 一样。
template <>
struct Vec<3> {
    double x = 0, y = 0, z = 0;
    double& operator[](int i)       { return i == 0 ? x : (i == 1 ? y : z); }
    double  operator[](int i) const { return i == 0 ? x : (i == 1 ? y : z); }
};

// 【5】类型别名：给常用实例起短名字（geometry.h 里用的是 typedef，效果一样）
using Vec2 = Vec<2>;
using Vec3 = Vec<3>;

// 【6】普通函数模板：norm 对任意维度的 Vec 都成立
template <int n>
double norm(const Vec<n>& v) { return std::sqrt(v * v); }

// 【7】输出运算符也要模板化
template <int n>
std::ostream& operator<<(std::ostream& os, const Vec<n>& v) {
    os << "(";
    for (int i = 0; i < n; ++i) os << v[i] << (i + 1 < n ? ", " : "");
    return os << ")";
}

int main() {
    Vec2 a;              // 走通用模板 Vec<2>，只能用 a[i]
    a[0] = 3.0;
    a[1] = 4.0;

    Vec3 b{1, 2, 2};     // 走特化版本 Vec<3>，可以直接 b.x / b[i]

    std::cout << "a        = " << a << "\n";
    std::cout << "norm(a)  = " << norm(a) << "\n";   // 3-4-5 直角三角形
    std::cout << "b        = " << b << "\n";
    std::cout << "b·b      = " << b * b << "\n";
    std::cout << "b.x=" << b.x << "  b[1]=" << b[1] << "  b[2]=" << b[2] << "\n";
    return 0;
}
