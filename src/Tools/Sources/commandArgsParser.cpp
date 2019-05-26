#include "commandArgsParser.h"
#include <cstdlib>
#include <stdexcept>
#include <QString>
#include "string.h"
#include "utils.h"

secure_voice_call::CommandArgsParser::CommandArgsParser(int argc, char *argv[])
{
    mServerHost = "0.0.0.0";
    mServerPort = 5000;
    mPeerToPeerClientSidePort = 5001;
    mPeerToPeerServerSidePort = 5001;

    if (argc > 1){
        int index = 1;
        while (argc-1 > 0){
            if (parseServerIp(argv[index], argv[index + 1])){}
            else if (parseServerPort(argv[index], argv[index + 1])){}
            else if (parsePeerToPeerClientSidePort(argv[index], argv[index + 1])){}
            else if (parsePeerToPeerServerSidePort(argv[index], argv[index + 1])){}
            else { throw(std::logic_error("incorrect arguments of command line")); }

            index += 2;
            argc -= 2;
        }
    }
}

int secure_voice_call::CommandArgsParser::peerToPeerClientSidePort() const
{
    return mPeerToPeerClientSidePort;
}

int secure_voice_call::CommandArgsParser::peerToPeerServerSidePort() const
{
    return mPeerToPeerServerSidePort;
}

int secure_voice_call::CommandArgsParser::serverPort() const
{
    return mServerPort;
}

std::string secure_voice_call::CommandArgsParser::serverHost() const
{
    return mServerHost;
}

std::string secure_voice_call::CommandArgsParser::serverIp() const
{
    return mServerHost + ":" + std::to_string(mServerPort);
}

bool secure_voice_call::CommandArgsParser::parseServerIp(const char *argument, const char *argument2)
{
    if (strcmp(argument, "-server_ip") != 0){
        return false;
    }
    QString ipWithStubedPort = QString("%1:%2")
            .arg(argument2)
            .arg(5000);
    if(!secure_voice_call::isValidIp(ipWithStubedPort)){
        throw(std::logic_error("incorrect server ip in command args"));
    }
    mServerHost = argument2;
    return true;
}

bool secure_voice_call::CommandArgsParser::parseServerPort(const char *argument, const char *argument2)
{
    if (strcmp(argument, "-server_port") != 0){
        return false;
    }
    char * pEnd;
    int serverPort;

    serverPort = static_cast<int>(strtol(argument2, &pEnd, 10));
    if (serverPort == 0){
        throw(std::logic_error("incorrect server port in command args"));
    }
    mServerPort = serverPort;
    return true;
}

bool secure_voice_call::CommandArgsParser::parsePeerToPeerServerSidePort(const char *argument, const char *argument2)
{
    if (strcmp(argument, "-p2p_serverside_port") != 0){
        return false;
    }
    char * pEnd;
    int peerToPeerServerSidePort;

    peerToPeerServerSidePort = static_cast<int>(strtol(argument2, &pEnd, 10));
    if (peerToPeerServerSidePort == 0){
        throw(std::logic_error("incorrect p2p_serverside port in command args"));
    }
    mPeerToPeerServerSidePort = peerToPeerServerSidePort;
    return true;
}

bool secure_voice_call::CommandArgsParser::parsePeerToPeerClientSidePort(const char *argument, const char *argument2)
{
    if (strcmp(argument, "-p2p_clientside_port") != 0){
        return false;
    }
    char * pEnd;
    int peerToPeerClientSidePort;

    peerToPeerClientSidePort = static_cast<int>(strtol(argument2, &pEnd, 10));
    if (peerToPeerClientSidePort == 0){
        throw(std::logic_error("incorrect p2p_clientside port in command args"));
    }
    mPeerToPeerClientSidePort = peerToPeerClientSidePort;
    return true;
}
