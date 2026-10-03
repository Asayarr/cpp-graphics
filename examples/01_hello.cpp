// 01_hello.cpp  —  编译模型 / 标准输入输出 / namespace
/*
 * 编译并运行：
 *   g++ -std=c++20 01_hello.cpp -o 01_hello && ./01_hello
 * 或者用本目录的 Makefile：
 *   make 01_hello && ./01_hello
 */
#include <iostream>   // 注意：标准库头文件没有 .h 后缀
#include <string>

int main() {          // main 必须返回 int；不写 return 0 时默认返回 0
    // std::cout 是标准输出流，<< 会自动推导类型，不用 %d/%f 格式符
    std::cout << "Hello, C++!\n";

    std::string name;
    std::cout << "请输入名字: ";
    std::cin >> name;                 // >> 从标准输入读一个“单词”
    std::cout << "你好, " << name << "!\n";

    int x = 42;
    double pi = 3.14159;
    std::cout << x << " / " << pi << " / hex="
              << std::hex << x << std::dec << "\n";   // 还能控制格式

    // 用 using 省掉 std:: 前缀（但不要写在头文件里，会污染别人的命名空间）
    using std::cout;
    cout << "using 之后可以省略 std::\n";
    return 0;
}
