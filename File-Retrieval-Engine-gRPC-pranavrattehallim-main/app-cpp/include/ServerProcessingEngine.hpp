#ifndef SERVERPROCESSINGENGINE_HPP
#define SERVERPROCESSINGENGINE_HPP

#include <grpcpp/grpcpp.h>
#include <memory>
#include <string>
#include <vector>
#include <thread>
#include <mutex>  // Include for mutex to protect client list
#include "IndexStore.hpp"                 // Include IndexStore for indexing functionalities
#include "FileRetrievalEngineImpl.hpp"     // Include FileRetrievalEngineImpl for indexing and search services

// Client connection structure
class ClientConnection {
public:
    std::string clientID; // Unique identifier for the client
    std::unique_ptr<fre::FileRetrievalEngine::Stub> stub; // Client stub for RPC calls
};

class ServerProcessingEngine final : public fre::FileRetrievalEngine::Service {
public:
    // Constructor: initializes ServerProcessingEngine with a shared IndexStore and FileRetrievalEngineImpl
    explicit ServerProcessingEngine(std::shared_ptr<IndexStore> store);

    // Initializes the server and starts it on a specified port in a new thread
    void initialize(int serverPort);

    // Shuts down the server gracefully and joins the server thread
    void shutdown();

    // gRPC remote procedure for indexing
    grpc::Status ComputeIndex(
        grpc::ServerContext* context,
        const fre::IndexReq* request,
        fre::IndexRep* response) override;

    // gRPC remote procedure for searching
    grpc::Status ComputeSearch(
        grpc::ServerContext* context,
        const fre::SearchReq* request,
        fre::SearchRep* response) override;

    // gRPC remote procedure to provide a client ID
    grpc::Status GetClientID(
        grpc::ServerContext* context,
        const fre::ConnectReq* request,
        fre::ConnectRep* response) override;

    // gRPC remote procedure for shutdown (to notify clients)
    grpc::Status Shutdown(
        grpc::ServerContext* context,
        const fre::ShutdownReq* request,
        fre::ShutdownRep* response) override;

private:
    // Method that builds and starts the gRPC server
    void rungRPCServer(int serverPort);

    // Generates a unique client ID as a simple numeric string
    std::string generateUniqueClientID();

    // Adds a client to the connected clients list
    void addClient(const std::string& clientID, std::unique_ptr<fre::FileRetrievalEngine::Stub> clientStub);

    // Removes a client from the connected clients list
    void removeClient(const std::string& clientID);

    // Notify all connected clients to shut down
    void notifyClientsToShutdown();

    // Member variables
    std::shared_ptr<IndexStore> store;                        // Shared pointer to IndexStore instance
    std::shared_ptr<FileRetrievalEngineImpl> fileRetrievalEngineImpl; // FileRetrievalEngineImpl instance for indexing/search
    std::unique_ptr<grpc::Server> server;                     // Unique pointer to the gRPC server
    std::thread serverThread;                                 // Thread to run the gRPC server
    std::vector<ClientConnection> connectedClients;           // Vector to hold connected clients
    std::mutex clientsMutex;                                  // Mutex for thread-safe access to connected clients
};

#endif // SERVERPROCESSINGENGINE_HPP

