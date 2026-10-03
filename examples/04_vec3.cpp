// 04_vec3.cpp  —  图形学最核心的东西：自己写一个 vec3
// 学完这个你就明白 glm 里的 glm::vec3 为什么能写 v1 + v2 * 0.5f
#include <cmath>
#include <iostream>

struct Vec3 {
    float x{}, y{}, z{};

    Vec3() = default;                                   // 默认构造：全 0
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

    // ---------- 向量运算 ----------
    float dot(const Vec3& o) const {                    // 点积
        return x * o.x + y * o.y + z * o.z;
    }
    Vec3 cross(const Vec3& o) const {                   // 叉积
        return {y * o.z - z * o.y,
                z * o.x - x * o.z,
                x * o.y - y * o.x};
    }
    float length() const { return std::sqrt(dot(*this)); }
    Vec3 normalized() const {
        float l = length();
        return l > 0.0f ? Vec3{x / l, y / l, z / l} : Vec3{};
    }

    // ---------- 运算符重载 ----------
    Vec3 operator+(const Vec3& o) const { return {x + o.x, y + o.y, z + o.z}; }
    Vec3 operator-(const Vec3& o) const { return {x - o.x, y - o.y, z - o.z}; }
    Vec3 operator*(float s) const { return {x * s, y * s, z * s}; }
    Vec3 operator-() const { return {-x, -y, -z}; }     // 负号
    Vec3& operator+=(const Vec3& o) {                   // 返回引用以支持链式
        x += o.x; y += o.y; z += o.z;
        return *this;
    }
    bool operator==(const Vec3& o) const {
        return x == o.x && y == o.y && z == o.z;
    }
};

// 交换律：标量写在左边时，只能用非成员函数
Vec3 operator*(float s, const Vec3& v) { return v * s; }

// 让 Vec3 能直接用 std::cout 打印（这也是运算符重载）
std::ostream& operator<<(std::ostream& os, const Vec3& v) {
    return os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
}

int main() {
    Vec3 a{1, 0, 0}, b{0, 1, 0};
    std::cout << "a+b            = " << a + b << "\n";
    std::cout << "a-b            = " << a - b << "\n";
    std::cout << "a·b (dot)      = " << a.dot(b) << "\n";
    std::cout << "a×b (cross)    = " << a.cross(b) << "\n";
    std::cout << "2*a            = " << 2.0f * a << "\n";
    std::cout << "-a             = " << -a << "\n";

    Vec3 n = (a + b).normalized();
    std::cout << "normalize(a+b) = " << n << "  len=" << n.length() << "\n";

    // 模拟一个最简单的渲染循环：位置 += 速度 * dt
    Vec3 pos{0, 0, 0}, vel{1, 2, 3};
    const float dt = 0.5f;
    for (int frame = 0; frame < 3; ++frame) {
        pos += vel * dt;
        std::cout << "frame " << frame << "  pos = " << pos << "\n";
    }
    return 0;
}
