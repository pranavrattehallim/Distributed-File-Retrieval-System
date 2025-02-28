#ifndef INDEX_STORE_HPP
#define INDEX_STORE_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <shared_mutex>
#include <algorithm>

// IndexStore class handles document indexing and querying
class IndexStore {
public:
    // Constructor initializes the document counter
    IndexStore();

    // Updates the TermInvertedIndex with terms and their frequencies for a document, including client ID
    void updateIndex(const std::string& clientID, int documentNumber, const std::vector<std::pair<std::string, int>>& termFrequencyList);

    // 1.1. Adds a document to the DocumentMap and returns a unique document number
    int putDocument(const std::string& documentPath);

    // 1.2. Retrieves the document path given a document number
    std::string getDocument(int documentNumber) const;

    // 1.3. Queries the TermInvertedIndex for a term and returns a list of document IDs and frequencies
    std::vector<std::pair<std::string, int>> lookupIndex(const std::string& lowertermfromPE) const;

    // 1.4. Retrieves the top N results for the given terms, sorted by frequency and considering the AND search logic
    std::vector<std::pair<std::string, int>> getTopResults(const std::vector<std::string>& terms, size_t topN);

private:
    // Document counter for generating unique document numbers
    int documentCounter;

    // Mapping of document number to document path
    std::unordered_map<int, std::string> documentMap;

    // Mapping of document path to document number
    std::unordered_map<std::string, int> pathToNumber;

    // Inverted index: maps terms to a list of document IDs (including client ID) and their frequencies
    std::unordered_map<std::string, std::vector<std::pair<std::string, int>>> termInvertedIndex;

    // Mutexes for protecting shared data
    mutable std::shared_mutex documentMutex;         // Shared mutex for documentMap and pathToNumber
    mutable std::shared_mutex invertedIndexMutex;    // Shared mutex for termInvertedIndex
};

#endif // INDEX_STORE_HPP
