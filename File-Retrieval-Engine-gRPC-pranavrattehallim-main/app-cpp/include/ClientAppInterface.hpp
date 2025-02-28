#ifndef CLIENT_APP_INTERFACE_HPP
#define CLIENT_APP_INTERFACE_HPP

#include <string>
#include <vector>
#include "ClientProcessingEngine.hpp"  // Include the ClientProcessingEngine class

class ClientAppInterface {
public:
    // Constructor: Initializes ClientAppInterface with a reference to the ClientProcessingEngine
    ClientAppInterface(ClientProcessingEngine& engine);

    // Main method to start the client interface loop and process user commands
    void run();

private:
    // Reference to the ClientProcessingEngine for handling client operations
    ClientProcessingEngine& processingEngine;

    // Flag to track if the indexing has been completed
    bool indexed;
};

#endif // CLIENT_APP_INTERFACE_HPP
