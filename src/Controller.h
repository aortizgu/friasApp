#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class Controller: public drogon::HttpController<Controller> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(Controller::list, "/", Get);
    ADD_METHOD_TO(Controller::admin, "/admin", Get, "DigestAuthFilter");
    ADD_METHOD_TO(Controller::add, "/menu/add", Post, "DigestAuthFilter");
    ADD_METHOD_TO(Controller::del, "/menu/del", Post, "DigestAuthFilter");
    ADD_METHOD_TO(Controller::update, "/menu/update", Post, "DigestAuthFilter");
    METHOD_LIST_END

    void list(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr&)> &&callback) const;
    void _admin(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr&)> &&callback,
            const std::string &errorMsg = "") const;
    void admin(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr&)> &&callback) const;
    void add(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr&)> &&callback) const;
    void del(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr&)> &&callback) const;
    void update(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr&)> &&callback) const;
};
