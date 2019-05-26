#include "peertopeer.h"
#include <iostream> //FIXME
#include <memory>
#include <chrono>
#include <QEventLoop>
#include <QTimer>

secure_voice_call::PeerToPeer::PeerToPeer(int p2pServerSidePort)
    : QObject (nullptr),
      mClientServerSideAddress("0.0.0.0:" + std::to_string(p2pServerSidePort))
{
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

    builder.AddListeningPort(mClientServerSideAddress, grpc::InsecureServerCredentials());
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
                grpc::InsecureChannelCredentials()
                );
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
        mClientStream->WritesDone();
        mClientStream->Finish();
    }
    mClientState->setState(QMLClientState::ClientStates::Online);
}



grpc::Status secure_voice_call::PeerToPeer::HandShake(grpc::ServerContext *context, ::grpc::ServerReaderWriter<secure_voice_call::CallResponse, secure_voice_call::CallRequest> *stream)
{
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
    //stub
    CallResponse response;
    while (mClientStream->Read(&response) && mIsInConversation) {
        std::cout << "Client_ClientSide get response: " << response.audiobytes() << std::endl;
        //do something with request.audiobytes
    }
    mIsInConversation = false;
}

void secure_voice_call::PeerToPeer::clientWriteVoice()
{
    // stub
    CallRequest request;
    for (int i = 20; i < 24 && mIsInConversation; ++i) {
        request.set_audiobytes(i);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        mClientStream->Write(request);
    }
    mIsInConversation = false;
}

void secure_voice_call::PeerToPeer::serverReadVoice(ServerReaderWriter<CallResponse, CallRequest> *stream)
{
    //stub
    CallRequest request;
    while (stream->Read(&request) && mIsInConversation) {
        std::cout << "Client_ServerSide get request: " << request.audiobytes() << std::endl;
        //do something with request.audiobytes
    }
    mIsInConversation = false;
}

void secure_voice_call::PeerToPeer::serverWriteVoice(ServerReaderWriter<CallResponse, CallRequest> *stream)
{
    //stub
    CallResponse response;
    for (int i =0; i < 4 && mIsInConversation; ++i) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        response.set_audiobytes(i);
        stream->Write(response);
    }
    mIsInConversation = false;
    //--------------------------------------------------
    //audio record code
    //    CallResponse response;
    //    response.set_audiobytes(11111);
    //    while (true) {
    //        if(!stream->Write(response)){
    //            break;
    //        }
    //    }
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
