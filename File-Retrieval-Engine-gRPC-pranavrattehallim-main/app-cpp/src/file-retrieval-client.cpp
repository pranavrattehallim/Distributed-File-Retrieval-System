#include "ClientAppInterface.hpp"
#include "ClientProcessingEngine.hpp"
#include <iostream>

int main() {
    // Initialize the ClientProcessingEngine
    ClientProcessingEngine clientEngine;

    // Initialize the ClientAppInterface with the ClientProcessingEngine
    ClientAppInterface clientApp(clientEngine);

    // Run the client application interface (handles user interaction and commands)
    clientApp.run();

    // Successful termination
    return 0;
}
