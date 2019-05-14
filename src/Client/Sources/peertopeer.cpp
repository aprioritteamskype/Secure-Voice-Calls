#include "peertopeer.h"
#include <iostream> //FIXME
#include <memory>
#include <thread>

using namespace std;

secure_voice_call::PeerToPeer::PeerToPeer()
    : QObject (nullptr),
      mServerAddress("0.0.0.0:5001")
{

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
    while (mClientStream->Read(&response)) {
        cout << "Client_ClientSide get response: " << response.audiobytes() << endl;
        //do something with request.audiobytes
    }
}

void secure_voice_call::PeerToPeer::clientWriteVoice()
{
    // stub
    CallRequest request;
    for (int i = 20; i < 30; ++i) {
        request.set_audiobytes(i);
        mClientStream->Write(request);
    }
}

void secure_voice_call::PeerToPeer::serverReadVoice(ServerReaderWriter<CallResponse, CallRequest> *stream)
{
    //stub
    CallRequest request;
    while (stream->Read(&request)) {
        cout << "Client_ServerSide get request: " << request.audiobytes() << endl;
        //do something with request.audiobytes
    }
}

void secure_voice_call::PeerToPeer::serverWriteVoice(ServerReaderWriter<CallResponse, CallRequest> *stream)
{
    //stub
    CallResponse response;
    for (int i =0; i < 10; ++i) {
        response.set_audiobytes(i);
        stream->Write(response);
    }
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
