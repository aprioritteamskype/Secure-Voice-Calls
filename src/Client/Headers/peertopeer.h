#pragma once
#include <memory>
#include <thread>
#include <QObject>
#include <grpcpp/grpcpp.h>
#include "client-server.grpc.pb.h"
#include "qmlclientstate.h"
#include "audiomodule.h"
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
    class PeerToPeer final :public CallGreeter::Service
    {
    public:
        PeerToPeer();
        void runServer();

        void sendCallRequest(const std::string &ip, const std::string &callername);
        // Service interface
    public:
        grpc::Status HandShake(grpc::ServerContext *context, ::grpc::ServerReaderWriter<CallResponse, CallRequest> *stream) override;
    private:
        void clientReadVoice();
        void clientWriteVoice();
        void serverReadVoice(ServerReaderWriter<secure_voice_call::CallResponse, secure_voice_call::CallRequest> *stream);
        void serverWriteVoice(ServerReaderWriter<secure_voice_call::CallResponse, secure_voice_call::CallRequest> *stream);
    private:
        bool mIsInConversation = false;
        std::string mClientServerSideAddress;
        std::string mCallerName;
        std::unique_ptr<ClientReaderWriter<CallRequest, CallResponse>> mClientStream;
        std::unique_ptr<CallGreeter::Stub> mstub;
        std::unique_ptr<ClientContext> mContext;
        std::thread mServerThread;
        QMLClientState* mClientState;
        std::unique_ptr<AudioModule> mAudioModule;
    };
}
