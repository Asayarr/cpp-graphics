#pragma once
// 头文件（.h）：放“声明”。#pragma once 保证同一编译单元只包含一次。
// 注意：头文件里不要 using namespace std;（会污染所有包含它的文件）

struct Vec3 {
    float x = 0.0f, y = 0.0f, z = 0.0f;

    Vec3() = default;
    Vec3(float x, float y, float z);       // 只有声明，实现在 vec3.cpp

    float dot(const Vec3& o) const;
    Vec3 operator+(const Vec3& o) const;
};

// 头文件里可以直接定义的小函数会被多个 .cpp 各编译一份。
// 加 inline 表示“允许重复定义，链接器合并成一份”。（模板和类内定义的函数自动 inline）
inline float lengthSq(const Vec3& v) { return v.dot(v); }
