#ifndef SERVER_H
#define SERVER_H
#include <grpcpp/grpcpp.h>
#include <iostream>
#include "helloworld.grpc.pb.h"


using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;


using helloworld::HelloReply;
using helloworld::HelloRequest;
using helloworld::Greeter;

namespace secure_voice_call {
class Server final : public Greeter::Service
{
public:
    virtual Status SayHello(ServerContext* context,
                            const HelloRequest* request,
                            HelloReply* response) override;
};
}


#endif // SERVER_H
