// 源文件（.cpp）：放“实现”。开头包含自己的头文件。
#include "vec3.h"

Vec3::Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

float Vec3::dot(const Vec3& o) const {
    return x * o.x + y * o.y + z * o.z;
}

Vec3 Vec3::operator+(const Vec3& o) const {
    return {x + o.x, y + o.y, z + o.z};
}
