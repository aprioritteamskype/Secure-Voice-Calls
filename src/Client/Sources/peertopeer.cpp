#include "peertopeer.h"
#include <iostream> //FIXME
#include <memory>
#include <chrono>
#include <QEventLoop>
#include <QTimer>
#include <audiomodule.h>
#include <plog/Log.h>

secure_voice_call::PeerToPeer::PeerToPeer(int p2pServerSidePort)
    : QObject (nullptr),
      mClientServerSideAddress("0.0.0.0:" + std::to_string(p2pServerSidePort))
{

    mAudioModule.reset(new AudioModule());
    mClientState = &QMLClientState::getInstance();
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
    //builder.AddListeningPort(mClientServerSideAddress, creds);
    builder.RegisterService(this);
    builder.AddChannelArgument(GRPC_ARG_KEEPALIVE_TIME_MS, 2000);
    builder.AddChannelArgument(GRPC_ARG_KEEPALIVE_TIMEOUT_MS, 5000);


    std::unique_ptr<Server> server(builder.BuildAndStart());
    LOG(plog::info) << "[Call]ClientSide-Server listening on port: " << mClientServerSideAddress;

    server->Wait();
}

void secure_voice_call::PeerToPeer::sendCallRequest(std::string ip, std::string callername) //new thread
{
    LOG(plog::info) << "[Call][ClientSide]try to call callcaller: " << callername << " " << ip;
    mClientState->setCallerName(QString::fromStdString(callername));
    std::shared_ptr<Channel> channel = grpc::CreateChannel(
                ip,
                grpc::InsecureChannelCredentials() //FIXME
                //grpc::SslCredentials(opts)
                );
    mstub = CallGreeter::NewStub(channel);
    CallRequest request;
    CallResponse response;

    request.set_callername(mClientState->authorizatedAs().toStdString()); //send your authorizated user name
    ClientContext context;
    mClientStream = mstub->HandShake(&context);
    mClientStream->Write(request);


    if(!raceOutgoingCall(response, context)){
       LOG(plog::info) << "[Call][ClientSide]!raceOutgoingCall ";
        mClientState->setState(QMLClientState::ClientStates::Online);
        return;
    }

    if(response.issuccessful()){
        mClientState->setState(QMLClientState::ClientStates::InConversation);
        mIsInConversation = true;
        LOG(plog::info) << "[Call][ClientSide]Client_ClientSide: response is successful";
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
        LOG(plog::info) << "[Call][ClientSide]Client_ClientSide HandShake thread joins has been reached";
    }else{
        mClientStream->WritesDone();
        mClientStream->Finish();
        QMLClientState::getInstance().setStatus("User have declined your call");
    }
    mClientState->setState(QMLClientState::ClientStates::Online);
}



grpc::Status secure_voice_call::PeerToPeer::HandShake(grpc::ServerContext *context, ::grpc::ServerReaderWriter<secure_voice_call::CallResponse, secure_voice_call::CallRequest> *stream)
{
    LOG(plog::info) << "[Call][ServerSide]HANDSHAKE";
    //check flag isInConversation
    CallRequest request;
    CallResponse response;
    if (mIsInConversation) {
        response.set_issuccessful(false);
        stream->Write(response);
        return Status::CANCELLED;
    }
    LOG(plog::info) << "[Call][ServerSide]: ClientStates::IncomingCall:";

    stream->Read(&request);
    mClientState->setState(QMLClientState::ClientStates::IncomingCall);
    mClientState->setCallerName(QString::fromStdString(request.callername()));
    if(raceIncomingCall(*context)){
        response.set_issuccessful(true);
        stream->Write(response);
        mIsInConversation = true;
        LOG(plog::info) << "[Call][ServerSide]: QMLClientState::ClientStates::InConversation:";
        mClientState->setState(QMLClientState::ClientStates::InConversation);
        std::thread tRead([this, stream, &context](){
            serverReadVoiceThread(stream, *context);
        });
        std::thread tWrite([this, stream, &context](){
            serverWriteVoiceThread(stream, *context);
        });
        tRead.join();
        tWrite.join();
        LOG(plog::info) << "[Call]Client_ServerSide HandShake thread joins reached";
    }else {
        response.set_issuccessful(false);
        stream->Write(response);
    }
    mIsInConversation = false;
    mClientState->setState(QMLClientState::ClientStates::Online);
    return Status::OK;
}

void secure_voice_call::PeerToPeer::clientReadVoiceThread(grpc::ClientContext& context)
{
    CallResponse response;
    mAudioModule->startPlayingAudio();
    LOG(plog::info) << "[Call][ClientSide][Audio]startPlayingAudio";
    while (mClientStream->Read(&response) && mIsInConversation) {
        mAudioModule->readVoice(response);
    }
    mAudioModule->stopPlayingAudio();
    LOG(plog::info) << "[Call][ClientSide][Audio]stopPlayingAudio";
    if(!mIsCanceledStream){
        mIsCanceledStream = true;
        context.TryCancel();
    }
    mIsInConversation = false;
}

void secure_voice_call::PeerToPeer::clientWriteVoiceThread(grpc::ClientContext& context)
{
    mAudioModule->startRecordingAudio();
    LOG(plog::info) << "[Call][ClientSide][Audio]startRecordingAudio";
    CallRequest request;
    while(mIsInConversation){
        if(mAudioModule->writeVoice(request)){
            if(!mClientStream->Write(request)){
                 LOG(plog::warning) << "[Call][ClientSide][Audio]There is no connection";
                mAudioModule->stopRecordingAudio();
                 LOG(plog::info) << "[Call][ClientSide][Audio]stopRecordingAudio";
                mIsInConversation = false;
                return;
            }
        }
    }
    mAudioModule->stopRecordingAudio();
     LOG(plog::info) << "[Call][ClientSide][Audio]stopRecordingAudio";
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
     LOG(plog::info) << "[Call][ServerSide][Audio]startPlayingAudio";
    while (stream->Read(&request) && mIsInConversation) {
          mAudioModule->readVoice(request);
    }
    mAudioModule->stopPlayingAudio();
   LOG(plog::info) << "[Call][ServerSide][Audio]stopPlayingAudio";
    if(!context.IsCancelled()){
        context.TryCancel();
    }
    mIsInConversation = false;
}

void secure_voice_call::PeerToPeer::serverWriteVoiceThread(ServerReaderWriter<CallResponse, CallRequest> *stream, grpc::ServerContext& context)
{
    CallResponse response;
    mAudioModule->startRecordingAudio();
    LOG(plog::info) << "[Call][ServerSide][Audio]startRecordingAudio";
    while(mIsInConversation){
        if(mAudioModule->writeVoice(response)){
            if(!stream->Write(response)){
                LOG(plog::warning) << "[Call][ServerSide][Audio]There is no connection";
                mAudioModule->stopRecordingAudio();
                LOG(plog::info) << "[Call][ServerSide][Audio]stopRecordingAudio";
                mIsInConversation = false;
                return;
            }
        }
    }
    mAudioModule->stopRecordingAudio();
    LOG(plog::info) << "[Call][ServerSide][Audio]stopRecordingAudio";
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
                                           [](QMetaObject::Connection *ptr){
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
                    LOG(plog::info) << "[Call]OutgoingCallStates::FinishedByReading";
                    success = true;
                    break;
                }
                case OutgoingCallStates::FinishedByCancel:
                {
                    LOG(plog::info) << "[Call]OutgoingCallStates::FinishedByCancel";
                    context.TryCancel();
                    break;
                }
                case OutgoingCallStates::FinishedByTimer:
                {
                    LOG(plog::info) << "[Call]OutgoingCallStates::FinishedByTimer";
                    context.TryCancel();
                    break;
                }
                default:
                {
                   LOG(plog::error) << "[Call]unhandled OutgoingCallStates!";
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
                                           [](QMetaObject::Connection *ptr){
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
