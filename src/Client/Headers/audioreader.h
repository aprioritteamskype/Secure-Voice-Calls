#pragma once
#include "player.h"
#include "client-server.grpc.pb.h"
namespace secure_voice_call {
    class AudioReader{
    public:
        AudioReader(const quint64 packetSize = 16);
        void readAudio(CallRequest &request,std::shared_ptr<Player> player);
        void readAudio(CallResponse &response,std::shared_ptr<Player> player);
    private:
        const quint64 mPacketSize;
    };
}
