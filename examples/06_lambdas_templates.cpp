// 06_lambdas_templates.cpp  —  模板（泛型）与 lambda（匿名函数）
#include <algorithm>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

// ---------- 函数模板：写一遍，对所有类型生效（编译期生成具体版本）----------
template <typename T>
T maxv(T a, T b) { return a > b ? a : b; }

// 类模板：std::vector<T>、glm::mat<4,4,float> 都是这种
template <typename T>
struct Pair {
    T first, second;
    Pair(T a, T b) : first(a), second(b) {}
};

int main() {
    // 模板实参通常能推导出来
    std::cout << maxv(3, 7) << " "
              << maxv(2.5, 1.5) << " "
              << maxv<std::string>("apple", "banana") << "\n";

    Pair<int> pi{1, 2};
    Pair<std::string> ps{"x", "y"};
    std::cout << pi.first << "," << pi.second << " "
              << ps.first << "," << ps.second << "\n";

    // ---------- lambda：就地定义的小函数 ----------
    // 语法: [捕获](参数) -> 返回类型 { 函数体 }   返回类型可省略
    auto add = [](int a, int b) { return a + b; };
    std::cout << "add(2,3) = " << add(2, 3) << "\n";

    int base = 10;
    auto valueCap = [base](int v) { return v + base; };       // 值捕获（只读副本）
    auto refCap = [&base](int v) { return v + base; };        // 引用捕获
    base = 100;
    std::cout << "valueCap(1)=" << valueCap(1)                 // 11（捕获时是 10）
              << " refCap(1)=" << refCap(1) << "\n";           // 101（实时读）

    // lambda 最常见的用途：作为算法的参数
    std::vector<int> v{5, 2, 9, 1, 7};
    std::sort(v.begin(), v.end(), [](int a, int b) { return a > b; });  // 降序
    std::for_each(v.begin(), v.end(), [](int n) { std::cout << n << " "; });
    std::cout << "\n";

    // ---------- std::function：能保存任意可调用对象（回调/事件）----------
    std::function<float(float)> curve = [](float t) { return t * t; };
    std::cout << "curve(3) = " << curve(3) << "\n";

    // 实际用途：把回调存起来，稍后调用
    std::vector<std::function<void()>> commands;
    commands.push_back([&] { std::cout << "  命令 1: base=" << base << "\n"; });
    commands.push_back([]{ std::cout << "  命令 2: 无捕获\n"; });
    for (auto& cmd : commands) cmd();
    return 0;
}
