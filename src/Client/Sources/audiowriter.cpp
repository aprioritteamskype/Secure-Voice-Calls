#include  "audiowriter.h"

secure_voice_call::AudioWriter::AudioWriter(const quint64 packetSize):mPacketSize(packetSize)
{
    mPacket.resize(mPacketSize);
}

bool secure_voice_call::AudioWriter::writeAudio(secure_voice_call::CallRequest &request,std::shared_ptr<Recorder> recorder)
{
    if(recorder->getDataFromBuff(mPacket.data(),mPacketSize)){
        request.set_audiobytes(mPacket.data(),mPacketSize);
        request.set_packetsize(mPacketSize);
        return true;
    }
    return false;
}

bool secure_voice_call::AudioWriter::writeAudio(secure_voice_call::CallResponse &response,std::shared_ptr<Recorder> recorder)
{
    if(recorder->getDataFromBuff(mPacket.data(),mPacketSize)){
        response.set_audiobytes(mPacket.data(),mPacketSize);
        response.set_packetsize(mPacketSize);
        return true;
    }
    return false;
}



