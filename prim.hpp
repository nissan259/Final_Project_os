#ifndef PRIM_MST_HPP
#define PRIM_MST_HPP

#include "graph.hpp"
#include <vector>
#include <tuple>

class PrimMST {
public:
    std::vector<std::tuple<int, int, int>> computeMST(Graph& graph);
};

#endif  // PRIM_MST_HPP
