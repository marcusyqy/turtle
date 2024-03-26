#include "minigraph/edge.hpp"
#include "minigraph/mem.hpp"
#include "minigraph/node.hpp"
#include <iostream>

struct Resolver {
  void resolve() {
    while (head) {
      head->delegate();
      head = head->next;
    }
    clear();
  }

  void clear() {
    head = nullptr;
    tail = nullptr;
    arena.clear();
  }

  template <typename T>
  void attach(mini::Node<T>& node) {
    node.on_outdated({ mini::connect<&Resolver::callback<T>>, *this });
  }

private:
  template <typename T>
  void callback(mini::Node<T>& node) {
    auto ptr = arena.push(sizeof(Node), alignof(Node));
    auto n   = new (ptr) Node{ node, nullptr };
    if (tail) {
      tail->next = n;
      tail       = tail->next;
    } else {
      head = n;
      tail = n;
    }
    std::cout << "Node depth is: " << std::dec << node.depth() << std::endl;
  }

private:
  mini::Default_Stack_Allocator arena;
  struct Node {
    mini::Delegate<void()> delegate;
    Node* next;
  } *head = nullptr, *tail = nullptr;
};

struct Graph {

  template <typename T>
  mini::Edge<T>& edge(T&& t) {
    std::cout << "allocating edge:" << std::dec << sizeof(mini::Edge<T>) << "align " << alignof(mini::Edge<T>)
              << std::endl;
    auto ptr = arena.push(sizeof(mini::Edge<T>), alignof(mini::Edge<T>));
    auto& e  = *::new (ptr) mini::Edge<T>{ t };
    add_to_destruct(e);
    return e;
  }

  template <typename T>
  mini::Edge<T>& edge(const T& t) {
    std::cout << "allocating edge:" << std::dec << sizeof(mini::Edge<T>) << "align " << alignof(mini::Edge<T>)
              << std::endl;
    auto ptr = arena.push(sizeof(mini::Edge<T>), alignof(mini::Edge<T>));
    auto& e  = *::new (ptr) mini::Edge<T>{ t };
    add_to_destruct(e);
    return e;
  }

  template <typename T, typename... Args>
  typename mini::Node<T>::Outputs& node(typename mini::Node<T>::Inputs inputs, Args&&... args) {
    std::cout << "allocating node:" << std::dec << sizeof(mini::Node<T>) << "align " << alignof(mini::Node<T>)
              << std::endl;
    auto ptr = arena.push(sizeof(mini::Node<T>), alignof(mini::Node<T>));
    // memleak
    auto node = ::new (ptr) mini::Node<T>{ inputs, std::forward<Args&&>(args)... };
    resolver.attach(*node);
    add_to_destruct(*node);
    return node->edges();
  }

  void run() { resolver.resolve(); }

  void clear() {
    resolver.clear();
    while (head) {
      head->destruct(head->ptr);
      head = head->next;
    }
    arena.clear();
  }
  ~Graph() { clear(); }

private:
  template <typename T>
  static void destruct(void* p) {
    std::destroy_at((T*)p);
  }

  template <typename T>
  void add_to_destruct(T& p) {
    auto ptr = (Destructor_Node*)arena.push(sizeof(Destructor_Node), alignof(Destructor_Node));
    ptr->ptr = &p;
    ptr->destruct.connect<&Graph::destruct<T>>();
    ptr->next = head;
    head      = ptr;
  }

private:
  struct Destructor_Node {
    void* ptr;
    mini::Delegate<void(void*)> destruct;
    Destructor_Node* next = nullptr;
  }* head = nullptr;

  mini::Default_Stack_Allocator arena;
  Resolver resolver;
};

struct Add {
  double operator()(double x, double y) const {
    std::cout << "Add is called" << std::endl;
    return x + y;
  }
};

struct Multiply {
  double operator()(double x, double y) const {
    std::cout << "Multiply is called" << std::endl;
    return x * y;
  }
};

struct Result {
  double x;
  Result(double y) { x = y; }
  Result& operator=(const Result& o) = delete;
  // Result(const Result& o) = delete;
};

struct Upgrade {
  std::tuple<Result, Result> operator()(double x, double y) const { return std::make_tuple(Result{ x }, Result{ y }); }
};

struct Switcher {
  const Result& operator()(bool toggle, const Result& x, const Result& y) const {
    if (toggle) return x;
    return y;
  }
};

int main() {
  std::cout << "Size of delegate:" << sizeof(std::vector<mini::Delegate<void()>>) << std::endl;
  Graph graph{};
  // 1
  auto x1 = graph.edge(5.0);
  auto y1 = graph.edge(7.0);
  auto z1 = graph.edge(8.0);

  auto& [add1_result] = graph.node<Add>({ x1, y1 });
  auto& [mul1_result] = graph.node<Multiply>({ z1, add1_result });

  // 2
  auto x2 = graph.edge(8.0);
  auto y2 = graph.edge(4.0);
  auto z2 = graph.edge(18.0);

  auto& [add2_result] = graph.node<Add>({ x2, y2 });

  auto& [mul2_result] = graph.node<Multiply>({ z2, add2_result });

  auto& [upgrade1, upgrade2] = graph.node<Upgrade>({ mul1_result, mul2_result });

  auto condition = graph.edge(false);
  auto& [result] = graph.node<Switcher>({ condition, upgrade1, upgrade2 });

  graph.run();
  std::cout << "=============================================================" << std::endl;
  std::cout << "begin(1)" << std::endl;
  std::cout << "x : " << x1.get() << std::endl;
  std::cout << "y : " << y1.get() << std::endl;
  std::cout << "z : " << z1.get() << std::endl;
  std::cout << "x+y : " << add1_result.get() << std::endl;
  std::cout << "(x+y)*z : " << mul1_result.get() << std::endl;
  std::cout << "result : " << upgrade1.get().x << std::endl;
  std::cout << "=============================================================" << std::endl;
  std::cout << "begin(2)" << std::endl;
  std::cout << "x : " << x2.get() << std::endl;
  std::cout << "y : " << y2.get() << std::endl;
  std::cout << "z : " << z2.get() << std::endl;
  std::cout << "x+y : " << add2_result.get() << std::endl;
  std::cout << "(x+y)*z : " << mul2_result.get() << std::endl;
  std::cout << "result : " << upgrade2.get().x << std::endl;
  std::cout << std::endl;

  auto print = [&] {
    graph.run();
    std::cout << "=============================================================" << std::endl;
    std::cout << "switcher" << std::endl;
    std::cout << "switcher result: " << result.get().x << std::endl;
    std::cout << "switcher condition: " << std::boolalpha << condition.get() << std::endl;
    std::cout << "=============================================================" << std::endl;
    std::cout << std::endl;
  };

  x1 = 0;
  print();

  condition = true;
  print();
  condition = false;
  print();
  condition = true;
  print();
}
