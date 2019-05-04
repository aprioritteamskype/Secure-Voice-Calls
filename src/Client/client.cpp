#include "client.h"

std::string secure_voice_call::Client::sendRequest(const std::string &requestStr)
{
       HelloRequest request;

       request.set_name(requestStr);

       HelloReply reply;

       ClientContext context;

       Status status = mstub->SayHello(&context, request, &reply);

       if(status.ok()){
           return reply.message();
       } else {
           std::cout << status.error_code() << ": " << status.error_message() << std::endl;
           return "bad response";
       }
   }
