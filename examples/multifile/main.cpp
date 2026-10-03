// 多文件编译示范
// 一步到位：  g++ -std=c++20 vec3.cpp main.cpp -o app
// 分开编译：  g++ -std=c++20 -c vec3.cpp -o vec3.o
//             g++ -std=c++20 -c main.cpp -o main.o
//             g++ vec3.o main.o -o app
// 关键点：只 #include 头文件，不要把 .cpp 互相 include！
#include "vec3.h"

#include <iostream>

int main() {
    Vec3 a{1, 2, 3};
    Vec3 b{4, 5, 6};
    Vec3 c = a + b;                      // 用的是 vec3.cpp 里的实现
    std::cout << "a+b = (" << c.x << ", " << c.y << ", " << c.z << ")\n";
    std::cout << "(a+b)·b = " << c.dot(b) << "\n";
    std::cout << "|a|^2 = " << lengthSq(a) << "\n";
    return 0;
}
