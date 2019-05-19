#include "client.h"
#include <thread>
#include "qmlclientstate.h"
#include "utils.h"
#include <iostream> //FIXME extra include

secure_voice_call::Client::Client(secure_voice_call::QMLClientsOnlineModel &model)
    : QObject (nullptr),
      mServerAddress("0.0.0.0:5000"),
      mModel(&model)
{
    mClientsOnlineRequest.set_requesttype(secure_voice_call::TypeMessage::GetClientsOnline);
    mGetIpByNameRequest.set_requesttype(secure_voice_call::TypeMessage::GetIpByUserName);

    std::shared_ptr<Channel> channel = grpc::CreateChannel(
                mServerAddress,
                grpc::InsecureChannelCredentials()
                );
    mstub = Greeter::NewStub(channel);
}

void secure_voice_call::Client::sendAuthorizationRequest(const QString &name)
{
    Status status;
    AuthorizationRequest request;
    request.set_requesttype(secure_voice_call::TypeMessage::Authorization);
    mname = name.toStdString();
    request.set_name(mname);
    std::unique_ptr<ClientContext> tmpPtr(new ClientContext);
    std::swap(mContext, tmpPtr); //you cannot reuse this object between calls

    AuthorizationResponse response;
    mstream = mstub->Authorization(mContext.get());

    if (!mstream->Write(request)) {
        mHasConnection = false;
        mstream->WritesDone();
        status =  mstream->Finish();
    }

    if (!mstream->Read(&response)) {
        mstream->WritesDone();
        mHasConnection = false;
        status = mstream->Finish();
    }

        if (response.issuccessful()
                && response.responsetype() == secure_voice_call::TypeMessage::Authorization){
            mHasConnection = true;
            addClientToModel(response);
            using secure_voice_call::QMLClientState;
            QMLClientState::getInstance().setState(QMLClientState::ClientStates::Online);
        }
        if(!status.ok())
        std::cout << "63 client.cpp status not ok" << std::endl;
}

void secure_voice_call::Client::addClientToModel(const secure_voice_call::AuthorizationResponse &response) const
{
    std::vector<std::string> clients;
    for(int i = 0, size = response.usernames_size(); i < size; ++i){
        if(response.usernames(i) != mname){
            clients.push_back(response.usernames(i));
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
    std::cout << "setState(QMLClientState::ClientStates::OutgoingCall) " << std::endl;
    QMLClientState::getInstance().setState(QMLClientState::ClientStates::OutgoingCall);
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
    QString qstrUserip = QString::fromStdString(response.userip());
    secure_voice_call::changePort(qstrUserip, 5002);
    std::thread callThread([this, &qstrUserip, &username](){
        mPeerToPeer.sendCallRequest(qstrUserip.toStdString(), username.toStdString());
    });
    callThread.detach();
}
