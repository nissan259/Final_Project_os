#include "prim.hpp"
#include <iostream>
#include <queue>
#include <limits>

std::vector<std::tuple<int, int, int>> PrimMST::computeMST(Graph& graph) {
    int V = graph.V;
    std::vector<int> key(V, std::numeric_limits<int>::max());
    std::vector<int> parent(V, -1);
    std::vector<bool> inMST(V, false);
    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<>> pq;

    pq.push({0, 0});
    key[0] = 0;

    std::vector<std::tuple<int, int, int>> mstEdges;
    int totalWeight = 0;

    while (!pq.empty()) {
        int u = pq.top().second;
        pq.pop();

        if (inMST[u]) continue;
        inMST[u] = true;

        if (parent[u] != -1) {
            mstEdges.push_back({key[u], parent[u], u});
            totalWeight += key[u];

            // Print the selected edge
            std::cout << "Prim: Edge: " << parent[u] << " -- " << u << " (weight: " << key[u] << ")\n";
        }

        // Traverse all edges connected to u
        for (const auto& edge : graph.getEdges()) {
            int weight, v1, v2;
            std::tie(weight, v1, v2) = edge;

            int v = (u == v1) ? v2 : v1;

            if (!inMST[v] && weight < key[v]) {
                key[v] = weight;
                parent[v] = u;
                pq.push({key[v], v});
            }
        }
    }

    std::cout << "Prim's Total Weight: " << totalWeight << std::endl;
    return mstEdges;
}
