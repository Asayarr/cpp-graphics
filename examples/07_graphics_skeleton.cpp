// 07_graphics_skeleton.cpp  —  一个图形学程序的典型骨架（伪代码，不能直接跑）
// 目的是让你把前面学的语法和真实图形学代码对上号。
#include <iostream>
#include <memory>
#include <string>
#include <vector>

// ---------------- 1) 数学：真实项目用 glm ----------------
// #include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
// using glm::vec3; using glm::mat4;
struct vec3 { float x, y, z; };

// ---------------- 2) 用 RAII 封装 GPU 资源（真实项目里是 shader/VBO/VAO）---
class Shader {
public:
    Shader(const std::string& /*vertexSrc*/, const std::string& /*fragmentSrc*/) {
        // glCreateProgram(); glShaderSource(...); glCompileShader(...); glLinkProgram(...);
    }
    ~Shader() {
        // glDeleteProgram(id_);   // 析构自动清理，不写就泄漏
    }
    void use() const { /* glUseProgram(id_); */ }
private:
    unsigned int id_ = 0;
};

// ---------------- 3) 用虚函数/多态表达场景节点 ----------------
class Object {
public:
    virtual ~Object() = default;
    virtual void update(float dt) = 0;
    virtual void draw(const Shader& shader) const = 0;
};

class Triangle : public Object {
public:
    Triangle() = default;

    void update(float dt) override {
        angle_ += rotSpeed_ * dt;            // 用数学更新状态
    }
    void draw(const Shader& shader) const override {
        shader.use();
        // glBindVertexArray(vao_);
        // glUniformMatrix4fv(..., /*MVP*/, ...);
        // glDrawArrays(GL_TRIANGLES, 0, 3);
        (void)angle_;
    }

private:
    unsigned int vao_ = 0;
    float angle_ = 0.0f;
    float rotSpeed_ = 1.0f;
};

int main() {
    // ---------------- 4) 初始化：窗口 + 上下文 ----------------
    // GLFWwindow* window = glfwCreateWindow(800, 600, "My Renderer", nullptr, nullptr);
    // glfwMakeContextCurrent(window);
    // gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    // ---------------- 5) 资源：智能指针管理生命周期 ----------------
    auto shader = std::make_unique<Shader>("vertex.glsl", "fragment.glsl");
    std::vector<std::unique_ptr<Object>> scene;
    scene.push_back(std::make_unique<Triangle>());

    // ---------------- 6) 主循环：渲染程序的心跳 ----------------
    const float dt = 1.0f / 60.0f;
    for (int frame = 0; frame < 3; ++frame) {  // 真实代码是 while (!glfwWindowShouldClose(window))
        // glfwPollEvents();                    // 处理输入事件
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (auto& obj : scene) obj->update(dt);           // 更新
        for (const auto& obj : scene) obj->draw(*shader);  // 绘制

        // glfwSwapBuffers(window);             // 双缓冲交换
        std::cout << "frame " << frame << " 已渲染\n";
    }
    // ---------------- 7) 退出：智能指针和析构自动清理 GPU 资源 ----------------
    std::cout << "退出，资源自动释放\n";
    return 0;
}
