#pragma once
#include <memory>
#include <thread>
#include <atomic>
#include <QObject>
#include <grpcpp/grpcpp.h>
#include "client-server.grpc.pb.h"
#include "qmlclientstate.h"
#include "audiomodule.h"
#include "qmlmissedcallsmodel.h"

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

#define SVC_PEERTOPEER_KEEPALIVE_TIME_MS 2*1000
#define SVC_PEERTOPEER_KEEPALIVE_TIMEOUT_MS 5*1000

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
        PeerToPeer(secure_voice_call::QMLMissedCallsModel *missedCalls,
                   int p2pServerSidePort = 5001);

        void declineCall();
        void sendCallRequest(std::string ip, std::string callername);
    public:
        grpc::Status HandShake(grpc::ServerContext *context, ::grpc::ServerReaderWriter<CallResponse, CallRequest> *stream) override;
    private:
        void runServer();
        void clientReadVoiceThread(grpc::ClientContext& context);
        void clientWriteVoiceThread(grpc::ClientContext& context);
        void serverReadVoiceThread(ServerReaderWriter<secure_voice_call::CallResponse, secure_voice_call::CallRequest> *stream,
                                   grpc::ServerContext& context);
        void serverWriteVoiceThread(ServerReaderWriter<secure_voice_call::CallResponse, secure_voice_call::CallRequest> *stream,
                                    grpc::ServerContext& context);
        bool raceOutgoingCall(CallResponse& response, ClientContext& context);
        bool raceIncomingCall(ServerContext& context);
    signals:
        void finishPeerToPeerOutgoingCall(OutgoingCallStates st);
        void finishPeerToPeerIncomingCall(bool);
    private:
        std::atomic_bool mIsInConversation{false};
        std::atomic_bool mIsCanceledStream{false};
        std::atomic_int mIncomingCallTimersCount{0};
        std::atomic_int mOutgoingCallTimersCount{0};
        std::string mClientServerSideAddress;
        std::string mCallerName;
        std::unique_ptr<ClientReaderWriter<CallRequest, CallResponse>> mClientStream;
        std::unique_ptr<CallGreeter::Stub> mstub;
        std::thread mServerThread;
        std::unique_ptr<AudioModule> mAudioModule;
        secure_voice_call::QMLMissedCallsModel* mMissedCalls;
    };
}
