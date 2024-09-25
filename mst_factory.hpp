#ifndef MST_FACTORY_HPP
#define MST_FACTORY_HPP

#include <string>
#include <vector>
#include <tuple>
#include "graph.hpp"
#include "Boruvka.hpp"
#include "prim.hpp"
#include "kruskal.hpp"

class MSTFactory {
public:
    static std::vector<std::tuple<int, int, int>> computeMST(Graph& graph, const std::string& algorithm);
};

#endif  // MST_FACTORY_HPP
