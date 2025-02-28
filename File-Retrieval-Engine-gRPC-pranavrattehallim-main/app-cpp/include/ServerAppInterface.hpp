#ifndef SERVER_APP_INTERFACE_HPP
#define SERVER_APP_INTERFACE_HPP

#include "ServerProcessingEngine.hpp" // Include the header for ServerProcessingEngine
#include <vector>
#include <string>

class ServerAppInterface {
public:
    // Constructor
    ServerAppInterface(ServerProcessingEngine& engine);

    // Main command-line interface for running the server app
    void run();

private:
    // Reference to the ServerProcessingEngine for forwarding commands
    ServerProcessingEngine& serverEngine;

    // Display available menu options
    void showMenu();

    // Handle listing of connected clients
    void handleClientListRequest();
};

#endif // SERVER_APP_INTERFACE_HPP
