#include "peertopeer.h"
#include <iostream> //FIXME
#include <memory>
#include <chrono>
#include <QEventLoop>
#include <QTimer>
#include <audiomodule.h>

secure_voice_call::PeerToPeer::PeerToPeer(secure_voice_call::QMLMissedCallsModel *missedCalls,
                                          int p2pServerSidePort)
    : QObject (nullptr),
      mClientServerSideAddress("0.0.0.0:" + std::to_string(p2pServerSidePort)),
      mMissedCalls(missedCalls)
{

    mAudioModule.reset(new AudioModule());
    mServerThread =  std::thread([this](){
        runServer();
    });
}

void secure_voice_call::PeerToPeer::declineCall()
{
    mIsInConversation = false;
}

void secure_voice_call::PeerToPeer::runServer()
{
    ServerBuilder builder;

    builder.AddListeningPort(mClientServerSideAddress, grpc::InsecureServerCredentials()); //FIXME
    builder.RegisterService(this);
    builder.AddChannelArgument(GRPC_ARG_KEEPALIVE_TIME_MS, 2000);
    builder.AddChannelArgument(GRPC_ARG_KEEPALIVE_TIMEOUT_MS, 5000);


    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "ClientSide-Server listening on port: " << mClientServerSideAddress << std::endl;

    server->Wait();
}

void secure_voice_call::PeerToPeer::sendCallRequest(std::string ip, std::string callername) //new thread
{
    std::cout << "try to call callcaller: " << callername << " " << ip << std::endl;
    QMLClientState::getInstance().setCallerName(QString::fromStdString(callername));

    grpc::ChannelArguments chArgs;
    chArgs.SetInt(GRPC_ARG_KEEPALIVE_TIME_MS, SVC_PEERTOPEER_KEEPALIVE_TIME_MS);
    chArgs.SetInt(GRPC_ARG_KEEPALIVE_TIMEOUT_MS, SVC_PEERTOPEER_KEEPALIVE_TIMEOUT_MS);
    std::shared_ptr<Channel> channel = grpc::CreateCustomChannel(
                ip,
                grpc::InsecureChannelCredentials(),
                chArgs
                );

    mstub = CallGreeter::NewStub(channel);
    CallRequest request;
    CallResponse response;

    request.set_callername(QMLClientState::getInstance().authorizatedAs().toStdString()); //send your authorizated user name
    ClientContext context;
    mClientStream = mstub->HandShake(&context);
    mClientStream->Write(request);


    if(!raceOutgoingCall(response, context)){
        std::cout << "!raceOutgoingCall " << std::endl;
        QMLClientState::getInstance().setState(QMLClientState::ClientStates::Online);
        return;
    }

    if(response.issuccessful()){
        QMLClientState::getInstance().setState(QMLClientState::ClientStates::InConversation);
        mIsInConversation = true;
        std::cout << "Client_ClientSide: response is successful" << std::endl;
        std::thread tRead([this, &context](){
            clientReadVoiceThread(context);
        });
        std::thread tWrite([this, &context](){
            clientWriteVoiceThread(context);
        });
        tRead.join();
        tWrite.join();
        mIsCanceledStream = false;
        mClientStream->Finish();
        std::cout << "Client_ClientSide HandShake thread joins has been reached" << std::endl;
    }else{
        mClientStream->WritesDone();
        mClientStream->Finish();
        QMLClientState::getInstance().setStatus("User have declined your call");
    }
    QMLClientState::getInstance().setState(QMLClientState::ClientStates::Online);
}



grpc::Status secure_voice_call::PeerToPeer::HandShake(grpc::ServerContext *context, ::grpc::ServerReaderWriter<secure_voice_call::CallResponse, secure_voice_call::CallRequest> *stream)
{
    std::cout<<"HANDSHAKE"<<std::endl;
    //check flag isInConversation
    CallRequest request;
    stream->Read(&request);
    CallResponse response;
    if (mIsInConversation) {
        response.set_issuccessful(false);
        std::cout << "missed call: " << request.callername()
                  << " "
                  << QTime::currentTime().toString().toStdString()
                  << std::endl;
        mMissedCalls->addMissedCall(request.callername(), QTime::currentTime());
        stream->Write(response);
        return Status::CANCELLED;
    }
    std::cout << "ServerSide: ClientStates::IncomingCall: " << std::endl;

    QMLClientState::getInstance().setState(QMLClientState::ClientStates::IncomingCall);
    QMLClientState::getInstance().setCallerName(QString::fromStdString(request.callername()));
    if(raceIncomingCall(*context)){
        response.set_issuccessful(true);
        stream->Write(response);
        mIsInConversation = true;
        std::cout << "ServerSide: QMLClientState::ClientStates::InConversation: " << std::endl;
        QMLClientState::getInstance().setState(QMLClientState::ClientStates::InConversation);
        std::thread tRead([this, stream, &context](){
            serverReadVoiceThread(stream, *context);
        });
        std::thread tWrite([this, stream, &context](){
            serverWriteVoiceThread(stream, *context);
        });
        tRead.join();
        tWrite.join();
        std::cout << "Client_ServerSide HandShake thread joins reached" << std::endl;
    }else {
        response.set_issuccessful(false);
        stream->Write(response);
    }
    mIsInConversation = false;
    QMLClientState::getInstance().setState(QMLClientState::ClientStates::Online);
    return Status::OK;
}

void secure_voice_call::PeerToPeer::clientReadVoiceThread(grpc::ClientContext& context)
{
    CallResponse response;
    mAudioModule->startPlayingAudio();
    std::cout<<"startPlayingAudio"<<std::endl;
    while (mIsInConversation
           && QMLClientState::getInstance().getState() == QMLClientState::ClientStates::InConversation
           && mClientStream->Read(&response))
    {
        mAudioModule->readVoice(response);
    }
    mAudioModule->stopPlayingAudio();
    if(!mIsCanceledStream){
        mIsCanceledStream = true;
        context.TryCancel();
    }
    mIsInConversation = false;
}

void secure_voice_call::PeerToPeer::clientWriteVoiceThread(grpc::ClientContext& context)
{
    mAudioModule->startRecordingAudio();
    std::cout<<"startRecordingAudio"<<std::endl;
    CallRequest request;
    while(mIsInConversation
          && QMLClientState::getInstance().getState() == QMLClientState::ClientStates::InConversation)
    {
        if(mAudioModule->writeVoice(request)){
            if(!mClientStream->Write(request)){
                std::cout<<"There is no connection"<<std::endl;
                mAudioModule->stopRecordingAudio();
                std::cout<<"stopRecordingAudio"<<std::endl;
                mIsInConversation = false;
                return;
            }
        }
    }
    mAudioModule->stopRecordingAudio();
    std::cout<<"stopRecordingAudio"<<std::endl;
    if(!mIsCanceledStream){
        mIsCanceledStream = true;
        context.TryCancel();
    }
    mIsInConversation = false;
}

void secure_voice_call::PeerToPeer::serverReadVoiceThread(ServerReaderWriter<CallResponse, CallRequest> *stream, grpc::ServerContext& context)
{
    CallRequest request;
    mAudioModule->startPlayingAudio();
    std::cout<<"startPlayingAudio"<<std::endl;
    while (mIsInConversation
           && QMLClientState::getInstance().getState() == QMLClientState::ClientStates::InConversation
           && stream->Read(&request))
    {
          mAudioModule->readVoice(request);
    }
    mAudioModule->stopPlayingAudio();
    std::cout<<"stopPlayingAudio"<<std::endl;
    if(!context.IsCancelled()){
        context.TryCancel();
    }
    mIsInConversation = false;
}

void secure_voice_call::PeerToPeer::serverWriteVoiceThread(ServerReaderWriter<CallResponse, CallRequest> *stream, grpc::ServerContext& context)
{
    CallResponse response;
    mAudioModule->startRecordingAudio();
    std::cout<<"startRecordingAudio"<<std::endl;
    while(mIsInConversation
          && QMLClientState::getInstance().getState() == QMLClientState::ClientStates::InConversation)
    {
        if(mAudioModule->writeVoice(response)){
            if(!stream->Write(response)){
                std::cout<<"There is no connection"<<std::endl;
                mAudioModule->stopRecordingAudio();
                std::cout<<"stopRecordingAudio"<<std::endl;
                mIsInConversation = false;
                return;
            }
        }
    }
    mAudioModule->stopRecordingAudio();
    std::cout<<"stopRecordingAudio"<<std::endl;
    if(!context.IsCancelled()){
        context.TryCancel();
    }
    mIsInConversation = false;
}

//what earlier, user on the other side take a call or user on this side cancel
bool secure_voice_call::PeerToPeer::raceOutgoingCall(secure_voice_call::CallResponse &response, ClientContext& context)
{
    bool success = false;
    std::thread readThread([this, &response](){
        mClientStream->Read(&response);
        emit finishPeerToPeerOutgoingCall(OutgoingCallStates::FinishedByReading);
    });
    //[timer]-----------------------------------------------------------------------------
    std::thread timerThread([this](){
        int numberOfTimer = ++mOutgoingCallTimersCount;
        std::this_thread::sleep_for(std::chrono::seconds(12));
        if (numberOfTimer != mOutgoingCallTimersCount) return;
        emit finishPeerToPeerOutgoingCall(OutgoingCallStates::FinishedByTimer);
        mOutgoingCallTimersCount = 0;
    });
    timerThread.detach();
    //[timer]-----------------------------------------------------------------------------
    QEventLoop loop;
    std::shared_ptr<QMetaObject::Connection> sh(new QMetaObject::Connection(),
                                           [this](QMetaObject::Connection *ptr){
        QObject::disconnect(*ptr);
        delete ptr;
    });

    OutgoingCallStates outgoingState = OutgoingCallStates::NotFinished;
    *sh = QObject::connect(this, &PeerToPeer::finishPeerToPeerOutgoingCall,
                           [&success, &loop, &outgoingState, &context](OutgoingCallStates st){
        if(outgoingState == OutgoingCallStates::NotFinished) {
            outgoingState = st;
            switch (outgoingState) {
                case OutgoingCallStates::FinishedByReading:
                {
                    std::cout << "OutgoingCallStates::FinishedByReading" << std::endl;
                    success = true;
                    break;
                }
                case OutgoingCallStates::FinishedByCancel:
                {
                    std::cout << "OutgoingCallStates::FinishedByCancel" << std::endl;
                    context.TryCancel();
                    break;
                }
                case OutgoingCallStates::FinishedByTimer:
                {
                    std::cout << "OutgoingCallStates::FinishedByTimer" << std::endl;
                    context.TryCancel();
                    break;
                }
                default:
                {
                    std::cout <<  "unhandled OutgoingCallStates" << std::endl;
                }
            }
            loop.exit();
        }
    });

    loop.exec();
    readThread.join();
    return success;
}

//what earlier, user on the other side cancels a call or user on this side accepts\declines
bool secure_voice_call::PeerToPeer::raceIncomingCall(ServerContext& context)
{
    bool success = false;
    std::atomic_bool isPeerToPeerIncomingCallFinished(false);
    //[timer]-----------------------------------------------------------------------------
    std::thread timerThread([this](){
        int numberOfTimer = ++mIncomingCallTimersCount;
        std::this_thread::sleep_for(std::chrono::seconds(12));
        if (numberOfTimer != mIncomingCallTimersCount) return;
        emit finishPeerToPeerIncomingCall(false);
        mIncomingCallTimersCount = 0;
    });
    timerThread.detach();
    //[timer]-----------------------------------------------------------------------------
    QEventLoop loop;
    //[ClientSideCanceled connection]-----------------------------------------------------
    std::thread checkCanceledThread([this, &context, &isPeerToPeerIncomingCallFinished](){
        while(!context.IsCancelled() && !isPeerToPeerIncomingCallFinished){
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
        finishPeerToPeerIncomingCall(false);
    });
    //[ClientSideCanceled connection]-----------------------------------------------------
    std::shared_ptr<QMetaObject::Connection> sh(new QMetaObject::Connection(),
                                           [this](QMetaObject::Connection *ptr){
        QObject::disconnect(*ptr);
        delete ptr;
    });

    *sh = QObject::connect(this, &PeerToPeer::finishPeerToPeerIncomingCall,
                           [&loop, &success, &isPeerToPeerIncomingCallFinished]
                           (bool isSuccessed){
        if (loop.isRunning()){
            success = isSuccessed;
            isPeerToPeerIncomingCallFinished = true;
            loop.exit();
        }
    }
    );
    loop.exec();
    checkCanceledThread.join();
    return success;
}
