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

void node_editor();

struct Storage {

    mini::Edge<double>& create_edge(double value);
    mini::Edge<double>& add(mini::Edge<double>& first, mini::Edge<double>& second);
    mini::Edge<double>& mul(mini::Edge<double>& first, mini::Edge<double>& second);

    // hardcode for now.
    std::vector<std::unique_ptr<mini::Edge<double>>> double_pool;
    std::vector<std::unique_ptr<mini::Node<Add>>> add_pool;
    std::vector<std::unique_ptr<mini::Node<Multiply>>> mul_pool;
};

