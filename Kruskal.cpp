#include "kruskal.hpp"
#include <iostream>
#include <algorithm>
#include <vector>

class DSU {
    std::vector<int> parent, rank;

public:
    DSU(int n) : parent(n, -1), rank(n, 1) {}

    int find(int i) {
        if (parent[i] == -1)
            return i;
        return parent[i] = find(parent[i]);
    }

    void unite(int x, int y) {
        int s1 = find(x);
        int s2 = find(y);

        if (s1 != s2) {
            if (rank[s1] < rank[s2]) {
                parent[s1] = s2;
            } else if (rank[s1] > rank[s2]) {
                parent[s2] = s1;
            } else {
                parent[s2] = s1;
                rank[s1] += 1;
            }
        }
    }
};

std::vector<std::tuple<int, int, int>> KruskalMST::computeMST(Graph& graph) {
    std::vector<std::tuple<int, int, int>> edges = graph.getEdges();
    std::sort(edges.begin(), edges.end());  // Sort edges by weight

    DSU dsu(graph.V);  // Initialize DSU for the number of vertices
    std::vector<std::tuple<int, int, int>> mstEdges;
    int totalWeight = 0;

    std::cout << "Kruskal's MST selected edges:\n";
    for (auto& edge : edges) {
        int w, u, v;
        std::tie(w, u, v) = edge;

        // Check if including this edge forms a cycle
        if (dsu.find(u) != dsu.find(v)) {
            dsu.unite(u, v);  // Union the sets
            mstEdges.push_back(edge);  // Add the edge to MST
            totalWeight += w;

            // Print the selected edge
            std::cout << "Kruskal: Edge: " << u << " -- " << v << " (weight: " << w << ")\n";
        }
    }

    std::cout << "Kruskal's Total Weight: " << totalWeight << std::endl;
    return mstEdges;
};
