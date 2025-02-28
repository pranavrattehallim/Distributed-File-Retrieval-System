#ifndef CLIENT_PROCESSING_ENGINE_HPP
#define CLIENT_PROCESSING_ENGINE_HPP

#include <iostream> // Include iostream for input/output streams
#include <fstream> // Include fstream for file handling
#include <algorithm> // Include algorithm for various utility functions
#include <cctype> // Include cctype for character classification functions
#include <chrono> // Include chrono for time-related functions
#include <filesystem> // Include filesystem for file and directory manipulation
#include <vector> // Include vector for dynamic array
#include <unordered_map> // Include unordered_map for hash table based mapping
#include <grpcpp/grpcpp.h> // Include general gRPC headers
#include <grpcpp/support/channel_arguments.h> // Include for ChannelArguments
#include <grpcpp/security/credentials.h> // Include for InsecureChannelCredentials
#include <string> // Include string for string manipulation

#include "proto/File-Retrieval-Engine.grpc.pb.h" // Include gRPC definitions

class ClientProcessingEngine {
public:
    // Constructor: Initializes gRPC client stub
    ClientProcessingEngine();

    // Connects the client to the server with the provided IP address and port
    bool connect(const std::string& server_ip, int server_port);

    // Indexes the specified folder and sends an INDEX REQUEST to the server via gRPC
    bool indexFolder(const std::string& folder_path);

    // Sends a SEARCH REQUEST with query terms and returns the top 10 relevant documents via gRPC
    bool search(const std::vector<std::string>& query_terms);

    // Handles shutdown notification from the server
    grpc::Status Shutdown(grpc::ServerContext* context, const fre::ShutdownReq* request, fre::ShutdownRep* response);
    
    bool isShutdownRequested() const { return shutdown_requested_; }

private:
    std::unique_ptr<fre::FileRetrievalEngine::Stub> stub_; // gRPC client stub for server communication
    std::string clientID; // Client ID used for indexing
    bool shutdown_requested_ = false;

    // Extracts word frequencies from the specified document file
    std::unordered_map<std::string, int> extractWordFrequencies(const std::string& file_path);
};

#endif // CLIENT_PROCESSING_ENGINE_HPP

