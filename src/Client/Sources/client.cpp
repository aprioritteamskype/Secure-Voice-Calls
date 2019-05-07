#include "client.h"


grpc::Status secure_voice_call::Client::sendAuthorizationRequest(const std::string &name, bool &authorizationSuccesfull)
{
    AuthorizationRequest request;
    request.set_name(name);

    AuthorizationResponse response;
    ClientContext context;
    Status status = mstub->Authorization(&context, request, &response);
    authorizationSuccesfull = response.issuccessful();
    return status;
}
