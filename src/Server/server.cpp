#include "server.h"
#include <algorithm>

void secure_voice_call::Server::addClientsToResponse(secure_voice_call::AuthorizationResponse &response)
{
    for (const auto &element : m_clientOnline){
        response.add_usernames(element.first);
    }
}

grpc::Status secure_voice_call::Server::Authorization(grpc::ServerContext *context,
                                                      ServerReaderWriter<secure_voice_call::AuthorizationResponse,
                                                      secure_voice_call::AuthorizationRequest> *stream)
{
    secure_voice_call::AuthorizationRequest request;
    secure_voice_call::AuthorizationResponse response;

    std::string name;
    while (stream->Read(&request)) {
        switch (static_cast<secure_voice_call::TypeMessage>(request.requesttype())) {
            case secure_voice_call::TypeMessage::Authorization:
            {
                response.set_responsetype(secure_voice_call::TypeMessage::Authorization);
                response.set_issuccessful(m_clientOnline.insert(
                                              std::pair<std::string,std::string>(request.name(),context->peer()))
                                          .second);
                if (response.issuccessful()){
                    name = request.name();
                    addClientsToResponse(response);
                    stream->Write(response);
                }else {
                    return Status::CANCELLED;
                }
                break;
            }
            case secure_voice_call::TypeMessage::GetClientsOnline:
            {
                response.Clear();
                response.set_responsetype(secure_voice_call::TypeMessage::GetClientsOnline);
                addClientsToResponse(response);
                stream->Write(response);
                break;
            }
            case secure_voice_call::TypeMessage::GetIpByUserName:
            {
                response.Clear();
                response.set_responsetype(secure_voice_call::TypeMessage::GetIpByUserName);
                auto it = m_clientOnline.find(request.getipbyusername());
                if(it != m_clientOnline.end()){
                response.set_userip(it->second);
                stream->Write(response);
                } else {
                    //FIXME not reliable way of processing
                    //NEEDTO add error field to proto
                    //NEEDTO process the false return value of stream->Write(response);
                    //NEEDTO MBP - to move common parts of this section to separate function
                    return Status::CANCELLED;
                }
                break;
            }
        }
    }
    auto it = m_clientOnline.find(name);
    if(it != m_clientOnline.end()){
        m_clientOnline.erase(m_clientOnline.find(name));
    }
    return Status::OK;
}

