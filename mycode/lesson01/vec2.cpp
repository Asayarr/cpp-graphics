#include "vec2.h"
#include <iostream>

Vec2 add(const Vec2 &a, const Vec2 &b) {
    return {a.x + b.x, a.y + b.y}; 
}

void print(const Vec2 &v) { 
    std::cout << "(" << v.x << ", " << v.y<< ")\n"; 
}
