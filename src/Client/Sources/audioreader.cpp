#include  "audioreader.h"

secure_voice_call::AudioReader::AudioReader(const quint64 packetSize):mPacketSize(packetSize)
{
}

void secure_voice_call::AudioReader::readAudio(secure_voice_call::CallRequest &request, std::shared_ptr<Player> player)
{
    player->pushDataToBuff(request.audiobytes().c_str(),mPacketSize);
}

void secure_voice_call::AudioReader::readAudio(secure_voice_call::CallResponse &response, std::shared_ptr<Player> player)
{
    player->pushDataToBuff(response.audiobytes().c_str(),mPacketSize);
}
