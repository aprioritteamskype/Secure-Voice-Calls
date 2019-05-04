#ifndef CLIENT_H
#define CLIENT_H
#include <grpcpp/grpcpp.h>
#include <iostream>
#include "helloworld.grpc.pb.h"

using helloworld::HelloReply;
using helloworld::HelloRequest;
using helloworld::Greeter;

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

namespace secure_voice_call {
class Client {
public:
    Client(std::shared_ptr<Channel> channel) : mstub(helloworld::Greeter::NewStub(channel)) {}

    std::string sendRequest(const std::string &requestStr);

private:
    std::unique_ptr<helloworld::Greeter::Stub> mstub;
};
}

#endif // CLIENT_H
