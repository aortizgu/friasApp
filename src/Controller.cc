#include "Controller.h"

#include <spdlog/spdlog.h>

#include "models/Menu.h"
#include "version.h"

using namespace drogon_model::sqlite3;

void menuFromReq(const HttpRequestPtr &req, Menu &menu) {
    menu.setName(req->getParameter("name"));
    menu.setFirst1(req->getParameter("first1"));
    menu.setFirst2(req->getParameter("first2"));
    menu.setFirst3(req->getParameter("first3"));
    menu.setFirst4(req->getParameter("first4"));
    menu.setFirst5(req->getParameter("first5"));
    menu.setFirst6(req->getParameter("first6"));
    menu.setSecond1(req->getParameter("second1"));
    menu.setSecond2(req->getParameter("second2"));
    menu.setSecond3(req->getParameter("second3"));
    menu.setSecond4(req->getParameter("second4"));
    menu.setSecond5(req->getParameter("second5"));
    menu.setSecond6(req->getParameter("second6"));
    menu.setComment1(req->getParameter("comment1"));
    menu.setComment2(req->getParameter("comment2"));
    menu.setComment3(req->getParameter("comment3"));
    menu.setEnabled(req->getParameter("enabled") == "on");
}

void Controller::list(const HttpRequestPtr &req,
        std::function<void(const HttpResponsePtr&)> &&callback) const {
    auto resp = HttpResponse::newHttpResponse();
    try {
        Mapper<Menu> mp(app().getDbClient());
        auto menus = mp.findAll();

        HttpViewData data;
        data.insert("title", "ListMenu");
        data.insert("menus", menus);
        resp = HttpResponse::newHttpViewResponse("MenusView.csp", data);

        for (auto &&menu : menus) {

            spdlog::info("Controller::list: {0}", menu.getValueOfName());
        }
    } catch (const DrogonDbException &e) {
        resp->setStatusCode(HttpStatusCode::k500InternalServerError);
        spdlog::error("Controller::list: exception {0}", e.base().what());
    }
    callback(resp);
}

void Controller::admin(const HttpRequestPtr &req,
        std::function<void(const HttpResponsePtr&)> &&callback) const {
    auto resp = HttpResponse::newHttpResponse();
    try {
        HttpViewData data;
        data.insert("title", "Admin");

        if (!req->getCookie("message").empty()) {
            data.insert("message", req->getCookie("message"));
        }

        Mapper<Menu> mpMenu(app().getDbClient());
        auto menus = mpMenu.findAll();
        data.insert("menus", menus);
        data.insert("version", FRIASAPP_VERSION);
        resp = HttpResponse::newHttpViewResponse("AdminView.csp", data);
    } catch (const DrogonDbException &e) {
        resp->setStatusCode(HttpStatusCode::k500InternalServerError);
        spdlog::error("Controller::_admin: exception {0}", e.base().what());
    }
    resp->addCookie("message", "");
    callback(resp);
}

void Controller::print(const HttpRequestPtr &req,
        std::function<void(const HttpResponsePtr&)> &&callback) const {
    int id = 0;
    try {
        std::string::size_type sz;
        id = std::stoi(req->getParameter("id"), &sz);
    } catch (...) {
    }
    if (id) {
        try {
            Mapper<Menu> mp(app().getDbClient());
            auto menu = mp.findByPrimaryKey(id);

            spdlog::info("Controller::print: menu to print, name : {0}",
                    menu.getValueOfName());

            HttpViewData data;
            data.insert("menu", menu);
            auto resp = HttpResponse::newHttpViewResponse("PrintView.csp",
                    data);
            callback(resp);
        } catch (const DrogonDbException &e) {
            spdlog::error("Controller::print: exception {0}", e.base().what());
            auto resp = HttpResponse::newHttpViewResponse("AdminView.csp");
            resp->addCookie("message",
                    "Error, no se ha encontrado el menu id "
                            + req->getParameter("id"));
            callback(resp);
        }
    } else {
        auto resp = HttpResponse::newHttpViewResponse("AdminView.csp");
        resp->addCookie("message",
                "Error, no se ha encontrado el menu id "
                        + req->getParameter("id"));
        callback(resp);
    }
}

void Controller::edit(const HttpRequestPtr &req,
        std::function<void(const HttpResponsePtr&)> &&callback) const {
    int id = 0;
    try {
        std::string::size_type sz;
        id = std::stoi(req->getParameter("id"), &sz);
    } catch (...) {
    }
    if (id) {
        try {
            Mapper<Menu> mp(app().getDbClient());
            auto menu = mp.findByPrimaryKey(id);

            spdlog::info("Controller::edit: menu to edit, name : {0}",
                    menu.getValueOfName());

            HttpViewData data;
            data.insert("menu", menu);
            auto resp = HttpResponse::newHttpViewResponse("EditView.csp", data);
            callback(resp);
        } catch (const DrogonDbException &e) {
            spdlog::error("Controller::edit: exception {0}", e.base().what());
            auto resp = HttpResponse::newHttpViewResponse("EditView.csp");
            callback(resp);
        }
    } else {
        auto resp = HttpResponse::newHttpViewResponse("EditView.csp");
        callback(resp);
    }
}

void Controller::save(const HttpRequestPtr &req,
        std::function<void(const HttpResponsePtr&)> &&callback) const {
    auto resp = HttpResponse::newRedirectionResponse("/admin");
    int id = 0;
    try {
        std::string::size_type sz;
        id = std::stoi(req->getParameter("id"), &sz);
    } catch (...) {
    }
    Menu menu;
    menuFromReq(req, menu);
    try {
        Mapper<Menu> mp(app().getDbClient());
        if (id) {
            menu.setId(id);
            if (mp.update(menu)) {
                spdlog::info("Controller::save: menu updated, id {0}", id);
                resp->addCookie("message",
                        "Modificado menu id "
                                + std::to_string(menu.getValueOfId()));
            } else {
                spdlog::info("Controller::save: menu not found, id {0}", id);
                resp->addCookie("message",
                        "Error, no se ha encontrado el menu id "
                                + req->getParameter("id"));
            }
        } else {
            mp.insert(menu);
            spdlog::info("Controller::save: menu added, id {0}",
                    menu.getValueOfId());
            resp->addCookie("message",
                    "Añadido nuevo menu id "
                            + std::to_string(menu.getValueOfId()));
        }
    } catch (const DrogonDbException &e) {
        spdlog::error("Controller::update: exception {0}", e.base().what());
        resp->addCookie("message", e.base().what());
    }
    callback(resp);
}

void Controller::del(const HttpRequestPtr &req,
        std::function<void(const HttpResponsePtr&)> &&callback) const {
    auto resp = HttpResponse::newRedirectionResponse("/admin");
    int id = 0;
    try {
        std::string::size_type sz;
        id = std::stoi(req->getParameter("id"), &sz);
    } catch (...) {
    }
    if (id) {
        try {
            Mapper<Menu> mp(app().getDbClient());
            if (mp.deleteByPrimaryKey(id)) {
                spdlog::info("Controller::del: deleted {0}", id);
                resp->addCookie("message",
                        "Eliminado menu id " + req->getParameter("id"));
            } else {
                spdlog::info("Controller::del: not found menu {0}", id);
                resp->addCookie("message",
                        "Error, no se ha encontrado el menu id "
                                + req->getParameter("id"));
            }
        } catch (const DrogonDbException &e) {
            spdlog::error("Controller::del: exception {0}", e.base().what());
            resp->addCookie("message", e.base().what());
        }
    } else {
        spdlog::error("Controller::del: no id to delete, nothing to do");
        resp->addCookie("message",
                "Error, no se ha encontrado el menu id "
                        + req->getParameter("id"));
    }
    callback(resp);
}
