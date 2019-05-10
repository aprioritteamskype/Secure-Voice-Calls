#include "server.h"
#include <algorithm>

void secure_voice_call::Server::addClientsToResponse(secure_voice_call::AuthorizationResponse &response)
{
    for (const auto &element : m_clientOnline){
        response.add_user_names(element.first);
    }
}

grpc::Status secure_voice_call::Server::Authorization(grpc::ServerContext *context,
                                                      ServerReaderWriter<secure_voice_call::AuthorizationResponse,
                                                      secure_voice_call::AuthorizationRequest> *stream)
{
    secure_voice_call::AuthorizationRequest request;
    secure_voice_call::AuthorizationResponse response;
    std::string name;
    if (stream->Read(&request)) {
        response.set_issuccessful(m_clientOnline.insert(
                                      std::pair<std::string,std::string>(request.name(),context->peer()))
                                  .second);
        if (response.issuccessful()){
            name = request.name();
            addClientsToResponse(response);
            stream->Write(response);
            while (stream->Read(&request)) {
                response.Clear();
                addClientsToResponse(response);
                stream->Write(response);
            }
            m_clientOnline.erase(m_clientOnline.find(name));
            return Status::OK;
        } else {
            return  Status::CANCELLED;
        }
    } else {
        return  Status::CANCELLED;
    }
}
