// 02_basics.cpp  —  C 的指针 + C# 的值/引用语义，在 C++ 里怎么合体
#include <algorithm>
#include <array>
#include <iostream>
#include <string>
#include <vector>

// ---------- 三种传参方式（图形学里天天用）----------
void byValue(int x) { x = 100; (void)x; }            // 拷贝一份，改不到外面
void byRef(int& x) { x = 100; }                      // 引用：改到外面
void byConstRef(const std::vector<int>& v) {         // 只读且不拷贝（推荐）
    std::cout << "byConstRef 收到 " << v.size() << " 个元素\n";
}

int main() {
    // 1) 四种初始化写法
    int a = 1;     // C 风格
    int b(2);      // 构造风格
    int c{3};      // 列表初始化（推荐，能防止隐式窄化，如 int c{3.5} 会报错）
    int d{};       // 默认 0
    std::cout << a << " " << b << " " << c << " " << d << "\n";

    // 2) auto：编译期类型推导（不是 Python 那种动态类型！）
    auto i = 10;                    // int
    auto f = 1.5;                   // double
    auto s = std::string("hi");     // std::string
    std::cout << i << " " << f << " " << s << "\n";

    // 3) 引用 vs 指针
    int v = 5;
    int& r = v;      // 引用：必须初始化；不能改指向；没有 null
    int* p = &v;     // 指针：就是 C 那一套，可以为空、可以改指向
    r = 7;           // 等价于 v = 7
    *p = 9;
    std::cout << "v=" << v << " r=" << r << " *p=" << *p << "\n";

    // 4) const 正确性
    const int k = 42;
    const int* pc = &k;   // 指向常量的指针：*pc 不能改
    int* const cp = &v;   // 常量指针：cp 不能改指向
    // *pc = 1;   // 编译错误
    // cp = &a;   // 编译错误
    std::cout << "k=" << k << " *pc=" << *pc << " *cp=" << *cp << "\n";

    // 5) 容器：替代 C 的裸数组
    std::vector<int> nums{1, 2, 3, 4, 5};    // 动态数组（可增长）
    nums.push_back(6);
    std::array<int, 3> fixed{10, 20, 30};    // 定长数组，等价 int[3] 但更安全
    std::string str = "graphics";            // 字符串是真正的类型

    // 6) 范围 for：遍历容器的标准姿势
    for (int n : nums) std::cout << n << " ";
    std::cout << "\n";
    for (int& n : nums) n *= 2;                       // 用引用才能修改
    for (const auto& n : nums) std::cout << n << " "; // 只读 + 不拷贝
    std::cout << "\n";

    // 7) 三种传参的实际效果
    int x = 1;
    byValue(x);  std::cout << "byValue 后: " << x << "\n";   // 仍是 1
    byRef(x);    std::cout << "byRef   后: " << x << "\n";   // 变成 100
    byConstRef(nums);

    std::cout << "size=" << nums.size()
              << " str=" << str
              << " fixed[1]=" << fixed[1] << "\n";
    return 0;
}
