#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <vector>
#include <tuple>

class Graph {
public:
    int V;
    std::vector<std::tuple<int, int, int>> mstEdges;  // To store MST edges
    std::vector<std::vector<int>> adjMatrix;  // Adjacency matrix

    Graph(int V);

    void addEdge(int u, int v, int weight);
    void removeEdge(int u, int v);
    std::vector<std::tuple<int, int, int>> getEdges();
    void addMSTEdge(int u, int v, int weight);

    // Statistics functions
    int getTotalWeight();
    int getLongestDistance();
    double getAverageDistance();
    int getShortestDistance();

    // Path-based functions
    int getShortestPath(int start, int end);
    int getLongestPath(int start, int end);

    void printMST();
};

#endif  // GRAPH_HPP
