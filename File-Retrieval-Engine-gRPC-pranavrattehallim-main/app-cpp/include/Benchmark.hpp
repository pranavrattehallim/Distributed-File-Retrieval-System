#ifndef BENCHMARK_HPP
#define BENCHMARK_HPP

#include <string>
#include "ClientProcessingEngine.hpp" // Ensure the ClientProcessingEngine is included

// Function prototype for benchmarking the client
void benchmarkClient(ClientProcessingEngine& clientEngine, const std::string& dataset_path);

#endif // BENCHMARK_HPP

