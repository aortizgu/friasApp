#pragma once
#include <drogon/HttpController.h>
using namespace drogon;
class aog : public drogon::HttpController<aog>
{
public:
  METHOD_LIST_BEGIN
  METHOD_ADD(aog::hello,
             "/hello/{userName}",
             Get,
             "DigestAuthFilter");
  METHOD_ADD(aog::users,
             "/users",
             Get,
             "DigestAuthFilter");
  METHOD_LIST_END
  void hello(const HttpRequestPtr &req,
             std::function<void(const HttpResponsePtr &)> &&callback,
             const std::string &userName) const;
  void users(const HttpRequestPtr &req,
             std::function<void(const HttpResponsePtr &)> &&callback) const;
};
