#include "ServerAppInterface.hpp" // Include the header for the ServerAppInterface
#include <thread> // Include for threading capabilities
#include <iostream> // Include for console input/output operations

// Constructor for the ServerAppInterface class, takes a reference to ServerProcessingEngine
ServerAppInterface::ServerAppInterface(ServerProcessingEngine& engine) : serverEngine(engine) {}

// Main command-line loop for running the server app interface
void ServerAppInterface::run() {
    // Start a new thread to handle command input
    std::thread commandThread([this]() {
        std::string command; // String to hold user command
        while (true) { // Infinite loop to keep accepting commands
            showMenu(); // Display the command menu
            std::cout << "Enter command: "; // Prompt user for input
            std::getline(std::cin, command); // Read the command from standard input

            // Check the command and call appropriate handler
            if (command == "quit" || command == "exit") {
                std::cout << "Shutting down the server..." << std::endl;
                serverEngine.shutdown(); // Call shutdown on the server engine to stop the gRPC server
                std::cout << "Server application exited." << std::endl;
                exit(0);  // Safely exit the application after shutdown
            } else {
                std::cout << "Invalid command. Please try again." << std::endl; // Handle invalid input
            }
        }
    });

    commandThread.detach(); // Detach the thread so it runs independently

    // Keep the main thread alive to allow the server to run
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));  // Sleep for 1 second in each loop
    }
}

// Display available menu options to the user
void ServerAppInterface::showMenu() {
    std::cout << "\n=== Server Command Menu ===" << std::endl; // Header for the menu
    std::cout << "1. quit/exit - Exit the server application" << std::endl; // Option to quit
}

