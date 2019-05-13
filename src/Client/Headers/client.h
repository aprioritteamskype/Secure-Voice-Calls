#pragma once
#include <grpcpp/grpcpp.h>
#include <iostream>
#include "client-server.grpc.pb.h"
#include "qmlclientsonlinemodel.h"

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
    Client(secure_voice_call::QMLClientsOnlineModel &model);// : mstub(Greeter::NewStub(channel)) {}

   grpc::Status sendAuthorizationRequest(const QString &name);
   void addClientToModel(const AuthorizationResponse &response) const;
   void sendClientsOnlineRequest();
   void sendIdByUserNameRequest(const QString &username);
private:
    secure_voice_call::QMLClientsOnlineModel *mModel;
    std::string mServerAddress;
    std::unique_ptr<Greeter::Stub> mstub;
    std::unique_ptr<ClientReaderWriter<AuthorizationRequest, AuthorizationResponse>> mstream;
    AuthorizationRequest mClientsOnlineRequest; //clientsonline request
    AuthorizationRequest mGetIpByNameRequest;
    std::string mname;
    std::unique_ptr<ClientContext> mContext;
    bool mHasConnection = false;
};
}
