#pragma once

#include "minigraph/edge.hpp"
#include "minigraph/node.hpp"
#include "minigraph/mem.hpp"

#include <memory>
#include <vector>
#include <iostream>

struct Add {
  double operator()(double x, double y) const { return x + y; }
};

struct Multiply {
  double operator()(double x, double y) const { return x * y; }
};

void node_editor();


struct Executor {
  void resolve() {
    std::cout << "Run" << std::endl;
    while (head) {
      head->delegate();
      head = head->next;
    }
    std::cout << "Run End" << std::endl;
    clear();
  }

  void clear() {
    head = nullptr;
    tail = nullptr;
    arena.clear();
  }

  template <typename T>
  void attach(mini::Node<T>& node) {
    node.on_outdated({ mini::connect<&Executor::callback<T>>, *this });
  }

private:
  template <typename T>
  void callback(mini::Node<T>& node) {
    auto n = arena.push<Node>(node, nullptr);
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
    std::cout << "allocating edge:" << std::dec << sizeof(mini::Edge<T>) << ", align " << alignof(mini::Edge<T>)
              << std::endl;
    auto e = arena.push<mini::Edge<T>>(t);
    return *e;
  }

  template <typename T>
  mini::Edge<T>& edge(const T& t) {
    std::cout << "allocating edge:" << std::dec << sizeof(mini::Edge<T>) << ", align " << alignof(mini::Edge<T>)
              << std::endl;
    auto e = arena.push<mini::Edge<T>>(t);
    return *e;
  }

  template <typename T, typename... Args>
  typename mini::Node<T>::Ref_Outputs node(typename mini::Node<T>::Inputs inputs, Args&&... args) {
    std::cout << "allocating node:" << std::dec << sizeof(mini::Node<T>) << ", align " << alignof(mini::Node<T>)
              << std::endl;
    auto node = arena.push<mini::Node<T>>( inputs, std::forward<Args&&>(args)... );
    exec.attach(*node);
    return node->edges();
  }

  void run() { exec.resolve(); }

  void clear() {
    exec.clear();
    arena.clear();
  }

  ~Graph() { clear(); }

private:
  mini::Default_Stack_Allocator arena;
  Executor exec;
};
