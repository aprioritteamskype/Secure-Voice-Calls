#include "peertopeer.h"
#include <iostream> //FIXME
#include <memory>
#include <chrono>
#define PACKET_SIZE 16
#include <QEventLoop>
#include <QTimer>
using namespace std;
secure_voice_call::PeerToPeer::PeerToPeer(int p2pServerSidePort)
    : QObject (nullptr),
      mClientServerSideAddress("0.0.0.0:" + std::to_string(p2pServerSidePort))
#include <audiomodule.h>
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
    //    std::string key;
    //    std::string cert;
    //    std::string root;

    //    read ( "/home/max/test_voice/src/Server/secur/server.crt", cert );
    //    read ( "/home/max/test_voice/src/Server/secur/server.key", key );
    //    read ( "/home/max/test_voice/src/Server/secur/ca.crt", root );

    //    grpc::SslServerCredentialsOptions::PemKeyCertPair pkcp;
    //    pkcp.private_key = key;
    //    pkcp.cert_chain = cert;

    //    grpc::SslServerCredentialsOptions ssl_opts;
    //    ssl_opts.pem_key_cert_pairs.push_back(pkcp);
    //    ssl_opts.pem_root_certs=root;

    //    std::shared_ptr<grpc::ServerCredentials> creds ;
    //    creds = grpc::SslServerCredentials(ssl_opts);
    //
    ServerBuilder builder;

    builder.AddListeningPort(mClientServerSideAddress, grpc::InsecureServerCredentials()); //FIXME
    //builder.AddListeningPort(mClientServerSideAddress, creds);
    builder.RegisterService(this);

    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "ClientSide-Server listening on port: " << mClientServerSideAddress << std::endl;

    server->Wait();
}

void secure_voice_call::PeerToPeer::sendCallRequest(std::string ip, std::string callername) //new thread
{
    std::cout << "try to call callcaller: " << callername << " " << ip << std::endl;
    std::shared_ptr<Channel> channel = grpc::CreateChannel(
                ip,
                grpc::InsecureChannelCredentials() //FIXME
                //grpc::SslCredentials(opts)
                );
    cout << "mstub next " << endl;
    mstub = CallGreeter::NewStub(channel);
    CallRequest request;
    CallResponse response;

    request.set_callername(callername);
    ClientContext context;
    mClientStream = mstub->HandShake(&context);
    mClientStream->Write(request);


    if(!raceOutgoingCall(response, context)){
        std::cout << "!raceOutgoingCall " << std::endl;
        mClientState->setState(QMLClientState::ClientStates::Online);
        return;
    }

    if(response.issuccessful()){
        mClientState->setState(QMLClientState::ClientStates::InConversation);
        mIsInConversation = true;
        std::cout << "Client_ClientSide: response is successful" << std::endl;
        std::thread tRead([this](){
            clientReadVoice();
        });
        std::thread tWrite([this](){
            clientWriteVoice();
        });
        tRead.join();
        tWrite.join();
        mClientStream->WritesDone();
        mClientStream->Finish();
        std::cout << "Client_ClientSide HandShake thread joins has been reached" << std::endl;
    }else{
         cout << "WOW WTF? " << endl;
        mClientStream->WritesDone();
        mClientStream->Finish();
    }
    mClientState->setState(QMLClientState::ClientStates::Online);
}



grpc::Status secure_voice_call::PeerToPeer::HandShake(grpc::ServerContext *context, ::grpc::ServerReaderWriter<secure_voice_call::CallResponse, secure_voice_call::CallRequest> *stream)
{
    std::cout<<"HANDSHAKE"<<std::endl;
    //check flag isInConversation
    CallRequest request;
    CallResponse response;
    if (mIsInConversation) {
        response.set_issuccessful(false);
        stream->Write(response);
        return Status::CANCELLED;
    }
    std::cout << "ServerSide: ClientStates::IncomingCall: " << std::endl;
    mClientState->setState(QMLClientState::ClientStates::IncomingCall);

    stream->Read(&request);
    if(raceIncomingCall(*context)){
        response.set_issuccessful(true);
        stream->Write(response);
        mIsInConversation = true;
        std::cout << "ServerSide: QMLClientState::ClientStates::InConversation: " << std::endl;
        mClientState->setState(QMLClientState::ClientStates::InConversation);
        std::thread tRead([this, stream](){
            serverReadVoice(stream);
        });
        std::thread tWrite([this, stream](){
            serverWriteVoice(stream);
        });
        tRead.join();
        tWrite.join();
        std::cout << "Client_ServerSide HandShake thread joins reached" << std::endl;
    }else {
        response.set_issuccessful(false);
        stream->Write(response);
    }
    mIsInConversation = false;
    mClientState->setState(QMLClientState::ClientStates::Online);
    return Status::OK;
}

void secure_voice_call::PeerToPeer::clientReadVoice()
{
    CallResponse response;
    mAudioModule->startPlayingAudio();
    std::cout<<"startPlayingAudio"<<std::endl;
    while (mClientStream->Read(&response) && mIsInConversation) {
        mAudioModule->readVoice(response);
    }
    mAudioModule->stopPlayingAudio();
    std::cout<<"stopPlayingAudio"<<std::endl;
    mIsInConversation = false;
}

void secure_voice_call::PeerToPeer::clientWriteVoice()
{
    mAudioModule->startRecordingAudio();
    std::cout<<"startRecordingAudio"<<std::endl;
    CallRequest request;
    while(mIsInConversation){
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
    mIsInConversation = false;
}

void secure_voice_call::PeerToPeer::serverReadVoice(ServerReaderWriter<CallResponse, CallRequest> *stream)
{
    CallRequest request;
    mAudioModule->startPlayingAudio();
    std::cout<<"startPlayingAudio"<<std::endl;
    while (stream->Read(&request) && mIsInConversation) {
          mAudioModule->readVoice(request);
    }
    mAudioModule->stopPlayingAudio();
    std::cout<<"stopPlayingAudio"<<std::endl;
    mIsInConversation = false;
}

void secure_voice_call::PeerToPeer::serverWriteVoice(ServerReaderWriter<CallResponse, CallRequest> *stream)
{
    CallResponse response;
    mAudioModule->startRecordingAudio();
    std::cout<<"startRecordingAudio"<<std::endl;
    while(mIsInConversation){
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
    mIsInConversation = false;
}

//what earlier, user on the other side take a call or user on this side cancel
bool secure_voice_call::PeerToPeer::raceOutgoingCall(secure_voice_call::CallResponse &response, ClientContext& context)
{
    OutgoingCallStates outgoingState = OutgoingCallStates::NotFinished;
    bool success = false;
    std::thread readThread([this, &response](){
        mClientStream->Read(&response);
        emit finishPeerToPeerOutgoingCall(OutgoingCallStates::FinishedByReading);
    });
    //[timer]-----------------------------------------------------------------------------
    std::thread timerThread([this](){
        std::this_thread::sleep_for(std::chrono::seconds(10));
        emit finishPeerToPeerOutgoingCall(OutgoingCallStates::FinishedByTimer);
    });
    timerThread.detach();
    //[timer]-----------------------------------------------------------------------------
    QEventLoop loop;
    std::shared_ptr<QMetaObject::Connection> sh(new QMetaObject::Connection(),
                                           [](QMetaObject::Connection *ptr){
        QObject::disconnect(*ptr);
        delete ptr;
    });
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
            default: break;
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
        std::this_thread::sleep_for(std::chrono::seconds(10));
        emit finishPeerToPeerIncomingCall(false);
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
