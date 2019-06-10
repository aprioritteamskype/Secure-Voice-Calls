#pragma once
#include <grpcpp/grpcpp.h>
#include <iostream>
#include "client-server.grpc.pb.h"
#include "qmlclientsonlinemodel.h"
#include "qmlmissedcallsmodel.h"
#include "peertopeer.h"

using secure_voice_call::Greeter;
using secure_voice_call::AuthorizationRequest;
using secure_voice_call::AuthorizationResponse;

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using grpc::ClientReaderWriter;

#define SVC_CLIENTSERVER_KEEPALIVE_TIME_MS 10*1000
#define SVC_CLIENTSERVERKEEPALIVE_TIMEOUT_MS 5*1000

namespace secure_voice_call {
    class Client : public QObject {
        Q_OBJECT
    public:
        Client(secure_voice_call::QMLClientsOnlineModel *model,
               secure_voice_call::QMLMissedCallsModel *missedCallsModel,
               int p2pClientSidePort = 5001,
               int p2pServerSidePort = 5001,
               const std::string& serverAddress = "0.0.0.0:5000");

        Q_INVOKABLE void sendAuthorizationRequest(const QString &name);
        Q_INVOKABLE void sendClientsOnlineRequest();
        Q_INVOKABLE void sendIdByUserNameRequest(const QString &username);
        Q_INVOKABLE void declineCall();
        Q_INVOKABLE void finishPeerToPeerOutgoingCall();
        Q_INVOKABLE void finishPeerToPeerIncomingCall(bool success);
        Q_INVOKABLE void exit();
        void addClientToModel(const AuthorizationResponse &response) const;
    private:
        std::string mServerAddress;
        std::string mname;
        bool mHasConnection = false;
        int mP2PClientSidePort;
        std::unique_ptr<Greeter::Stub> mstub;
        std::unique_ptr<ClientReaderWriter<AuthorizationRequest, AuthorizationResponse>> mstream;
        std::unique_ptr<ClientContext> mContext;
        AuthorizationRequest mClientsOnlineRequest;
        AuthorizationRequest mGetIpByNameRequest;
        secure_voice_call::QMLClientsOnlineModel *mModel;
        secure_voice_call::PeerToPeer mPeerToPeer;
    };
}
