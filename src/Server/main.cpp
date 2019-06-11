#include <QCoreApplication>
#include <iostream>
#include <thread>
#include <QDir>
#include <plog/Log.h>
#include <plog/Appenders/ConsoleAppender.h>

#include "server.h"
#include "commandArgsParser.h"

#define LOG_DIR_PASS  "Logs"
#define LOG_PASS  "Logs/ServerLog.txt"
#define SVC_KEEPALIVE_TIME_MS 10*1000
#define SVC_KEEPALIVE_TIMEOUT_MS 5*1000
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

    if(!QDir().exists(LOG_DIR_PASS)){
        QDir().mkdir(LOG_DIR_PASS);
    }
    static plog::ConsoleAppender<plog::TxtFormatter> consoleAppender;
    plog::init(plog::info,LOG_PASS).addAppender(&consoleAppender);

    builder.AddListeningPort(address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    builder.AddChannelArgument(GRPC_ARG_KEEPALIVE_TIME_MS, SVC_KEEPALIVE_TIME_MS);
    builder.AddChannelArgument(GRPC_ARG_KEEPALIVE_TIMEOUT_MS, SVC_KEEPALIVE_TIMEOUT_MS);

    std::unique_ptr<Server> server(builder.BuildAndStart());
    LOG(plog::info) << "[Server]Server listening on port: " << address;

    server->Wait();
}
