#pragma once

#include <map>
#include <iostream>
#include <grpcpp/grpcpp.h>
#include "client-server.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using grpc::ServerReaderWriter;

using secure_voice_call::Greeter;
using secure_voice_call::AuthorizationRequest;
using secure_voice_call::AuthorizationResponse;

namespace secure_voice_call {
    class Server final : public Greeter::Service
    {
    private:
        std::map<std::string, std::string> m_clientOnline;

    public:
        void addClientsToResponse(AuthorizationResponse &response);
        // Service interface
    public:
        Status Authorization(ServerContext *context, ServerReaderWriter<AuthorizationResponse, AuthorizationRequest> *stream) override;
    };
}
