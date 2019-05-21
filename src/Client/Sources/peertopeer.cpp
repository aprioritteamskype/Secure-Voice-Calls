#include "peertopeer.h"
#include <iostream> //FIXME
#include <memory>
#include <chrono>
#include<QThread>
#define PACKET_SIZE 16
//FIXME
#include <fstream>
#include <sstream>
#include <audiomodule.h>
using namespace std;

secure_voice_call::PeerToPeer::PeerToPeer()
    : //mClientServerSideAddress("0.0.0.0:5001")// FIXME
      mClientServerSideAddress("localhost:5001")
{

    mAudioModule.reset(new AudioModule());
    mClientState = &QMLClientState::getInstance();
    mServerThread =  std::thread([this](){
        runServer();
    });
}

void read ( const std::string& filename, std::string& data )
{
    std::ifstream file ( filename.c_str (), std::ios::in );

    if ( file.is_open () )
    {
        std::stringstream ss;
        ss << file.rdbuf ();

        file.close ();

        data = ss.str ();
    }else std::cout<<"FILE IS NOT OPENED"<<std::endl;

    return;
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

void secure_voice_call::PeerToPeer::sendCallRequest(const string &ip, const std::string &callername) //new thread
{
    std::string cert;
    std::string key;
    std::string root;
    read ( "/home/max/test_voice/src/Server/secur/client.crt", cert );
    read ( "/home/max/test_voice/src/Server/secur/client.key", key );
    read ( "/home/max/test_voice/src/Server/secur/ca.crt", root );


    grpc::SslCredentialsOptions opts =
    {
        root,
        key,
        cert
    };
    //
    cout << "try to call callcaller: " << callername << ip << endl;
    mContext = std::unique_ptr<ClientContext>(new ClientContext); //you cannot reuse this object between calls
    std::shared_ptr<Channel> channel = grpc::CreateChannel(
                ip,
                grpc::InsecureChannelCredentials() //FIXME
                //grpc::SslCredentials(opts)
                );
    cout << "mstub next " << endl;
    mstub = CallGreeter::NewStub(channel);
    CallRequest request;
    CallResponse response;
    request.set_callername(callername); //FIXME
    cout << "handshake? " << endl;
    mClientStream = mstub->HandShake(mContext.get());
    mClientStream->Write(request);
    mClientStream->Read(&response);
    if(response.issuccessful()){
        mClientState->setState(QMLClientState::ClientStates::InConversation);
        mIsInConversation = true;
        cout << "Client_ClientSide: response is successful" << endl;
        std::thread tRead([this](){
            clientReadVoice();
        });
        std::thread tWrite([this](){
            clientWriteVoice();
        });
        tRead.join();
        tWrite.join();
        cout << "Client_ClientSide HandShake thread joins has been reached" << endl;
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
    } else {
        cout << "ServerSide: ClientStates::IncomingCall: " << endl;
        mClientState->setStateInvoker(QMLClientState::ClientStates::IncomingCall);
        std::this_thread::sleep_for(std::chrono::seconds(10));

    }

    stream->Read(&request);
    if(true){ //if name has same ip as in server -> if callername = [Client::sendIdByUserNameRequest] FIXME
        response.set_issuccessful(true); //set flag isInConversation
        stream->Write(response);
        mIsInConversation = true;
        cout << "ServerSide: QMLClientState::ClientStates::InConversation: " << endl;
        mClientState->setStateInvoker(QMLClientState::ClientStates::InConversation);
        std::thread tRead([this, stream](){
            serverReadVoice(stream);
        });
        std::thread tWrite([this, stream](){
            serverWriteVoice(stream);
        });
        tRead.join();
        tWrite.join();
        cout << "Client_ServerSide HandShake thread joins reached" << endl;
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
    std::cout<<"PeerToPeer::clientReadVoice()"<<std::endl;
    //stub
    CallResponse response;
    mAudioModule->startPlayingAudio();
    while (mClientStream->Read(&response)) {
        mAudioModule->readVoice(response);
    }

    mAudioModule->stopPlayingAudio();
    mIsInConversation = false;
}

void secure_voice_call::PeerToPeer::clientWriteVoice()
{
    // stub
    std::cout<<"PeerToPeer::clientWriteVoice()"<<std::endl;
    mAudioModule->startRecordingAudio();
    CallRequest request;
    bool flag = true;

    while(flag){
        if(mAudioModule->writeVoice(request)){
            if(!mClientStream->Write(request)){
                std::cout<<"There is no connection"<<std::endl;
                mAudioModule->stopRecordingAudio();
                mIsInConversation = false;
                return;
            }
        }
    }
    mAudioModule->stopRecordingAudio();
    mIsInConversation = false;
}

void secure_voice_call::PeerToPeer::serverReadVoice(ServerReaderWriter<CallResponse, CallRequest> *stream)
{
    std::cout<<"PeerToPeer::serverReadVoice(ServerReaderWriter<CallResponse, CallRequest> *stream)"<<std::endl;
    //stub
    CallRequest request;
    mAudioModule->startPlayingAudio();
    while (stream->Read(&request)) {
        mAudioModule->readVoice(request);
//        packetSize = request.packetsize();
//        memcpy(&pBuff[mPlayer->get_mBuffReadyToRead()%buffSize],request.audiobytes().c_str(), packetSize); //write data to buffer
//        mPlayer->raise_mBufReadyToRead(packetSize); //next buffer location
    }
    mAudioModule->stopPlayingAudio();
//    emit stopPlayingAudio();
    mIsInConversation = false;
}

void secure_voice_call::PeerToPeer::serverWriteVoice(ServerReaderWriter<CallResponse, CallRequest> *stream)
{
    std::cout<<"PeerToPeer::serverWriteVoice(ServerReaderWriter<CallResponse, CallRequest> *stream)"<<std::endl;
    //stub
    CallResponse response;
    mAudioModule->startRecordingAudio();
    bool flag = true;
    while(flag){
        if(mAudioModule->writeVoice(response)){
            if(!stream->Write(response)){
                std::cout<<"There is no connection"<<std::endl;
                mAudioModule->stopRecordingAudio();
                mIsInConversation = false;
                return;
            }
        }
    }
    mAudioModule->stopRecordingAudio();
    mIsInConversation = false;
}
