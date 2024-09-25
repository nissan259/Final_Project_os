#include <iostream>
#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <string>
#include <functional>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>  // For memset
#include "graph.hpp"
#include "mst_factory.hpp"

// Active Object class to manage async tasks
class ActiveObject {
    std::queue<std::function<void()>> tasks;
    std::mutex mtx;
    std::condition_variable cv;
    bool done;
    std::thread worker;

    void run() {
        while (true) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(mtx);
                cv.wait(lock, [this] { return !tasks.empty() || done; });
                if (done && tasks.empty()) return;
                task = std::move(tasks.front());
                tasks.pop();
            }
            task();
        }
    }

public:
    ActiveObject() : done(false), worker(&ActiveObject::run, this) {}
    ~ActiveObject() {
        {
            std::lock_guard<std::mutex> lock(mtx);
            done = true;
        }
        cv.notify_all();
        worker.join();
    }

    // Enqueue tasks for async execution
    void submit(std::function<void()> task) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            tasks.push(std::move(task));
        }
        cv.notify_one();
    }
};

// Menu display
std::string menu =
    "Menu:\n"
    "1. Create a new graph (provide number of vertices)\n"
    "2. Add an edge (provide: from, to, weight)\n"
    "3. Remove an edge (provide: from, to)\n"
    "4. Get total weight of MST\n"
    "5. Get longest path in MST (provide: start, end)\n"
    "6. Get shortest path in MST (provide: start, end)\n"
    "7. Get average distance between edges in MST\n"
    "8. Print MST\n"
    "9. Exit\n";

// Send a message to the client
void sendMessage(int client_fd, const std::string& message) {
    send(client_fd, message.c_str(), message.size(), 0);
}

// Server pipeline class
class PipelineServer {
    ActiveObject stage1; // Receive and parse graph data
    ActiveObject stage2; // Apply graph changes (optional)
    ActiveObject stage3; // Compute MST
    Graph currentGraph{0}; // Graph object to store the current graph

public:
    PipelineServer() {}

    void handleRequest(int new_socket) {
        // Send the menu to the client
        sendMessage(new_socket, menu);

        // Start Stage 1
        stage1.submit([new_socket, this]() {
            std::cout << "Processing Stage 1: Receive Client Request for client " << new_socket << std::endl;

            char buffer[1024] = {0}; // Mutable buffer for receiving input

            while (true) {
                // Clear buffer and read client input
                memset(buffer, 0, sizeof(buffer));
                read(new_socket, buffer, sizeof(buffer)); // Ensure buffer is mutable
                int choice = std::stoi(buffer);

                // Start Stage 2 (Graph Operations)
                stage2.submit([new_socket, this, choice, buffer]() mutable {
                    std::cout << "Processing Stage 2: Graph Operation for client " << new_socket << std::endl;

                    switch (choice) {
                        case 1:
                            sendMessage(new_socket, "Enter the number of vertices:\n");
                            read(new_socket, buffer, sizeof(buffer)); // Mutable buffer
                            currentGraph = Graph(std::stoi(buffer));
                            sendMessage(new_socket, "Graph created successfully!\n");
                            break;
                        case 2: {
                            sendMessage(new_socket, "Enter 'from', 'to', and 'weight' for the edge:\n");
                            read(new_socket, buffer, sizeof(buffer)); // Mutable buffer
                            int from, to, weight;
                            sscanf(buffer, "%d %d %d", &from, &to, &weight);
                            currentGraph.addEdge(from, to, weight);
                            sendMessage(new_socket, "Edge added successfully!\n");
                            break;
                        }
                        case 3: {
                            sendMessage(new_socket, "Enter 'from' and 'to' of the edge to remove:\n");
                            read(new_socket, buffer, sizeof(buffer)); // Mutable buffer
                            int from, to;
                            sscanf(buffer, "%d %d", &from, &to);
                            currentGraph.removeEdge(from, to);
                            sendMessage(new_socket, "Edge removed successfully!\n");
                            break;
                        }
                        default:
                            sendMessage(new_socket, "Invalid choice. Please try again.\n");
                    }

                    // Start Stage 3 (Calculations like MST)
                    stage3.submit([new_socket, this, choice]() {
                        std::cout << "Processing Stage 3: Calculation (MST, Pathfinding, etc.) for client " << new_socket << std::endl;

                        switch (choice) {
                            case 4: {
                                int totalWeight = currentGraph.getTotalWeight();
                                sendMessage(new_socket, "Total weight of MST: " + std::to_string(totalWeight) + "\n");
                                break;
                            }
                            case 5: {
                                sendMessage(new_socket, "Enter 'start' and 'end' vertices for the longest path:\n");
                                char buffer[1024];
                                read(new_socket, buffer, sizeof(buffer));
                                int start, end;
                                sscanf(buffer, "%d %d", &start, &end);
                                int longestPath = currentGraph.getLongestPath(start, end);
                                sendMessage(new_socket, "Longest path: " + std::to_string(longestPath) + "\n");
                                break;
                            }
                            case 6: {
                                sendMessage(new_socket, "Enter 'start' and 'end' vertices for the shortest path:\n");
                                char buffer[1024];
                                read(new_socket, buffer, sizeof(buffer));
                                int start, end;
                                sscanf(buffer, "%d %d", &start, &end);
                                int shortestPath = currentGraph.getShortestPath(start, end);
                                sendMessage(new_socket, "Shortest path: " + std::to_string(shortestPath) + "\n");
                                break;
                            }
                            case 7: {
                                double avgDistance = currentGraph.getAverageDistance();
                                sendMessage(new_socket, "Average distance: " + std::to_string(avgDistance) + "\n");
                                break;
                            }
                            case 8:
                                sendMessage(new_socket, "MST Edges:\n");
                                currentGraph.printMST();
                                break;
                            case 9:
                                sendMessage(new_socket, "Goodbye!\n");
                                close(new_socket);
                                return;
                            default:
                                sendMessage(new_socket, "Invalid choice. Please try again.\n");
                        }

                        // Send the menu again for the next operation
                        sendMessage(new_socket, menu);
                    });
                });
            }
        });
    }
};

// This will handle incoming requests asynchronously
int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    // Binding and listening
    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 3);

    PipelineServer pipelineServer;

    std::cout << "Server listening on port 8080\n";

    while (true) {
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        std::cout << "New client connected!" << std::endl;
        pipelineServer.handleRequest(new_socket);  // Each request handled by the pipeline
    }

    return 0;
};
//make
// ./pipeline_server
// Server listening on port 8080
//open a new terminal
//nc localhost 8080