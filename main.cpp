#include <iostream>
#include "graph.hpp"
#include "prim.hpp"
#include "kruskal.hpp"
#include "mst_factory.hpp"

// Function to print the edges of the MST
void printMSTEdges(const std::vector<std::tuple<int, int, int>>& mstEdges) {
    std::cout << "MST Edges:\n";
    for (const auto& edge : mstEdges) {
        int weight, u, v;
        std::tie(weight, u, v) = edge;
        std::cout << u << " -- " << v << " (weight: " << weight << ")\n";
    }
    std::cout << "----------------------\n";
}

// Function to print additional MST statistics
void printMSTResults(Graph& graph) {
    std::cout << "Total weight of MST: " << graph.getTotalWeight() << "\n";
    std::cout << "Longest distance in MST: " << graph.getLongestDistance() << "\n";
    std::cout << "Average distance between edges: " << graph.getAverageDistance() << "\n";
    std::cout << "Shortest distance in MST: " << graph.getShortestDistance() << "\n";
    std::cout << "----------------------\n";
}

void runMSTAlgorithms(Graph& graph, const std::vector<std::string>& algorithms) {
    for (const std::string& algorithm : algorithms) {
        std::cout << "Running " << algorithm << " algorithm...\n";
        
        // Use MSTFactory to compute MST
        auto mstEdges = MSTFactory::computeMST(graph, algorithm);

        // Print the edges for the current MST
        printMSTEdges(mstEdges);

        // Clear MST edges in the graph to avoid adding up from previous runs
        graph.mstEdges.clear();
        
        // Add the computed MST edges to the graph for further calculations
        for (const auto& edge : mstEdges) {
            int weight, u, v;
            std::tie(weight, u, v) = edge;
            graph.addMSTEdge(u, v, weight);
        }

        // Print the results for the selected algorithm
        printMSTResults(graph);
    }
}

int main() {
    // Example 1: Basic Test with 5 Vertices
    Graph graph1(5);
    graph1.addEdge(0, 1, 10);
    graph1.addEdge(0, 2, 6);
    graph1.addEdge(0, 3, 5);
    graph1.addEdge(1, 3, 15);
    graph1.addEdge(2, 3, 4);

    std::vector<std::string> algorithms = {"prim", "kruskal"};
    std::cout << "Example 1: Basic Test\n";
    runMSTAlgorithms(graph1, algorithms);

    // Example 2: More Complex Test with 6 Vertices
    Graph graph2(6);
    graph2.addEdge(0, 1, 3);
    graph2.addEdge(0, 2, 1);
    graph2.addEdge(1, 2, 3);
    graph2.addEdge(1, 3, 1);
    graph2.addEdge(2, 4, 6);
    graph2.addEdge(3, 4, 5);
    graph2.addEdge(3, 5, 2);
    graph2.addEdge(4, 5, 4);

    std::cout << "\nExample 2: More Complex Test\n";
    runMSTAlgorithms(graph2, algorithms);

    // Example 3: Large Test Case with 4 Vertices
    Graph graph3(4);
    graph3.addEdge(0, 1, 10);
    graph3.addEdge(0, 2, 8);
    graph3.addEdge(0, 3, 5);
    graph3.addEdge(1, 2, 12);
    graph3.addEdge(2, 3, 7);

    std::cout << "\nExample 3: Larger Test Case\n";
    runMSTAlgorithms(graph3, algorithms);

    return 0;
}
