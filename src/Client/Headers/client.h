#pragma once
#include <grpcpp/grpcpp.h>
#include <iostream>
#include "client-server.grpc.pb.h"
#include "qmlclientsonlinemodel.h"
#include "peertopeer.h"

using secure_voice_call::Greeter;
using secure_voice_call::AuthorizationRequest;
using secure_voice_call::AuthorizationResponse;

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using grpc::ClientReaderWriter;

namespace secure_voice_call {
    class Client : public QObject {
        Q_OBJECT
    public:
        Client(secure_voice_call::QMLClientsOnlineModel &model);

        Q_INVOKABLE void sendAuthorizationRequest(const QString &name);
        Q_INVOKABLE void sendClientsOnlineRequest();
        Q_INVOKABLE void sendIdByUserNameRequest(const QString &username);
        Q_INVOKABLE void declineCall();
        void addClientToModel(const AuthorizationResponse &response) const;
    private:
    private:
        std::string mServerAddress;
        std::string mname;
        bool mHasConnection = false;
        std::unique_ptr<Greeter::Stub> mstub;
        std::unique_ptr<ClientReaderWriter<AuthorizationRequest, AuthorizationResponse>> mstream;
        std::unique_ptr<ClientContext> mContext;
        AuthorizationRequest mClientsOnlineRequest;
        AuthorizationRequest mGetIpByNameRequest;
        secure_voice_call::QMLClientsOnlineModel *mModel;
        secure_voice_call::PeerToPeer mPeerToPeer;
    };
}
