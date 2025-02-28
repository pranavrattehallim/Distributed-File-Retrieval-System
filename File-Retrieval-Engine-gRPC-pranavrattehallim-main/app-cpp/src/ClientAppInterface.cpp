#include "ClientAppInterface.hpp"  // Include the header file for the ClientAppInterface class
#include <iostream>  // Include for input and output stream
#include <sstream>  // Include for string stream functionality
#include <string>  // Include for string manipulations
#include <vector>  // Include for using vectors (dynamic arrays)
#include <limits>  // Include for numeric limits
#include <chrono>  // Include for time measurement

// Constructor initializes the ClientAppInterface with a reference to a ClientProcessingEngine
ClientAppInterface::ClientAppInterface(ClientProcessingEngine& engine) : processingEngine(engine), indexed(false) {}

// Main loop of the application that handles user interactions and commands
void ClientAppInterface::run() {
    std::string command;  // Variable to store the user input command

    // Prompt the user for the server's IP and port before showing other options
    std::string serverIP;  // Variable to store the server IP address
    int serverPort = 0;  // Variable to store the server port number

    std::cout << "> Please enter the server IP address: ";  // Prompt for IP address input
    std::getline(std::cin, serverIP);  // Read the entire line input for the server IP

    std::cout << "> Please enter the server port: ";  // Prompt for port input
    while (true) {  // Infinite loop to ensure valid input
        std::cin >> serverPort;  // Read the server port
        if (serverPort > 0) {  // Check if the port number is positive
            break;  // If valid input (positive number), break the loop
        }
        std::cout << "> Invalid input. Please enter a valid server port: ";  // Prompt for valid port input
    }

    // Clear the input buffer to prepare for the next input
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // Try to connect to the server
    std::cout << "Connecting to server " << serverIP << ":" << serverPort << "..." << std::endl;  // Display connection attempt
    if (!processingEngine.connect(serverIP, serverPort)) {  // Call the connect method on the processing engine
        std::cout << "Failed to connect to the server." << std::endl;  // Inform the user of a failed connection
        return;  // Exit the run method if connection fails
    }
    std::cout << "Connected to the server successfully." << std::endl;  // Inform the user of a successful connection

    // Infinite loop to continuously prompt the user for commands
    while (true) {
        // Display available options based on whether indexing has been performed
        if (indexed) {
            std::cout << "> Options available: search <Terms> | quit" << std::endl;  // Options if indexed
        } else {
            std::cout << "> Options available: index <Folder path> | search <Terms> | quit" << std::endl;  // Options if not indexed
        }

        std::cout << "> ";  // Display the command prompt
        std::getline(std::cin, command);  // Read the user's command input

        // Handle the "quit" command to exit the program
        if (command == "quit") {
            std::cout << "Thanks for using the File Retrieval Engine!" << std::endl;  // Farewell message
            break;  // Exit the loop, ending the program
        }
        // Handle the "index" command to index the contents of a specified folder
        else if (command.rfind("index ", 0) == 0) {  // Check if command starts with "index "
            std::string folderPath = command.substr(6);  // Extract the folder path from the command
            std::cout << "Indexing folder: " << folderPath << "..." << std::endl;  // Inform user about indexing
            if (processingEngine.indexFolder(folderPath)) {  // Call indexFolder on the processing engine
                std::cout << "Indexing completed successfully." << std::endl;  // Confirm successful indexing
                indexed = true;  // Set the indexed flag to true after indexing
                continue;  // Go back to the top of the loop to ask for commands again
            } else {
                std::cout << "Failed to index the folder. Please check the path." << std::endl;  // Inform user of failure
            }
        }
        // Handle the "search" command to search for terms within the indexed documents
        else if (command.rfind("search ", 0) == 0) {  // Check if command starts with "search "
            std::istringstream ss(command.substr(7));  // Create a string stream from the search terms
            std::vector<std::string> terms;  // Vector to hold the search terms
            std::string term;  // Variable to hold each term

            // Extract each term from the input and add it to the vector
            while (ss >> term) {
                terms.push_back(term);  // Add the term to the vector
            }

            // Simply pass the terms to ClientProcessingEngine and let it handle everything else
            if (!terms.empty()) {
                processingEngine.search(terms);  // Call search function in ClientProcessingEngine
            } else {
                std::cout << "Please provide at least 1 search term." << std::endl;  // In case no terms were provided
            }
        }

        // Handle invalid commands that do not match any of the expected patterns
        else {
            std::cout << "Invalid command. Please try again." << std::endl;  // Prompt user to try again
        }
         // Check for shutdown request after processing commands
        if (processingEngine.isShutdownRequested()) {
        std::cout << "Client is shutting down as per server request." << std::endl;
        break;  // Exit the loop due to shutdown request
        }
    }
}
