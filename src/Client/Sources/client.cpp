#include "client.h"
#include "qmlclientstate.h"
#include <iostream> //FIXME extra include

secure_voice_call::Client::Client(secure_voice_call::QMLClientsOnlineModel &model)
    : QObject (nullptr),
      mModel(&model),
      mServerAddress("0.0.0.0:5000"){
    mClientsOnlineRequest.set_requesttype(secure_voice_call::TypeMessage::GetClientsOnline);
    mGetIpByNameRequest.set_requesttype(secure_voice_call::TypeMessage::GetIpByUserName);
    std::shared_ptr<Channel> channel = grpc::CreateChannel(
                mServerAddress,
                grpc::InsecureChannelCredentials()
                );
    mstub = Greeter::NewStub(channel);

    using secure_voice_call::QMLClientState;
    connect(&QMLClientState::getInstance(), &QMLClientState::tryAuthorizate,
            this, &Client::sendAuthorizationRequest);
    connect(&QMLClientState::getInstance(), &QMLClientState::refreshClientList,
            this, &Client::sendClientsOnlineRequest);
    connect(&QMLClientState::getInstance(), &QMLClientState::getUserIdByName,
            this, &Client::sendIdByUserNameRequest);
}

grpc::Status secure_voice_call::Client::sendAuthorizationRequest(const QString &name)
{
    Status status;
    AuthorizationRequest request;
    request.set_requesttype(secure_voice_call::TypeMessage::Authorization);
    mname = name.toStdString();
    request.set_name(mname);
    std::unique_ptr<ClientContext> tmpPtr(new ClientContext);
    mContext = std::move(tmpPtr);

    AuthorizationResponse response;
    mstream = mstub->Authorization(mContext.get());

    if (!mstream->Write(request)) {
        mHasConnection = false;
        mstream->WritesDone();
        return  mstream->Finish();
    }

    if (!mstream->Read(&response)) {
        mstream->WritesDone();
        mHasConnection = false;
        return mstream->Finish();
    }

        if (response.issuccessful()
                && response.responsetype() == secure_voice_call::TypeMessage::Authorization){
            mHasConnection = true;
            addClientToModel(response);
            using secure_voice_call::QMLClientState;
            QMLClientState::getInstance().setState(QMLClientState::ClientStates::Online);
        }
        return status;
}

void secure_voice_call::Client::addClientToModel(const secure_voice_call::AuthorizationResponse &response) const
{
    std::vector<std::string> clients;
    for(int i = 0, size = response.user_names_size(); i < size; ++i){
        if(response.user_names(i) != mname){
            clients.push_back(response.user_names(i));
        }
    }
    mModel->setCLients(clients);
}

void secure_voice_call::Client::sendClientsOnlineRequest()
{
    if(!mHasConnection)
    {
        QMLClientState::getInstance().setState(QMLClientState::ClientStates::Authorization);
        return;
    }
    AuthorizationResponse response;

    if (!mstream->Write(mClientsOnlineRequest)) {
        mHasConnection = false;
        mstream->WritesDone();
        mstream->Finish();
        QMLClientState::getInstance().setState(QMLClientState::ClientStates::Authorization);
        return;
    }
    if (!mstream->Read(&response)){
        mHasConnection = false;
        mstream->WritesDone();
        mstream->Finish();
        QMLClientState::getInstance().setState(QMLClientState::ClientStates::Authorization);
        return;
    }
    if(response.responsetype() == secure_voice_call::TypeMessage::GetClientsOnline){
        addClientToModel(response);
    } else {
        QMLClientState::getInstance().setState(QMLClientState::ClientStates::Authorization);
    }
}

void secure_voice_call::Client::sendIdByUserNameRequest(const QString &username)
{
    //FIXME extra code
    std::cout << "void secure_voice_call::Client::sendIdByUserNameRequest(const QString &username) "
              << username.toStdString() <<  std::endl;
    if(!mHasConnection)
    {
        QMLClientState::getInstance().setState(QMLClientState::ClientStates::Authorization);
        return;
    }

    AuthorizationResponse response;
    mGetIpByNameRequest.set_getipbyusername(username.toStdString());
    if (!mstream->Write(mGetIpByNameRequest)) {
        mHasConnection = false;
        mstream->WritesDone();
        mstream->Finish();
        QMLClientState::getInstance().setState(QMLClientState::ClientStates::Authorization);
        return;
    }
    if (!mstream->Read(&response)){
        mHasConnection = false;
        mstream->WritesDone();
        mstream->Finish();
        QMLClientState::getInstance().setState(QMLClientState::ClientStates::Authorization);
        return;
    }
    std::cout << response.userip() << std::endl;
}

