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
    public:
        Client(secure_voice_call::QMLClientsOnlineModel &model);

       grpc::Status sendAuthorizationRequest(const QString &name);
       void addClientToModel(const AuthorizationResponse &response) const;
       void sendClientsOnlineRequest();
       void sendIdByUserNameRequest(const QString &username);
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
