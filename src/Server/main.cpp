#include <QCoreApplication>
#include <iostream>
#include <thread>
#include "server.h"

void runServer();

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    std::thread serverThread(runServer);
    serverThread.join();

    return a.exec();
}

void runServer()
{
    std::string address("0.0.0.0:5000");
    secure_voice_call::Server service;

    ServerBuilder builder;

    builder.AddListeningPort(address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on port: " << address << std::endl;

    server->Wait();
}
