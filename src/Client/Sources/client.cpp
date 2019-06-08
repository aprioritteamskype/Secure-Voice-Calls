#include "client.h"
#include <thread>
#include <iostream> //FIXME extra include
#include "qmlclientstate.h"
#include "utils.h"

secure_voice_call::Client::Client(secure_voice_call::QMLClientsOnlineModel *model,
                                  secure_voice_call::QMLMissedCallsModel *missedCallsModel,
                                  int p2pClientSidePort,
                                  int p2pServerSidePort,
                                  const std::string& serverAddress)
    : QObject (nullptr),
      mServerAddress(serverAddress),
      mP2PClientSidePort(p2pClientSidePort),
      mModel(model),
      mPeerToPeer(missedCallsModel, p2pServerSidePort)
{
    mClientsOnlineRequest.set_requesttype(secure_voice_call::TypeMessage::GetClientsOnline);
    mGetIpByNameRequest.set_requesttype(secure_voice_call::TypeMessage::GetIpByUserName);

    grpc::ChannelArguments chArgs;
    chArgs.SetInt(GRPC_ARG_KEEPALIVE_TIME_MS, SVC_CLIENTSERVER_KEEPALIVE_TIME_MS);
    chArgs.SetInt(GRPC_ARG_KEEPALIVE_TIMEOUT_MS, SVC_CLIENTSERVERKEEPALIVE_TIMEOUT_MS);
    std::shared_ptr<Channel> channel = grpc::CreateCustomChannel(
                mServerAddress,
                grpc::InsecureChannelCredentials(),
                chArgs);
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

    if (!mstream->Write(request) || !mstream->Read(&response)) {
        mHasConnection = false;
        mstream->WritesDone();
        status =  mstream->Finish();
        std::cout << status.error_message() << std::endl;
        QMLClientState::getInstance().setStatus("Connection with server failed");
        return;
    }

    if (response.issuccessful()
            && response.responsetype() == secure_voice_call::TypeMessage::Authorization){
        mHasConnection = true;
        addClientToModel(response);
        using secure_voice_call::QMLClientState;
        QMLClientState::getInstance().setState(QMLClientState::ClientStates::Online);
        QMLClientState::getInstance().setAuthorizatedAs(name);
    } else {
        mHasConnection = false;
        mstream->WritesDone();
        status =  mstream->Finish();
        QMLClientState::getInstance().setStatus("Bad authorization: use another login");
    }
    if(!status.ok())
        std::cout << "row 63 client.cpp bad authorization" << std::endl;
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

void secure_voice_call::Client::declineCall()
{
    mPeerToPeer.declineCall();
}

void secure_voice_call::Client::finishPeerToPeerOutgoingCall()
{
    emit mPeerToPeer.finishPeerToPeerOutgoingCall(PeerToPeer::OutgoingCallStates::FinishedByCancel);
}

void secure_voice_call::Client::finishPeerToPeerIncomingCall(bool success)
{
    emit mPeerToPeer.finishPeerToPeerIncomingCall(success);
}

void secure_voice_call::Client::exit()
{
    if(mHasConnection){
        mstream->WritesDone();
        mstream->Finish();
        mHasConnection = false;
    }
    using secure_voice_call::QMLClientState;
    QMLClientState::getInstance().setState(QMLClientState::ClientStates::Authorization);
    QMLClientState::getInstance().setStatus("Signed out");
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
        QMLClientState::getInstance().setStatus("Connection with server failed");
        return;
    }
    if (!mstream->Read(&response)){
        mHasConnection = false;
        mstream->WritesDone();
        mstream->Finish();
        QMLClientState::getInstance().setState(QMLClientState::ClientStates::Authorization);
        QMLClientState::getInstance().setStatus("Connection with server failed");
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
    secure_voice_call::changePort(qstrUserip, mP2PClientSidePort);
    std::thread callThread([this, qstrUserip, username](){
        try
        {
        mPeerToPeer.sendCallRequest(qstrUserip.toStdString(), username.toStdString());
        }
        catch (const std::exception& ex)
        {
            std::cout << "error " << ex.what() << std::endl;
        }
    });
    callThread.detach();
}
