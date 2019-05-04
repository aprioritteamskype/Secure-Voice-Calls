#include "server.h"

grpc::Status secure_voice_call::Server::SayHello(grpc::ServerContext *context, const helloworld::HelloRequest *request, helloworld::HelloReply *response)
{
    std::cout << "SERVER__ request->name(): " << request->name() << std::endl;
    response->set_message("hello world!");
    std::cout << "SERVER__ set response->message() " << response->message() << std::endl;
    return Status::OK;
}
