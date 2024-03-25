#pragma once
#include "edge.hpp"
#include "node.hpp"
#include "mem.hpp"


namespace mini {

struct Graph {
public:
    template<typename T>
    Edge<T>& edge(T&& t) { // here we need to disallow references?
        return {};
    }

    template<typename T>
    Edge<T>& edge(const T& t) { // here we need to disallow references?
        return {};
    }

    template<typename T>
    typename Node<T>::outputs node() {

    }

private:
    Stack_Allocator<1 << 
};

}