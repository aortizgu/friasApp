#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

class Controller: public drogon::HttpController<Controller> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(Controller::list, "/", Get);
    ADD_METHOD_TO(Controller::listOld, "/portal/index.php", Get);
    ADD_METHOD_TO(Controller::admin, "/admin", Get, "DigestAuthFilter");
    ADD_METHOD_TO(Controller::edit, "/edit", Get, "DigestAuthFilter");
    ADD_METHOD_TO(Controller::print, "/print", Get, "DigestAuthFilter");
    ADD_METHOD_TO(Controller::save, "/save", Get, "DigestAuthFilter");
    ADD_METHOD_TO(Controller::del, "/del", Get, "DigestAuthFilter");
    METHOD_LIST_END

    void list(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr&)> &&callback) const;
    void listOld(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr&)> &&callback) const;
    void admin(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr&)> &&callback) const;
    void print(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr&)> &&callback) const;
    void edit(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr&)> &&callback) const;
    void save(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr&)> &&callback) const;
    void del(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr&)> &&callback) const;
};
