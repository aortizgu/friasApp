#include "aog.h"

void aog::hello(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback,
                const std::string &userName) const
{
    auto resp = HttpResponse::newHttpResponse();
    resp->setBody("<P>" + userName + "</P>");
    callback(resp);
}