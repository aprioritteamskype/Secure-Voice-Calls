#ifndef SERVER_H
#define SERVER_H
#include <grpcpp/grpcpp.h>
#include <iostream>
#include <map>
#include "client-server.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using secure_voice_call::Greeter;
using secure_voice_call::AuthorizationRequest;
using secure_voice_call::AuthorizationResponse;

namespace secure_voice_call {
class Server final : public Greeter::Service
{
public:
    virtual Status Authorization(ServerContext* context,
                                 const AuthorizationRequest* request,
                                 AuthorizationResponse* response) override;
    private:
    std::map<std::string, std::string> m_clientOnline;
};
}


#endif // SERVER_H
