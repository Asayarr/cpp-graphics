# mycode —— 你自己的 C++ 练习

`examples/` 里是教程示例（当参考，别动）；**这个目录用来练 C++ 基础语法**。

（tinyrenderer 相关的实现代码放在 `../tinyrenderer/mine/`。）

## 怎么开始

新建一个 `.cpp`，比如 `mycode/test.cpp`：

```cpp
#include <iostream>

int main() {
    std::cout << "hello from mycode\n";
}
```

然后在 VS Code 里打开它、按 **F5** —— 会自动编译并用 gdb 调试，不用改任何配置文件。

## 命令行

```bash
cd ~/cpp-graphics
make mycode/test            # 编译单个
./mycode/test               # 运行
make                        # 编译全部（examples + mycode）
```

## 多文件项目

如果要把代码拆成 `.h` + `.cpp`，在这个目录下开子文件夹，参考 `examples/multifile/` 的写法，
手动编译：

```bash
g++ -std=c++20 -Wall -Wextra mycode/项目名/*.cpp -o mycode/项目名/app
```
