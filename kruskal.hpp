#ifndef KRUSKAL_MST_HPP
#define KRUSKAL_MST_HPP

#include "graph.hpp"
#include <vector>
#include <tuple>

class KruskalMST {
public:
    std::vector<std::tuple<int, int, int>> computeMST(Graph& graph);
};

#endif  // KRUSKAL_MST_HPP
