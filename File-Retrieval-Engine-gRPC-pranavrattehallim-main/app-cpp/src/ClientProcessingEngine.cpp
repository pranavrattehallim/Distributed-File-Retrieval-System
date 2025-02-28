#include "ClientProcessingEngine.hpp"

namespace fs = std::filesystem; // Alias for filesystem namespace for easier usage

// Constructor for the ClientProcessingEngine class
ClientProcessingEngine::ClientProcessingEngine() {
    // Initialize gRPC
    grpc::ChannelArguments channel_args; // Create channel arguments
    channel_args.SetMaxReceiveMessageSize(INT_MAX); // Set max message size for receiving
    // Create a gRPC stub for communication with the server
    stub_ = fre::FileRetrievalEngine::NewStub(grpc::CreateCustomChannel("localhost:50051", grpc::InsecureChannelCredentials(), channel_args));
}

// Method to connect to the server using IP address and port
bool ClientProcessingEngine::connect(const std::string& server_ip, int server_port) {
    std::cout << "gRPC Client initialized and ready to connect to the server at " << server_ip << ":" << server_port << std::endl;

    // Request the client ID from the server after connecting
    grpc::ClientContext context; // Create a client context for the request
    fre::ConnectReq connectRequest; // Create a ConnectReq object
    fre::ConnectRep connectResponse; // Prepare a response object

    // gRPC: Call the server to establish a connection and receive the client ID
    grpc::Status status = stub_->GetClientID(&context, connectRequest, &connectResponse);
    if (status.ok()) {
        // Successfully connected and received the client ID
        clientID = connectResponse.client_id(); // Store the received client ID
        std::cout << "[INFO] Connected to server with Client ID: " << clientID << std::endl;
    } else {
        std::cerr << "Failed to connect to server: " << status.error_message() << std::endl;
        return false; // Handle connection failure
    }

    return true; // Connection successful
}

// Method to handle shutdown notification from the server
grpc::Status ClientProcessingEngine::Shutdown(grpc::ServerContext* context, const fre::ShutdownReq* request, fre::ShutdownRep* response) {
    std::cout << "[DEBUG-CPE] ClientProcessingEngine::Shutdown - Received shutdown notification from server." << std::endl;

    response->set_message("Client is shutting down.");

    // Log the response message before exiting
    std::cout << "[DEBUG-CPE] ClientProcessingEngine::Shutdown - Sending shutdown response to server: " << response->message() << std::endl;

    shutdown_requested_ = true; // Set the shutdown flag
    return grpc::Status::OK; // Indicate success
}


// Method to index a folder and its contents
bool ClientProcessingEngine::indexFolder(const std::string& folder_path) {
    auto start = std::chrono::high_resolution_clock::now(); // Start timing the indexing process

    // Check if the provided folder path exists and is a directory
    if (!fs::exists(folder_path) || !fs::is_directory(folder_path)) {
        std::cerr << "Error: Invalid folder path: " << folder_path << std::endl; // Log error if invalid
        return false; // Return failure
    }

    size_t totalBytes = 0; // Initialize total bytes counter

    // gRPC: Prepare to send index requests
    for (const auto& entry : fs::recursive_directory_iterator(folder_path)) { // Iterate over each file in the directory recursively
        if (entry.is_regular_file()) { // Check if the entry is a regular file
            std::string filePath = entry.path().string(); // Get the file path as a string

            // Extract word frequencies from the file
            std::unordered_map<std::string, int> wordFrequencies = extractWordFrequencies(filePath);

            // gRPC: Prepare computeIndex request
            grpc::ClientContext context; // Create a client context for the request
            fre::IndexReq request; // Create an IndexReq object for the indexing request
            fre::IndexRep response; // Create an IndexRep object for the response

            request.set_client_id(clientID); // Set the client ID in the request
            request.set_document_path(filePath); // Set the document path in the request

            // Populate the request with word frequencies
            for (const auto& pair : wordFrequencies) {
                auto term_freq = request.add_word_frequencies(); // Add a new word frequency to the request
                term_freq->set_word(pair.first); // Set the word
                term_freq->set_count(pair.second); // Set the count for the word
            }

            // gRPC: Call the server to process the index request
            grpc::Status status = stub_->ComputeIndex(&context, request, &response);
            if (!status.ok()) { // Check if the gRPC call was successful
                std::cerr << "gRPC call failed: " << status.error_message() << std::endl;
                return false; // Return failure on gRPC call failure
            }

            totalBytes += fs::file_size(filePath);  // Accumulate total bytes processed
        }
    }

    auto end = std::chrono::high_resolution_clock::now(); // End timing the process
    std::chrono::duration<double> duration = end - start; // Calculate duration

    // Log total bytes indexed and duration
    std::cout << "Completed indexing " << totalBytes << " bytes of data" << std::endl;
    std::cout << "Completed indexing in " << duration.count() << " seconds" << std::endl;

    return true; // Return success after timing and logging
}

// Method to search for terms in the indexed data
bool ClientProcessingEngine::search(const std::vector<std::string>& query_terms) {
    if (query_terms.empty()) { // Check if there are no search terms
        std::cerr << "Please provide at least 1 search term." << std::endl; // Log error for no terms
        return false; // Return failure
    }

    // gRPC: Prepare search request
    grpc::ClientContext context; // Create a client context for the request
    fre::SearchReq request; // Create a SearchReq object for the search request
    fre::SearchRep response; // Create a SearchRep object for the response

    for (const auto& term : query_terms) {
        request.add_terms(term); // Add each term to the search request
    }

    // gRPC: Call the server to process the search request
    grpc::Status status = stub_->ComputeSearch(&context, request, &response);
    if (!status.ok()) { // Check if the gRPC call was successful
        std::cerr << "gRPC search failed: " << status.error_message() << std::endl;
        return false; // Return failure on gRPC call failure
    }

    // Optional: Log the completion message from the server
    std::cout << "Server message: " << response.message() << std::endl; // Log the server's message
    // Log the search results
    for (const auto& result : response.documents()) { // Iterate through search results
        std::cout << "ClientID:Document Path: " << result.path() << ", Count: " << result.count() << std::endl; // Log each result
    }

    return true; // Return success
}

// Helper method to extract word frequencies from a document
std::unordered_map<std::string, int> ClientProcessingEngine::extractWordFrequencies(const std::string& file_path) {
    std::unordered_map<std::string, int> wordFrequencies; // Map to hold word frequencies
    std::ifstream file(file_path); // Open the file
    if (!file) { // Check if the file opened successfully
        std::cerr << "Failed to open file: " << file_path << std::endl; // Log error if failed
        return wordFrequencies; // Return empty map
    }

    std::string word; // String to hold the current word
    char ch; // Variable to hold each character read from the file
    while (file.get(ch)) { // Read characters from the file one by one
        if (std::isalnum(ch)) { // Check if the character is alphanumeric
            word += ch; // Retain case-sensitive characters
        } else { // If not alphanumeric
            if (!word.empty() && word.back() == 's' && ch == '\'') { // Handle possessive case
                word.pop_back(); // Remove trailing 's' if it's possessive
            }
            if (word.length() > 2) { // Check if word length is greater than 2
                ++wordFrequencies[word]; // Increment the count for the word
            }
            word.clear(); // Clear the current word for the next one
        }
    }
    if (word.length() > 2) { // Check the last word after exiting loop
        ++wordFrequencies[word]; // Increment its count if valid
    }

    return wordFrequencies; // Return the map of word frequencies
}

