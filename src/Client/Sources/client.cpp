#include "client.h"
#include "qmlclientstate.h"
#include <iostream>

secure_voice_call::Client::Client(secure_voice_call::QMLClientsOnlineModel &model)
    : QObject (nullptr),
      mModel(&model),
        mServerAddress("0.0.0.0:5000"){
    using secure_voice_call::QMLClientState;
    std::shared_ptr<Channel> channel = grpc::CreateChannel(
                mServerAddress,
                grpc::InsecureChannelCredentials()
                );
    mstub = Greeter::NewStub(channel);

    connect(&QMLClientState::getInstance(), &QMLClientState::tryAuthorizate,
            this, &Client::sendAuthorizationRequest);
}

grpc::Status secure_voice_call::Client::sendAuthorizationRequest(const QString &name)
{
    AuthorizationRequest request;
    request.set_name(name.toStdString());

    ClientContext context;
    AuthorizationResponse response;
    Status status = mstub->Authorization(&context, request, &response);

    if (status.ok()) {
        if (response.issuccessful()){
            std::vector<std::string> clients;
            for(int i = 0, size = response.user_names_size(); i < size; ++i){
                clients.push_back(response.user_names(i));
            }
            mModel->setCLients(clients);
            using secure_voice_call::QMLClientState;
            QMLClientState::getInstance().setState(QMLClientState::ClientStates::Online);
        }
    }

    return status;
}
