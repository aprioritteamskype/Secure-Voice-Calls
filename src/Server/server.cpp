#include "server.h"
#include <QUrl>

grpc::Status secure_voice_call::Server::Authorization(grpc::ServerContext *context, const AuthorizationRequest *request, AuthorizationResponse *response)
{
    std::cout << context->peer() << std::endl;
    std::cout << request->name() << std::endl;

    response->set_issuccessful(m_clientOnline.insert(
                                   std::pair<std::string,std::string>(request->name(),context->peer()))
                               .second);
    return Status::OK;
}
