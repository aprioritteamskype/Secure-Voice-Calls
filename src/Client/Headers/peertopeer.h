#pragma once
#include <memory>
#include <thread>
#include <atomic>
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
    class PeerToPeer final : public QObject, public CallGreeter::Service
    {
        Q_OBJECT
    public:
        enum OutgoingCallStates{
            NotFinished,
            FinishedByReading,
            FinishedByTimer,
            FinishedByCancel
        };
        Q_ENUM(OutgoingCallStates)
    public:
        PeerToPeer(int p2pServerSidePort = 5001);

        void declineCall();
        void sendCallRequest(std::string ip, std::string callername);
        // Service interface
    public:
        grpc::Status HandShake(grpc::ServerContext *context, ::grpc::ServerReaderWriter<CallResponse, CallRequest> *stream) override;
    private:
        void runServer();
        void clientReadVoice();
        void clientWriteVoice();
        void serverReadVoice(ServerReaderWriter<secure_voice_call::CallResponse, secure_voice_call::CallRequest> *stream);
        void serverWriteVoice(ServerReaderWriter<secure_voice_call::CallResponse, secure_voice_call::CallRequest> *stream);
        bool raceOutgoingCall(CallResponse& response, ClientContext& context);
        bool raceIncomingCall(ServerContext& context);
    signals:
        void finishPeerToPeerOutgoingCall(OutgoingCallStates st);
        void finishPeerToPeerIncomingCall(bool);
    private:
        std::atomic<bool> mIsInConversation{false};
        std::string mClientServerSideAddress;
        std::string mCallerName;
        std::unique_ptr<ClientReaderWriter<CallRequest, CallResponse>> mClientStream;
        std::unique_ptr<CallGreeter::Stub> mstub;
        std::thread mServerThread;
        QMLClientState* mClientState;
        std::unique_ptr<AudioModule> mAudioModule;
    };
}
