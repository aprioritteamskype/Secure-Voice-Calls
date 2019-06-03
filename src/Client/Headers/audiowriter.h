#pragma once
#include "recorder.h"
#include "client-server.grpc.pb.h"
namespace secure_voice_call {
    class AudioWriter{
    public:
        AudioWriter(const quint64 packetSize = 16);
        bool writeAudio(CallRequest &request,std::shared_ptr<Recorder> recorder);
        bool writeAudio(CallResponse &response,std::shared_ptr<Recorder> recorder);
    private:
        QByteArray mPacket;
        const quint64 mPacketSize;
    };
}
