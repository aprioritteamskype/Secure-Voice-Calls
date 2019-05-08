#include "server.h"

grpc::Status secure_voice_call::Server::Authorization(grpc::ServerContext *context, const AuthorizationRequest *request, AuthorizationResponse *response)
{
    std::cout << context->peer() << std::endl;
    std::cout << request->name() << std::endl;

    response->set_issuccessful(m_clientOnline.insert(
                                   std::pair<std::string,std::string>(request->name(),context->peer()))
                               .second);
    if (response->issuccessful()){
        for(int i =0; i< 10; ++i){
            std::string str = std::string("Vasya ") + std::to_string(i);
            response->add_user_names(str);
        }
    }
    return Status::OK;
}
