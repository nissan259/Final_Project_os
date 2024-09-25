#include "mst_factory.hpp"
using namespace std;
std::vector<std::tuple<int, int, int>> MSTFactory::computeMST(Graph& graph, const std::string& algorithm) {
    // if (algorithm == "boruvka") {
    //     BoruvkaMST boruvka;
    //     return boruvka.computeMST(graph);
      if (algorithm == "prim") {
        PrimMST prim;
        return prim.computeMST(graph);
    } else if (algorithm == "kruskal") {
        KruskalMST kruskal;
        return kruskal.computeMST(graph);
    }
    return {};
}
