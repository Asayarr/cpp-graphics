#include "vec2.h"

int main(){
    Vec2 a{1, 2}, b{3, 4};
    print(add(a, b));

    Vec2 c = a;
    c.x = 100;
    print(a);

    Vec2& d = a;
    d.x = 100;
    print(a);
}