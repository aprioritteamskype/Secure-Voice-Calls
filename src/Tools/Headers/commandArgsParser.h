#pragma once
#include <string>

//format with default values:
//-p2p_clientside_port 5001
//-p2p_serverside_port 5001
//-server_port 5000
//-server_ip 0.0.0.0

namespace secure_voice_call {
    class CommandArgsParser
    {
    public:
        CommandArgsParser(int argc, char *argv[]);
        int peerToPeerClientSidePort() const;
        int peerToPeerServerSidePort() const;
        int serverPort() const;
        std::string serverHost() const;
        std::string serverIp() const;

    private:
        std::string mServerHost;
        int mServerPort;
        int mPeerToPeerServerSidePort;
        int mPeerToPeerClientSidePort;

        bool parseServerIp(const char* argument, const char* argument2);
        bool parseServerPort(const char* argument, const char* argument2);
        bool parsePeerToPeerServerSidePort(const char* argument, const char* argument2);
        bool parsePeerToPeerClientSidePort(const char* argument, const char* argument2);
    };
}
