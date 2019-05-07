#ifndef CLIENT_H
#define CLIENT_H
#include <grpcpp/grpcpp.h>
#include <iostream>
#include "client-server.grpc.pb.h"

using secure_voice_call::Greeter;
using secure_voice_call::AuthorizationRequest;
using secure_voice_call::AuthorizationResponse;

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

namespace secure_voice_call {
class Client {
public:
    Client(std::shared_ptr<Channel> channel) : mstub(Greeter::NewStub(channel)) {}

    Status sendAuthorizationRequest(const std::string &name, bool &authorizationSuccesfull);

private:
    std::unique_ptr<Greeter::Stub> mstub;
};
}

#endif // CLIENT_H
