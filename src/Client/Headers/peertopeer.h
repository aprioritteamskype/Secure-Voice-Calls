#pragma once
#include "client-server.grpc.pb.h"
#include <QObject>
#include <grpcpp/grpcpp.h>
#include <memory>
#include <QThread>
#include "../Tools/recorder/recorder.h" //FIXME
#include "../Tools/player/player.h" //FIXME

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using grpc::ServerReaderWriter;

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using grpc::ClientReaderWriter;

using secure_voice_call::CallGreeter;
using secure_voice_call::CallRequest;
using secure_voice_call::CallResponse;

namespace secure_voice_call {
class PeerToPeer final : public QObject,public CallGreeter::Service
{
    Q_OBJECT
public:
    PeerToPeer();
    void runServer();

    Q_INVOKABLE void sendCallRequest(const std::string &ip, const std::string &callername);
signals:
    void startRecordingAudio();
    void stopRecordingAudio();
    void startPlayingAudio();
    void stopPlayingAudio();
    // Service interface
public:
    grpc::Status HandShake(grpc::ServerContext *context, ::grpc::ServerReaderWriter<CallResponse, CallRequest> *stream) override;
private:
    std::string mServerAddress;
    std::string mCallerName;
    std::unique_ptr<ClientReaderWriter<CallRequest, CallResponse>> mClientStream;
    std::unique_ptr<CallGreeter::Stub> mstub;
    std::unique_ptr<ClientContext> mContext;

    std::unique_ptr<Recorder> mRecorder;
    QThread *recThread;
    quint64 mBufSendPos = 0;

    std::unique_ptr<Player> mPlayer;
    QThread *playThread;

    void clientReadVoice();
    void clientWriteVoice();
    void serverReadVoice(ServerReaderWriter<secure_voice_call::CallResponse, secure_voice_call::CallRequest> *stream);
    void serverWriteVoice(ServerReaderWriter<secure_voice_call::CallResponse, secure_voice_call::CallRequest> *stream);
};
}
