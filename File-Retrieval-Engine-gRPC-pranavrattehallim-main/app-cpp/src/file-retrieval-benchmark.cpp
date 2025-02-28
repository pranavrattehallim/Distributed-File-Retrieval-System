#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <sstream>
#include <cstdlib> // for exit
#include <algorithm> // for std::transform
#include "ClientProcessingEngine.hpp" // Ensure this header is included
#include "Benchmark.hpp"

void benchmarkClient(ClientProcessingEngine& clientEngine, const std::string& dataset_path) {
    // Index the folder
    if (!clientEngine.indexFolder(dataset_path)) {
        std::cerr << "Failed to index folder: " << dataset_path << std::endl;
        return;
    }

    std::cout << "Client finished indexing: " << dataset_path << std::endl;
}

int main() {
    int num_clients;
    std::string server_ip;
    int server_port;

    // Ask for the number of clients
    std::cout << "Enter the number of clients: ";
    std::cin >> num_clients;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear input buffer

    // Ask for the server IP address
    std::cout << "Enter the server IP address: ";
    std::getline(std::cin, server_ip);  // Use getline to ensure the full input is captured

    // Ask for the server port
    std::cout << "Enter the server port: ";
    std::cin >> server_port;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear input buffer

    std::vector<std::string> dataset_paths(num_clients); // Allocate space for the dataset paths
    for (int i = 0; i < num_clients; ++i) {
        std::cout << "Enter the path for dataset " << (i + 1) << ": ";
        std::getline(std::cin, dataset_paths[i]);  // Ensure each dataset path is properly inputted
    }

    // Collect search terms at the start
    std::cout << "Enter search command: ";
    std::string search_command;
    std::getline(std::cin, search_command); // Collect search terms once

    // Trim whitespace and prepare the search command
    search_command.erase(0, search_command.find_first_not_of(' ')); // Trim leading spaces
    search_command.erase(search_command.find_last_not_of(' ') + 1); // Trim trailing spaces

    // Split the command into search terms
    std::istringstream iss(search_command);
    std::vector<std::string> query_terms;
    std::string term;

    // Extract each term from the input and add it to the vector
    while (iss >> term) {
    query_terms.push_back(term);  // Add the term to the vector
    }

    // Check if any search terms were provided
    if (query_terms.empty()) {
    std::cerr << "No search terms provided." << std::endl;
    return EXIT_FAILURE;
    }


    std::vector<ClientProcessingEngine> clients(num_clients);
    std::vector<std::thread> threads;

    // Create and run threads for each client
    for (int i = 0; i < num_clients; ++i) {
        // std::cout << "[DEBUG] Connecting client " << (i + 1) << " to the server..." << std::endl;
        if (!clients[i].connect(server_ip, server_port)) {
            std::cerr << "Failed to connect client " << (i + 1) << std::endl;
            return EXIT_FAILURE;
        }
        threads.emplace_back(benchmarkClient, std::ref(clients[i]), dataset_paths[i]);
    }

    // Join all threads
    for (auto& t : threads) {
        // std::cout << "[DEBUG] Waiting for thread to join..." << std::endl;
        t.join();
    }

    // std::cout << "[DEBUG] All clients finished indexing." << std::endl;

    // Perform search queries using the first available client after indexing
    // std::cout << "[DEBUG] Performing search with terms: ";
    for (const auto& term : query_terms) {
        std::cout << term << " ";
    }
    std::cout << std::endl;

    // Using client 0 (or any available client) for search
    if (!clients[0].search(query_terms)) { // Use search method to handle the query
        std::cerr << "Search failed." << std::endl;
    }

    return EXIT_SUCCESS;
}

