// 03_classes.cpp  —  类 / 构造 / 析构 / RAII / 运算符重载 / const 成员函数
#include <iostream>
#include <string>

// struct 和 class 在 C++ 里几乎一样，唯一区别：
//   struct 默认 public，class 默认 private。
struct Vec2 {
    float x{}, y{};   // {} 表示默认初始化为 0
};

// 一个“持有资源”的类：RAII = Resource Acquisition Is Initialization
// 构造时获取资源，析构时释放资源，靠作用域自动管理，不用手动 free
class Buffer {
public:
    // 构造函数 + 成员初始化列表（冒号后面，比在函数体里赋值更高效）
    explicit Buffer(int n) : size_(n), data_(new float[n]{}) {
        std::cout << "  Buffer(" << n << ") 分配了 " << n << " 个 float\n";
    }

    // 析构函数：离开作用域自动调用 —— RAII 的基石
    ~Buffer() {
        delete[] data_;
        std::cout << "  ~Buffer() 释放\n";
    }

    // 禁止拷贝：否则两个对象指向同一块内存，会 delete 两次
    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;

    // 运算符重载：让自定义类型像内置类型一样用
    float& operator[](int i) { return data_[i]; }              // 可写版本
    float operator[](int i) const { return data_[i]; }         // 只读版本

    int size() const { return size_; }   // 不修改成员 → 末尾加 const

private:
    int size_;
    float* data_;
};

int main() {
    std::cout << "进入作用域\n";
    {
        Buffer buf(4);
        buf[0] = 3.14f;                 // 调用 operator[]
        std::cout << "  buf[0]=" << buf[0]
                  << " size=" << buf.size() << "\n";
    }   // <-- 到这里自动调用 ~Buffer()，不需要手动释放

    std::cout << "离开作用域\n";

    // const 对象只能调用 const 成员函数
    const Buffer cb(2);
    std::cout << "cb[0]=" << cb[0] << " cb.size()=" << cb.size() << "\n";

    Vec2 v{1.0f, 2.0f};
    std::cout << "Vec2 = (" << v.x << ", " << v.y << ")\n";
    return 0;
}
