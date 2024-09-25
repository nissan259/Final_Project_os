#include "graph.hpp"
#include <algorithm>
#include <numeric>
#include <limits>
#include <iostream>
#include <queue>
#include <functional>


// Constructor
Graph::Graph(int V) : V(V), adjMatrix(V, std::vector<int>(V, -1)) {}

// Add an edge to the adjacency matrix and mstEdges
void Graph::addEdge(int u, int v, int weight) {
    mstEdges.push_back({weight, u, v});
    adjMatrix[u][v] = weight;
    adjMatrix[v][u] = weight;  // Assuming an undirected graph
}

// Remove an edge from the adjacency matrix and mstEdges
void Graph::removeEdge(int u, int v) {
    adjMatrix[u][v] = -1;
    adjMatrix[v][u] = -1;  // Assuming an undirected graph

    // Remove from mstEdges if present
    mstEdges.erase(std::remove_if(mstEdges.begin(), mstEdges.end(), 
                  [u, v](const std::tuple<int, int, int>& edge) {
                      return (std::get<1>(edge) == u && std::get<2>(edge) == v) || 
                             (std::get<1>(edge) == v && std::get<2>(edge) == u);
                  }), mstEdges.end());
}

std::vector<std::tuple<int, int, int>> Graph::getEdges() {
    return mstEdges;
}

void Graph::addMSTEdge(int u, int v, int weight) {
    mstEdges.push_back({weight, u, v});
    adjMatrix[u][v] = weight;
    adjMatrix[v][u] = weight;  // Assuming undirected
}

// Get total weight of MST
int Graph::getTotalWeight() {
    int totalWeight = 0;
    for (const auto& edge : mstEdges) {
        totalWeight += std::get<0>(edge);
    }
    return totalWeight;
}

// Get the longest distance in the MST
int Graph::getLongestDistance() {
    int longest = 0;
    for (const auto& edge : mstEdges) {
        longest = std::max(longest, std::get<0>(edge));
    }
    return longest;
}

// Get the average distance between edges in the MST
double Graph::getAverageDistance() {
    if (mstEdges.empty()) return 0;
    int totalDistance = 0;
    for (const auto& edge : mstEdges) {
        totalDistance += std::get<0>(edge);
    }
    return static_cast<double>(totalDistance) / mstEdges.size();
}

// Get the shortest distance in the MST
int Graph::getShortestDistance() {
    int shortest = std::numeric_limits<int>::max();
    for (const auto& edge : mstEdges) {
        shortest = std::min(shortest, std::get<0>(edge));
    }
    return shortest == std::numeric_limits<int>::max() ? 0 : shortest;
}

// Dijkstra's algorithm to find the shortest path between two vertices
int Graph::getShortestPath(int start, int end) {
    std::vector<int> dist(V, std::numeric_limits<int>::max());
    dist[start] = 0;

    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<>> pq;
    pq.push({0, start});

    while (!pq.empty()) {
        int u = pq.top().second;
        pq.pop();

        for (int v = 0; v < V; ++v) {
            if (adjMatrix[u][v] != -1) {  // There's an edge
                int weight = adjMatrix[u][v];
                if (dist[u] + weight < dist[v]) {
                    dist[v] = dist[u] + weight;
                    pq.push({dist[v], v});
                }
            }
        }
    }
    return dist[end];
}

// Use a simple DFS for finding the longest path in the graph
int Graph::getLongestPath(int start, int end) {
    std::vector<bool> visited(V, false);
    int longestPath = 0;

    // Lambda function to perform DFS and track the longest path
    std::function<void(int, int)> dfs = [&](int u, int distance) {
        visited[u] = true;  // Mark node as visited

        // If we reach the target node, update the longest path
        if (u == end) {
            longestPath = std::max(longestPath, distance);
        } else {
            // Explore all adjacent vertices
            for (int v = 0; v < V; ++v) {
                if (!visited[v] && adjMatrix[u][v] != -1) {  // If edge exists and vertex is not visited
                    dfs(v, distance + adjMatrix[u][v]);  // Recursive DFS call
                }
            }
        }

        visited[u] = false;  // Backtrack: mark the node as unvisited for other paths
    };

    // Start DFS from the 'start' node
    dfs(start, 0);
    
    return longestPath;  // Return the longest path found
}



// Print the MST edges
void Graph::printMST() {
    for (const auto& edge : mstEdges) {
        std::cout << std::get<1>(edge) << " -- " << std::get<2>(edge) << " [weight=" << std::get<0>(edge) << "]" << std::endl;
    }
}
