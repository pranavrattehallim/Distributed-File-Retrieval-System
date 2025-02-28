#include "ServerProcessingEngine.hpp"
#include "ServerAppInterface.hpp"
#include "IndexStore.hpp"
#include "FileRetrievalEngineImpl.hpp"
#include <iostream>

int main() {
    int serverPort = 50051; // Define the server port

    // Create a shared IndexStore instance
    auto indexStore = std::make_shared<IndexStore>();

    // Initialize the ServerProcessingEngine with the IndexStore
    ServerProcessingEngine serverEngine(indexStore);

    // Initialize the ServerAppInterface with a reference to the ServerProcessingEngine
    ServerAppInterface serverApp(serverEngine);

    // Start the server on the specified port
    serverEngine.initialize(serverPort);

    // Run the server application interface (this handles the server's command-line interface)
    serverApp.run();

    return 0;
}
