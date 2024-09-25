#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "graph.hpp"  // Include your Graph class
#include "mst_factory.hpp"

#define PORT 8081
#define BUFFER_SIZE 1024

Graph currentGraph(0);  // Global graph object

// Menu display
std::string menu =
    "Menu:\n"
    "1. Create a new graph with a specified number of vertices\n"
    "2. Add an edge to the graph (provide: from, to, weight)\n"
    "3. Remove an edge from the graph (provide: from, to)\n"
    "4. Get total weight of MST (Prim algorithm)\n"
    "5. Get total weight of MST (Kruskal algorithm)\n"
    "6. Get longest path in MST (provide: start, end)\n"
    "7. Get shortest path in MST (provide: start, end)\n"
    "8. Print MST (Prim algorithm)\n"
    "9. Print MST (Kruskal algorithm)\n"
    "10. Exit\n";

// Function to send a string message to the client
void sendMessage(int client_fd, const std::string& message) {
    send(client_fd, message.c_str(), message.size(), 0);
}

// Function to handle client requests based on the menu
void handleClientRequest(int client_fd) {
    char buffer[BUFFER_SIZE] = {0};

    // Send the menu to the client
    sendMessage(client_fd, menu);

    while (true) {
        // Clear buffer and read client input
        memset(buffer, 0, BUFFER_SIZE);
        read(client_fd, buffer, BUFFER_SIZE);
        int choice = std::stoi(buffer);
        std::cout << "Thread " << std::this_thread::get_id() << " is handling the request\n";
        switch (choice) {
            case 1:
                sendMessage(client_fd, "Enter the number of vertices:\n");
                read(client_fd, buffer, BUFFER_SIZE);
                currentGraph = Graph(std::stoi(buffer));
                sendMessage(client_fd, "Graph created successfully!\n");
                break;
            case 2: {
                sendMessage(client_fd, "Enter 'from', 'to', and 'weight' for the edge:\n");
                read(client_fd, buffer, BUFFER_SIZE);
                int from, to, weight;
                sscanf(buffer, "%d %d %d", &from, &to, &weight);
                currentGraph.addEdge(from, to, weight);
                sendMessage(client_fd, "Edge added successfully!\n");
                break;
            }
            case 3: {
                sendMessage(client_fd, "Enter 'from' and 'to' of the edge to remove:\n");
                read(client_fd, buffer, BUFFER_SIZE);
                int from, to;
                sscanf(buffer, "%d %d", &from, &to);
                currentGraph.removeEdge(from, to);
                sendMessage(client_fd, "Edge removed successfully!\n");
                break;
            }
            case 4: {
                int totalWeight = currentGraph.getTotalWeight();
                sendMessage(client_fd, "Total weight of MST (Prim): " + std::to_string(totalWeight) + "\n");
                break;
            }
            case 5: {
                int totalWeight = currentGraph.getTotalWeight();
                sendMessage(client_fd, "Total weight of MST (Kruskal): " + std::to_string(totalWeight) + "\n");
                break;
            }
            case 6: {
                sendMessage(client_fd, "Enter 'start' and 'end' vertices for the longest path:\n");
                read(client_fd, buffer, BUFFER_SIZE);
                int start, end;
                sscanf(buffer, "%d %d", &start, &end);
                int longestPath = currentGraph.getLongestPath(start, end);
                sendMessage(client_fd, "Longest path: " + std::to_string(longestPath) + "\n");
                break;
            }
            case 7: {
                sendMessage(client_fd, "Enter 'start' and 'end' vertices for the shortest path:\n");
                read(client_fd, buffer, BUFFER_SIZE);
                int start, end;
                sscanf(buffer, "%d %d", &start, &end);
                int shortestPath = currentGraph.getShortestPath(start, end);
                sendMessage(client_fd, "Shortest path: " + std::to_string(shortestPath) + "\n");
                break;
            }
            case 8:
                sendMessage(client_fd, "MST Edges (Prim):\n");
                currentGraph.printMST();
                break;
            case 9:
                sendMessage(client_fd, "MST Edges (Kruskal):\n");
                currentGraph.printMST();
                break;
            case 10:
                sendMessage(client_fd, "Goodbye!\n");
                close(client_fd);
                return;
            default:
                sendMessage(client_fd, "Invalid choice. Please try again.\n");
        }

        // Send the menu again for next operation
        sendMessage(client_fd, menu);
    }
}

// Leader-Follower Thread Pool
class LeaderFollowerPool {
    std::queue<int> clientQueue;
    std::mutex mtx;
    std::condition_variable cv;
    bool running = true;
    std::vector<std::thread> workers;

    void workerFunction() {
        while (running) {
            int client_fd;
            {
                std::unique_lock<std::mutex> lock(mtx);
                cv.wait(lock, [this] { return !clientQueue.empty() || !running; });
                if (!running && clientQueue.empty()) return;
                client_fd = clientQueue.front();
                clientQueue.pop();
            }

            // Handle client request
            handleClientRequest(client_fd);
        }
    }

public:
    LeaderFollowerPool(int numThreads) {
        for (int i = 0; i < numThreads; ++i) {
            workers.emplace_back(&LeaderFollowerPool::workerFunction, this);
        }
    }

    ~LeaderFollowerPool() {
        stop();
        for (auto& thread : workers) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

    void submitClient(int client_fd) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            clientQueue.push(client_fd);
        }
        cv.notify_one();
    }

    void stop() {
        {
            std::lock_guard<std::mutex> lock(mtx);
            running = false;
        }
        cv.notify_all();
    }
};

// Main function for the Leader-Follower server
int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Bind the socket to port 8080
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server started and listening on port " << PORT << std::endl;

    LeaderFollowerPool pool(3);  // Create a thread pool with 3 threads

    while (true) {
        new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        if (new_socket < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        std::cout << "New client connected!" << std::endl;
        pool.submitClient(new_socket);
    }

    return 0;
}

//make
// ./pipeline_server
// Server listening on port 8081
//open a new terminal
//nc localhost 8081