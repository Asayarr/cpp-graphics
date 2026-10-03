// 05_polymorphism.cpp  —  继承 / 虚函数 / 智能指针（现代 C++ 资源管理）
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

// ---------- 智能指针：现代 C++ 几乎不写裸 new/delete ----------
struct Mesh {
    std::string name;
    explicit Mesh(std::string n) : name(std::move(n)) {
        std::cout << "  + 创建 Mesh " << name << "\n";
    }
    ~Mesh() { std::cout << "  - 销毁 Mesh " << name << "\n"; }
};

// ---------- 多态：基类 + virtual，图形学的场景图/节点就靠它 ----------
class Node {
public:
    explicit Node(std::string n) : name_(std::move(n)) {}
    virtual ~Node() = default;                 // 基类析构一定要 virtual！
    virtual void update(float dt) = 0;         // 纯虚函数 → 抽象类
    virtual void draw() const {                // 有默认实现，子类可覆写
        std::cout << "  [Node] " << name_ << "\n";
    }
    const std::string& name() const { return name_; }
protected:
    std::string name_;
};

class Transform : public Node {
public:
    Transform(std::string n, float x) : Node(std::move(n)), x_(x) {}
    void update(float dt) override { x_ += dt; }        // override 让编译器帮你检查
    void draw() const override {
        std::cout << "  [Transform] " << name_ << " x=" << x_ << "\n";
    }
private:
    float x_ = 0.0f;
};

int main() {
    std::cout << "== unique_ptr：独占所有权，离开作用域自动 delete ==\n";
    {
        auto m = std::make_unique<Mesh>("cube");
        std::cout << "  名字: " << m->name << "\n";
        auto m2 = std::move(m);        // 所有权转移，m 变成空指针
        std::cout << "  转移后 m 是" << (m ? "非空" : "空") << "\n";
        m2->name = "cube2";
        std::cout << "  m2->name = " << m2->name << "\n";
    }  // 自动析构

    std::cout << "== shared_ptr：引用计数，最后一个销毁时释放 ==\n";
    auto a = std::make_shared<Mesh>("sphere");
    {
        auto b = a;                    // 计数 +1
        std::cout << "  作用域内计数 = " << a.use_count() << "\n";
    }
    std::cout << "  作用域外计数 = " << a.use_count() << "\n";

    std::cout << "== 多态：基类指针/引用统一处理不同子类 ==\n";
    // 注意：Node 有纯虚函数，是抽象类，不能直接 new Node(...)；
    //       只能通过子类创建，再用基类指针（unique_ptr<Node>）持有。
    std::vector<std::unique_ptr<Node>> scene;
    scene.push_back(std::make_unique<Transform>("player", 1.0f));
    scene.push_back(std::make_unique<Transform>("enemy", 5.0f));

    for (const auto& node : scene) node->update(1.0f);
    for (const auto& node : scene) node->draw();   // 动态派发到正确的子类
    return 0;
}
