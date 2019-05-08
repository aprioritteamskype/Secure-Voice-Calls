#include "client.h"


grpc::Status secure_voice_call::Client::sendAuthorizationRequest(const std::string &name, bool &authorizationSuccesfull, std::vector<std::string>& clients)
{
    AuthorizationRequest request;
    request.set_name(name);

    AuthorizationResponse response;
    ClientContext context;
    Status status = mstub->Authorization(&context, request, &response);


    authorizationSuccesfull = response.issuccessful();
    for(int i = 0, size = response.user_names_size(); i < size; ++i){
        clients.push_back(response.user_names(i));
    }

    return status;
}
