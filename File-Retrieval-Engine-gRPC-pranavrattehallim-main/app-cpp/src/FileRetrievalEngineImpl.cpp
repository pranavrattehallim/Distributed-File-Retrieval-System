#include "FileRetrievalEngineImpl.hpp" // Include the header for FileRetrievalEngineImpl
#include <iostream> // Include for console input/output operations
#include <algorithm> // Include for sorting operations
#include <chrono>
#include <sstream> // For constructing the result message

// Constructor for FileRetrievalEngineImpl
FileRetrievalEngineImpl::FileRetrievalEngineImpl(std::shared_ptr<IndexStore> store) : store_(std::move(store)) {
    // Initializes the index store for managing documents
}

// Handles indexing requests from the client
grpc::Status FileRetrievalEngineImpl::ComputeIndex(
        grpc::ServerContext* context,
        const fre::IndexReq* request,
        fre::IndexRep* reply)
{
    // Extract document path and client ID from the request
    std::string documentPath = request->document_path();
    std::string clientID = request->client_id();

    // Get document number for the path and store word frequencies
    int documentNumber = store_->putDocument(documentPath); // Store the document and get its ID

    // Populate term frequencies vector from request
    std::vector<std::pair<std::string, int>> termFrequencies;
    for (const auto& wordFreq : request->word_frequencies()) {
        termFrequencies.emplace_back(wordFreq.word(), wordFreq.count()); // Store word and its frequency
    }

    // Update the index with document number, client ID, and term frequencies
    store_->updateIndex(clientID, documentNumber, termFrequencies);

    // Set acknowledgment message in the reply
    reply->set_message("Indexing complete for document: " + documentPath);
    // Log the indexed document and associated client ID
    // std::cout << "[INFO-Impl] Indexed document: " << documentPath << " for Client ID: " << clientID << std::endl;

    return grpc::Status::OK; // Return OK status for successful indexing
}

// Handles search requests from the client
grpc::Status FileRetrievalEngineImpl::ComputeSearch(
        grpc::ServerContext* context,
        const fre::SearchReq* request,
        fre::SearchRep* reply)
{
    // Start timing the search request
    auto start = std::chrono::high_resolution_clock::now();

    // Extract search terms from the request
    std::vector<std::string> terms;
    for (const auto& term : request->terms()) {
        if (term == "and") continue; // Skip the term if it's "and"
        terms.push_back(term); // Store each valid search term in a vector
    }

    // Check if enough terms are provided (minimum of 1 as per requirements)
    if (terms.empty()) { // Check if there are no valid search terms
        std::cerr << "Please provide at least 1 search term." << std::endl; // Log error for no terms
        return grpc::Status(grpc::INVALID_ARGUMENT, "No search terms provided."); // Return failure with error status
    }

    // Prepare a map to hold document frequencies for term intersection and frequency accumulation
    std::unordered_map<std::string, int> documentFrequencyMap; // Map of document IDs (as strings) to their frequencies
    bool firstTerm = true; // Flag to check if it’s the first term in the iteration

    for (const auto& term : terms) {
        // Get documents and their frequencies for the current term
        auto termResults = store_->lookupIndex(term); // Lookup index for the term

        if (firstTerm) {
            // Initialize documentFrequencyMap with results from the first term
            for (const auto& [docID, freq] : termResults) {
                documentFrequencyMap[docID] = freq; // Store frequencies for the first term
            }
            firstTerm = false; // Set flag to false for subsequent terms
        } else {
            // Intersect documentFrequencyMap with results from the current term
            for (auto it = documentFrequencyMap.begin(); it != documentFrequencyMap.end();) {
                // Check if the document is also in the current term's results
                auto found = std::find_if(termResults.begin(), termResults.end(),
                                          [&](const auto& pair) { return pair.first == it->first; });

                if (found != termResults.end()) {
                    it->second += found->second; // Accumulate frequency for matching documents
                    ++it; // Move to the next document
                } else {
                    // Remove documents not found in the current term's results
                    it = documentFrequencyMap.erase(it);
                }
            }
        }
    }

    // Sort and retain top 10 documents based on frequency
    std::vector<std::pair<std::string, int>> sortedResults(documentFrequencyMap.begin(), documentFrequencyMap.end());
    std::sort(sortedResults.begin(), sortedResults.end(), [](const auto& a, const auto& b) {
        return a.second > b.second; // Sort in descending order by frequency
    });

    // Capture total results and limit the results to the top 10 documents
    int totalResults = sortedResults.size();
    if (sortedResults.size() > 10) {
        sortedResults.resize(10); // Keep only the top 10 results
    }

    // Prepare the reply message
    reply->set_message("Search completed in " +
                       std::to_string(std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - start).count()) +
                       " seconds. Search results (top " + std::to_string(sortedResults.size()) +
                       " out of " + std::to_string(totalResults) + "):");

    // Add document paths and frequencies to the reply
    for (const auto& [docID, freq] : sortedResults) {
        auto result = reply->add_documents(); // Create a new SearchResult in the response
        result->set_path(docID);               // Set the document path
        result->set_count(freq);               // Set the frequency count
    }

    // Log search completion
    // std::cout << "[DEBUG] Sending SearchRep message: " << reply->message() << std::endl;

    return grpc::Status::OK;
}

