#ifndef CLIENT_H
#define CLIENT_H
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

namespace secure_voice_call {
class Client : public QObject {
public:
    Client(secure_voice_call::QMLClientsOnlineModel &model);// : mstub(Greeter::NewStub(channel)) {}

   grpc::Status sendAuthorizationRequest(const QString &name);

private:
    secure_voice_call::QMLClientsOnlineModel *mModel;
    std::string mServerAddress;
    std::unique_ptr<Greeter::Stub> mstub;
};
}

#endif // CLIENT_H
