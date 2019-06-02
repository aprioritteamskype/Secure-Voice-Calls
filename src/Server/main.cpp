#include <QCoreApplication>
#include <iostream>
#include <thread>
#include "server.h"
#include "commandArgsParser.h"

void runServer(int port = 5000);

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    secure_voice_call::CommandArgsParser parser(argc,argv);

    std::thread serverThread(runServer, parser.serverPort());
    serverThread.join();

    return a.exec();
}

void runServer(int port)
{
    std::string address("0.0.0.0:" + std::to_string(port));
    secure_voice_call::Server service;

    ServerBuilder builder;

    builder.AddListeningPort(address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    builder.AddChannelArgument(GRPC_ARG_KEEPALIVE_TIME_MS, 20000);
    builder.AddChannelArgument(GRPC_ARG_KEEPALIVE_TIMEOUT_MS, 10000);

    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on port: " << address << std::endl;

    server->Wait();
}
