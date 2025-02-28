#include "IndexStore.hpp"
#include <shared_mutex>  // Include shared mutex for read-write locking
#include <algorithm>     // For sort function
#include <iostream>      // For input and output streams
#include <unordered_map> // For using std::unordered_map

// Constructor initializes the document counter to 0
IndexStore::IndexStore() : documentCounter(1) {}

// Mutex to protect shared resources during concurrent access
std::shared_mutex documentMutex;         // Shared mutex to protect documentMap and pathToNumber
std::shared_mutex invertedIndexMutex;    // Shared mutex to protect termInvertedIndex

// 1.1. Adds a document to the index, assigns a unique number, and returns it
int IndexStore::putDocument(const std::string& documentPath) {
    // Lock the mutex exclusively to ensure only one thread modifies the DocumentMap at a time
    std::unique_lock<std::shared_mutex> lock(documentMutex);

    // Check if the document is already indexed by its path
    if (pathToNumber.find(documentPath) != pathToNumber.end()) {
        return pathToNumber[documentPath];  // Return existing document number if found
    }

    // Assign a new document number and update the mappings
    int docNumber = documentCounter++; // Increment the document counter for unique document numbe
    documentMap[docNumber] = documentPath;         // Map document number to path (includes Client ID)
    pathToNumber[documentPath] = docNumber;        // Map path to document number
    // std::cout << "[DEBUG] Contents of pathToNumber:" << std::endl;
    // for (const auto& pair : pathToNumber) {
    //     std::cout << "Path: " << pair.first << ", Document Number: " << pair.second << std::endl;
    // }
    return docNumber; // Return the new document number
}

// 1.2. Retrieves the document path given its unique number
std::string IndexStore::getDocument(int documentNumber) const {
    // Lock the shared mutex for reading, allowing multiple threads to access the documentMap simultaneously
    std::shared_lock<std::shared_mutex> lock(documentMutex);

    auto it = documentMap.find(documentNumber);   // Find the document by its number
    if (it != documentMap.end()) {
        return it->second;  // Return the document path (which includes Client ID) if found
    } else {
        return "Error: Document number does not exist.";  // Return error if not found
    }
}

// 1.3. Updates the inverted index with terms and their frequencies for a specific document
void IndexStore::updateIndex(const std::string& clientID, int documentNumber, const std::vector<std::pair<std::string, int>>& termFrequencyList) {
    // Lock the mutex exclusively to ensure only one thread modifies the TermInvertedIndex at a time
    std::unique_lock<std::shared_mutex> lock(invertedIndexMutex);  // Acquire lock on entry, release on exit

    // Get the document path using the document number
    std::string documentPath = getDocument(documentNumber); // Get the document path

    // Iterate over each term and its frequency in the list
    for (const auto& termFrequency : termFrequencyList) {
        const std::string& term = termFrequency.first; // Get the term
        int frequency = termFrequency.second;          // Get the frequency

        // Create the entry key in the format "clientID:documentPath"
        std::string entryKey = clientID + ":" + documentPath;

        // DEBUG: Print the generated entry key
        // std::cout << "[DEBUG-UpdateIndex] Generated Entry Key: " << entryKey << std::endl;

        // Check if the term is already in the inverted index
        if (termInvertedIndex.find(term) == termInvertedIndex.end()) {
            // Add the term with the client ID and document path
            termInvertedIndex[term] = {{entryKey, frequency}};
        } else {
            bool updated = false; // Flag to check if the term was updated
            // Check if the document already exists for this term
            for (auto& entry : termInvertedIndex[term]) {
                if (entry.first == entryKey) {
                    entry.second += frequency; // Correctly add the frequency to the existing value
                    updated = true; // Mark as updated
                    break; // Exit the loop as the entry is found
                }
            }
            // If the document was not found, add it to the list for this term
            if (!updated) {
                termInvertedIndex[term].emplace_back(entryKey, frequency);
            }
        }
    }
}



// 1.4. Retrieves a list of documents and term frequencies for a given term
std::vector<std::pair<std::string, int>> IndexStore::lookupIndex(const std::string& termfromImpl) const {
    // Lock the shared mutex for reading, allowing multiple threads to access the TermInvertedIndex simultaneously
    std::shared_lock<std::shared_mutex> lock(invertedIndexMutex);

    auto it = termInvertedIndex.find(termfromImpl);  // Find the term in the inverted index
    if (it != termInvertedIndex.end()) {
        // Store the result in a temporary variable for logging
        const std::vector<std::pair<std::string, int>>& results = it->second;

        // Log the contents of the results
        // std::cout << "[DEBUG] Lookup results for term '" << termfromImpl << "':\n";
        // for (const auto& [docID, freq] : results) {
        //     std::cout << "Document ID: " << docID << ", Frequency: " << freq << std::endl;
        // }

        return results;  // Return the list of document IDs and frequencies
    } else {
        return {};  // Return an empty list if the term is not found
    }
}


// Retrieves the top N documents sorted by frequency for the given search terms
std::vector<std::pair<std::string, int>> IndexStore::getTopResults(const std::vector<std::string>& terms, size_t topN) {
    std::unordered_map<std::string, int> docFrequencyMap; // Map to hold document frequencies
    bool firstTerm = true; // Flag to indicate if processing the first term

    // Collect document frequencies for each term, supporting AND searches
    for (const auto& term : terms) {
        std::vector<std::pair<std::string, int>> termResults = lookupIndex(term); // Get results for the current term
        std::unordered_map<std::string, int> currentTermDocuments; // Map to hold current term documents

        // Collect frequencies of documents for the current term
        for (const auto& result : termResults) {
            currentTermDocuments[result.first] = result.second; // Store document ID and frequency
        }

        if (firstTerm) {
            docFrequencyMap = currentTermDocuments; // Initialize with first term results
            firstTerm = false; // Update flag
        } else {
            // Perform intersection of documents for AND search (common in both terms)
            for (auto it = docFrequencyMap.begin(); it != docFrequencyMap.end();) {
                if (currentTermDocuments.find(it->first) != currentTermDocuments.end()) {
                    it->second += currentTermDocuments[it->first];  // Sum frequencies if document is in both
                    ++it; // Move to the next document
                } else {
                    it = docFrequencyMap.erase(it);  // Remove documents not in the current term
                }
            }
        }
    }

    // Convert the map to a vector for sorting
    std::vector<std::pair<std::string, int>> sortedResults(docFrequencyMap.begin(), docFrequencyMap.end());

    // Sort results by frequency in descending order
    std::sort(sortedResults.begin(), sortedResults.end(), [](const auto& a, const auto& b) {
        return a.second > b.second; // Sort by frequency
    });

    // Limit results to the top N
    if (sortedResults.size() > topN) {
        sortedResults.resize(topN); // Resize to top N results
    }

    return sortedResults; // Return the sorted top results
}
