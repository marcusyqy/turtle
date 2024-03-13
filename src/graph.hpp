#pragma once

#include "minigraph/edge.hpp"
#include "minigraph/node.hpp"
#include <vector>
#include <memory>

struct Add {
    double operator()(double x, double y) const {
        return x + y;
    }
};

struct Multiply {
    double operator()(double x, double y) const {
        return x * y;
    }
};


struct Graph {

    void visualize();

    const mini::Edge<double>& create_edge(double value);
    const mini::Edge<double>& add(const mini::Edge<double>& first, const mini::Edge<double>& second);
    const mini::Edge<double>& mul(const mini::Edge<double>& first, const mini::Edge<double>& second);

    // hardcode for now.
    std::vector<std::unique_ptr<mini::Edge<double>>> double_pool;
    std::vector<std::unique_ptr<mini::Node<Add>>> add_pool;
    std::vector<std::unique_ptr<mini::Node<Multiply>>> mul_pool;
};

