#include "peertopeer.h"
#include <iostream> //FIXME
#include <memory>
#include <thread>
#define PACKET_SIZE 16
using namespace std;

secure_voice_call::PeerToPeer::PeerToPeer()
    : QObject (nullptr),
      mServerAddress("0.0.0.0:5001")
{
    mRecorder.reset(new Recorder());
    connect(this,SIGNAL(startRecordingAudio()),mRecorder.get(),SLOT(runRecorder()));
    connect(this,SIGNAL(stopRecordingAudio()),mRecorder.get(),SLOT(stopRecorder()));
    mPlayer.reset(new Player());
    connect(this,SIGNAL(startPlayingAudio()),mPlayer.get(),SLOT(runPlayer()));
    connect(this,SIGNAL(stopPlayingAudio()),mPlayer.get(),SLOT(stopPlayer()));

    recThread = new QThread(this);
    playThread = new QThread(this);
    connect(this,SIGNAL(destroyed()),recThread,SLOT(quit()));
    connect(this,SIGNAL(destroyed()),playThread,SLOT(quit()));
    mPlayer->moveToThread(playThread);
    mRecorder->moveToThread(recThread);
    playThread->start();
    recThread->start();
}

void secure_voice_call::PeerToPeer::runServer()
{
    ServerBuilder builder;

    builder.AddListeningPort(mServerAddress, grpc::InsecureServerCredentials());
    builder.RegisterService(this);

    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "ClientSide-Server listening on port: " << mServerAddress << std::endl;

    server->Wait();
}

void secure_voice_call::PeerToPeer::sendCallRequest(const string &ip, const std::string &callername) //new thread
{
    mContext = std::unique_ptr<ClientContext>(new ClientContext); //you cannot reuse this object between calls
    std::shared_ptr<Channel> channel = grpc::CreateChannel(
                ip,
                grpc::InsecureChannelCredentials()
                );
    mstub = CallGreeter::NewStub(channel);

    CallRequest request;
    CallResponse response;

    request.set_callername("STUBCALLERNAME"); //FIXME
    mClientStream = mstub->HandShake(mContext.get());
    mClientStream->Write(request);
    mClientStream->Read(&response);
    if(response.issuccessful()){
        cout << "Client_ClientSide: response is successful" << endl;
        std::thread tRead([this](){
            clientReadVoice();
        });
        std::thread tWrite([this](){
            clientWriteVoice();
        });
        tRead.join();
        tWrite.join();
        cout << "Client_ClientSide HandShake thread joins reached" << endl;
    }else{
        mClientStream->WritesDone();
        mClientStream->Finish();
    }
}

grpc::Status secure_voice_call::PeerToPeer::HandShake(grpc::ServerContext *context, ::grpc::ServerReaderWriter<secure_voice_call::CallResponse, secure_voice_call::CallRequest> *stream)
{
    //check flag isInConversation
    CallRequest request;
    CallResponse response;

    stream->Read(&request);
    cout << "Client_ServerSide callername:" <<request.callername() <<endl;
    if(true){ //if name has same ip as in server -> if callername = [Client::sendIdByUserNameRequest]
        response.set_issuccessful(true); //set flag isInConversation
        stream->Write(response);
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

    return Status::OK;
}

void secure_voice_call::PeerToPeer::clientReadVoice()
{
    //stub
    CallResponse response;
    char* const pBuff = mPlayer->getBuff();//pointer to buffer in Player
    const quint64 buffSize = mPlayer->getBuffSize();
    quint32 packetSize;
    emit startPlayingAudio();
        while (mClientStream->Read(&response)) {
            packetSize = response.packetsize();
            memcpy(&pBuff[mPlayer->get_mBuffReadyToRead()%buffSize],response.audiobytes().c_str(), packetSize); //write data to buffer
            mPlayer->raise_mBufReadyToRead(packetSize); //next buffer location
        }
    emit stopPlayingAudio();
    return;
}

void secure_voice_call::PeerToPeer::clientWriteVoice()
{
    emit startRecordingAudio();
    CallRequest request;
    bool flag = true;

    char* const pBuff = mRecorder->getBuff();//pointer to buffer in Recorder
    quint64 buffSize = mRecorder->getBuffSize();
    char* sendingPacket = new char[PACKET_SIZE];

    while(flag){
        if(mRecorder->getBuffWritePos() - mBufSendPos > PACKET_SIZE){
            memcpy(sendingPacket,&pBuff[mBufSendPos%buffSize],PACKET_SIZE);
            mBufSendPos += PACKET_SIZE;
            request.set_audiobytes(sendingPacket,PACKET_SIZE);
            request.set_packetsize(PACKET_SIZE);
            if(!mClientStream->Write(request)){
                std::cout<<"There is no connection"<<std::endl;
                emit stopRecordingAudio();
                return;
            }
        }
    }
    emit stopPlayingAudio();
}

void secure_voice_call::PeerToPeer::serverReadVoice(ServerReaderWriter<CallResponse, CallRequest> *stream)
{
    //stub
    CallRequest request;
    char* const pBuff = mPlayer->getBuff();//pointer to buffer in Player
    const quint64 buffSize = mPlayer->getBuffSize();
    quint32 packetSize;
    emit startPlayingAudio();
        while (stream->Read(&request)) {
            packetSize = request.packetsize();
            memcpy(&pBuff[mPlayer->get_mBuffReadyToRead()%buffSize],request.audiobytes().c_str(), packetSize); //write data to buffer
            mPlayer->raise_mBufReadyToRead(packetSize); //next buffer location
        }
    emit stopPlayingAudio();
    return;
}

void secure_voice_call::PeerToPeer::serverWriteVoice(ServerReaderWriter<CallResponse, CallRequest> *stream)
{
    //stub
    CallResponse response;
    emit startRecordingAudio();
    bool flag = true;

    char* const pBuff = mRecorder->getBuff();//pointer to buffer in Recorder
    quint64 buffSize = mRecorder->getBuffSize();
    char* sendingPacket = new char[PACKET_SIZE];

    while(flag){
        if(mRecorder->getBuffWritePos() - mBufSendPos > PACKET_SIZE){
            memcpy(sendingPacket,&pBuff[mBufSendPos%buffSize],PACKET_SIZE);
            mBufSendPos += PACKET_SIZE;
            response.set_audiobytes(sendingPacket,PACKET_SIZE);
            response.set_packetsize(PACKET_SIZE);
            if(!stream->Write(response)){
                std::cout<<"There is no connection"<<std::endl;
                emit stopRecordingAudio();
                return;
            }
        }
    }
    emit stopPlayingAudio();
}
