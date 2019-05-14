#pragma once
#include "client-server.grpc.pb.h"
#include <QObject>
#include <grpcpp/grpcpp.h>
#include <memory>

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
class PeerToPeer final : public CallGreeter::Service, public QObject
{
public:
    PeerToPeer();
    void runServer();

    Q_INVOKABLE void sendCallRequest(const std::string &ip, const std::string &callername);
    // Service interface
public:
    grpc::Status HandShake(grpc::ServerContext *context, ::grpc::ServerReaderWriter<CallResponse, CallRequest> *stream) override;
private:
    std::string mServerAddress;
    std::string mCallerName;
    std::unique_ptr<ClientReaderWriter<CallRequest, CallResponse>> mClientStream;
    std::unique_ptr<CallGreeter::Stub> mstub;
    std::unique_ptr<ClientContext> mContext;
    void clientReadVoice();
    void clientWriteVoice();
    void serverReadVoice(ServerReaderWriter<secure_voice_call::CallResponse, secure_voice_call::CallRequest> *stream);
    void serverWriteVoice(ServerReaderWriter<secure_voice_call::CallResponse, secure_voice_call::CallRequest> *stream);
};
}
