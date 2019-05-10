#include "server.h"

grpc::Status secure_voice_call::Server::Authorization(grpc::ServerContext *context, const AuthorizationRequest *request, AuthorizationResponse *response)
{
    std::cout << context->peer() << std::endl;
    std::cout << request->name() << std::endl;

    response->set_issuccessful(m_clientOnline.insert(
                                   std::pair<std::string,std::string>(request->name(),context->peer()))
                               .second);
    if (response->issuccessful()){
        for (const auto &element : m_clientOnline){
            response->add_user_names(element.first);
        }
        return Status::OK;
    }else return  Status::CANCELLED;
}
