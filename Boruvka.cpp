#include "Boruvka.hpp"
#include <algorithm>
#include <limits>

std::vector<std::tuple<int, int, int>> BoruvkaMST::computeMST(Graph& graph) {
    // Implementation of Borůvka's MST algorithm
    int V = graph.V;
    std::vector<int> parent(V), rank(V, 0);
    std::vector<int> cheapest(V, -1);
    std::vector<std::tuple<int, int, int>> mstEdges;
    
    // Initially each vertex is its own component
    for (int i = 0; i < V; ++i) parent[i] = i;

    int numComponents = V;
    
    while (numComponents > 1) {
        std::fill(cheapest.begin(), cheapest.end(), -1);
        
        for (auto& edge : graph.getEdges()) {
            int weight, u, v;
            std::tie(weight, u, v) = edge;

            int setU = findSet(parent, u);
            int setV = findSet(parent, v);

            if (setU != setV) {
                if (cheapest[setU] == -1 || std::get<0>(graph.getEdges()[cheapest[setU]]) > weight)
                    cheapest[setU] = &edge - &graph.getEdges()[0];

                if (cheapest[setV] == -1 || std::get<0>(graph.getEdges()[cheapest[setV]]) > weight)
                    cheapest[setV] = &edge - &graph.getEdges()[0];
            }
        }

        for (int i = 0; i < V; ++i) {
            if (cheapest[i] != -1) {
                int weight, u, v;
                std::tie(weight, u, v) = graph.getEdges()[cheapest[i]];

                int setU = findSet(parent, u);
                int setV = findSet(parent, v);

                if (setU != setV) {
                    mstEdges.push_back(std::make_tuple(weight, u, v));
                    unionSets(parent, rank, setU, setV);
                    numComponents--;
                }
            }
        }
    }
    return mstEdges;
}

int BoruvkaMST::findSet(std::vector<int>& parent, int i) {
    if (parent[i] == i) return i;
    return parent[i] = findSet(parent, parent[i]);
}

void BoruvkaMST::unionSets(std::vector<int>& parent, std::vector<int>& rank, int u, int v) {
    int setU = findSet(parent, u);
    int setV = findSet(parent, v);
    if (rank[setU] > rank[setV]) {
        parent[setV] = setU;
    } else if (rank[setU] < rank[setV]) {
        parent[setU] = setV;
    } else {
        parent[setV] = setU;
        rank[setU]++;
    }
}
