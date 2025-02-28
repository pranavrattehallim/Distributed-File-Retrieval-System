#include "ServerProcessingEngine.hpp" // Include the header file for ServerProcessingEngine
#include <chrono>  // Include for high-resolution clock functionality
#include <algorithm> // Include for std::transform and sorting functions
#include <unordered_map> // Include for using std::unordered_map for efficient lookups
#include <sstream> // Include for std::ostringstream for string stream operations
#include <iostream> // Include for input/output stream operations
#include <grpcpp/grpcpp.h> // Include the gRPC library headers
#include <vector> // Include for std::vector
#include <string> // Include for std::string
#include <memory> // Include for std::shared_ptr
#include <mutex>  // Include for std::mutex to protect client list

// Vector to maintain connected clients
std::vector<ClientConnection> connectedClients;
std::mutex clientsMutex; // Mutex for thread-safe access to connected clients

// Constructor: initializes the IndexStore and FileRetrievalEngineImpl
ServerProcessingEngine::ServerProcessingEngine(std::shared_ptr<IndexStore> store)
    : store(std::move(store)), fileRetrievalEngineImpl(std::make_shared<FileRetrievalEngineImpl>(this->store)) {
    // Initialize FileRetrievalEngineImpl with the shared IndexStore
}

// Starts the gRPC server in a separate thread
void ServerProcessingEngine::initialize(int serverPort) {
    serverThread = std::thread(&ServerProcessingEngine::rungRPCServer, this, serverPort);
}

// Generates a unique client ID as a simple numeric string
std::string ServerProcessingEngine::generateUniqueClientID() {
    static int clientCount = 0; // Simple counter for unique IDs
    return std::to_string(++clientCount); // Increment and return as string
}

// Adds a client to the connected clients list
void ServerProcessingEngine::addClient(const std::string& clientID, std::unique_ptr<fre::FileRetrievalEngine::Stub> clientStub) {
    std::lock_guard<std::mutex> lock(clientsMutex);
    connectedClients.push_back({clientID, std::move(clientStub)}); // Add new client with stub
}

// Removes a client from the connected clients list
void ServerProcessingEngine::removeClient(const std::string& clientID) {
    std::lock_guard<std::mutex> lock(clientsMutex);
    connectedClients.erase(std::remove_if(connectedClients.begin(), connectedClients.end(),
                                           [&clientID](const ClientConnection& client) {
                                               return client.clientID == clientID;
                                           }), 
                                            connectedClients.end());
}

// Notify all connected clients to shut down
void ServerProcessingEngine::notifyClientsToShutdown() {
    std::lock_guard<std::mutex> lock(clientsMutex);
    for (const auto& client : connectedClients) {
        // std::cout << "[DEBUG] ServerProcessingEngine::notifyClientsToShutdown - Notifying client " << client.clientID << " to shut down." << std::endl;

        grpc::ClientContext context;
        fre::ShutdownReq shutdownRequest; // Create shutdown request
        fre::ShutdownRep shutdownResponse; // Prepare response

        // Call the Shutdown method on the client stub
        grpc::Status status = client.stub->Shutdown(&context, shutdownRequest, &shutdownResponse);
        if (!status.ok()) {
            // std::cerr << "[ERROR] ServerProcessingEngine::notifyClientsToShutdown - Failed to notify client " << client.clientID << ": " << status.error_message() << std::endl;
        } else {
            // std::cout << "[DEBUG] ServerProcessingEngine::notifyClientsToShutdown - Shutdown notification sent to client " << client.clientID << "." << std::endl;
        }
    }
}


// Builds and starts the gRPC server
void ServerProcessingEngine::rungRPCServer(int serverPort) {
    grpc::ServerBuilder builder; // Create a gRPC server builder
    builder.AddListeningPort("0.0.0.0:" + std::to_string(serverPort), grpc::InsecureServerCredentials()); // Add a listening port
    builder.RegisterService(this); // Register this service (ServerProcessingEngine) with the server
    server = builder.BuildAndStart(); // Build and start the server
    std::cout << "Server is listening on port " << serverPort << std::endl;
    server->Wait(); // Keep the server running until it is stopped
}

// Shuts down the gRPC server
void ServerProcessingEngine::shutdown() {
    if (server) {
        notifyClientsToShutdown(); // Notify clients before shutting down
        server->Shutdown(); // Initiate server shutdown

        // Wait for the server thread to finish
        if (serverThread.joinable()) {
            serverThread.join();
        }
        // std::cout << "[INFO] Shutdown() called successfully" << std::endl;
    }
}

// gRPC remote procedure to provide a client ID
grpc::Status ServerProcessingEngine::GetClientID(
        grpc::ServerContext* context,
        const fre::ConnectReq* request,
        fre::ConnectRep* response) {
    // Generate a unique client ID
    std::string clientID = generateUniqueClientID();
    
    // Create a new client stub for this connection
    auto clientStub = fre::FileRetrievalEngine::NewStub(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));

    addClient(clientID, std::move(clientStub)); // Add the new client to the list
    response->set_client_id(clientID); // Set the client ID in the response

    std::cout << "[INFO] Provided Client ID: " << clientID << std::endl; // Log the provided Client ID
    return grpc::Status::OK; // Indicate success
}

// gRPC remote procedure for indexing
grpc::Status ServerProcessingEngine::ComputeIndex(
        grpc::ServerContext* context,
        const fre::IndexReq* request,
        fre::IndexRep* response) {
    return fileRetrievalEngineImpl->ComputeIndex(context, request, response);
}

// gRPC remote procedure for searching
grpc::Status ServerProcessingEngine::ComputeSearch(
        grpc::ServerContext* context,
        const fre::SearchReq* request,
        fre::SearchRep* response) {
    return fileRetrievalEngineImpl->ComputeSearch(context, request, response);
}

// gRPC remote procedure for shutdown (to notify clients)
grpc::Status ServerProcessingEngine::Shutdown(
        grpc::ServerContext* context,
        const fre::ShutdownReq* request,
        fre::ShutdownRep* response) {
    response->set_message("Server is shutting down."); // Optional message
    return grpc::Status::OK; // Indicate success
}

