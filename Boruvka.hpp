#ifndef BORUVKA_HPP
#define BORUVKA_HPP

#include "graph.hpp"
#include <vector>
#include <tuple>

class BoruvkaMST {
public:
    std::vector<std::tuple<int, int, int>> computeMST(Graph& graph);

private:
    int findSet(std::vector<int>& parent, int i);
    void unionSets(std::vector<int>& parent, std::vector<int>& rank, int u, int v);
};

#endif  // BORUVKA_HPP
