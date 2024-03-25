#if 0 
#include "minigraph/graph.hpp"
#include <utility>

int main() {
    mini::Graph graph{};
    auto e = graph.edge(4);

    int x = 5;
    auto xx = graph.edge<int>(x);
    auto xxx = graph.edge(std::move(x));
    auto xx2 = graph.edge(x);


    auto [e1, e2, e3] = graph.node();
}
#endif


int main() {

}