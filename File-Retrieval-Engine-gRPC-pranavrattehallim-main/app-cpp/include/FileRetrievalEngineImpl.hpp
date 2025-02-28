#ifndef FILERETRIEVALENGINEIMPL_HPP
#define FILERETRIEVALENGINEIMPL_HPP

#include "proto/File-Retrieval-Engine.grpc.pb.h"  // gRPC generated headers
#include "IndexStore.hpp"  // Assuming IndexStore manages document indexing
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <grpc/grpc.h>
#include <grpcpp/server_context.h>


class FileRetrievalEngineImpl : public fre::FileRetrievalEngine::Service {
public:
    // Constructor accepts a shared pointer to IndexStore for managing document data
    explicit FileRetrievalEngineImpl(std::shared_ptr<IndexStore> store);

    // gRPC method to handle indexing requests from the client
    grpc::Status ComputeIndex(grpc::ServerContext* context, const fre::IndexReq* request, fre::IndexRep* reply) override;

    // gRPC method to handle search requests from the client
    grpc::Status ComputeSearch(grpc::ServerContext* context, const fre::SearchReq* request, fre::SearchRep* reply) override;

private:
    std::shared_ptr<IndexStore> store_;  // Shared pointer to IndexStore
};

#endif // FILERETRIEVALENGINEIMPL_HPP
