#include "Controller.h"

#include "models/Menu.h"

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
            LOG_INFO << "Controller::list: " << menu.getValueOfName();
        }
    } catch (const DrogonDbException &e) {
        resp->setStatusCode(HttpStatusCode::k500InternalServerError);
        LOG_ERROR << "Controller::list: exception " << e.base().what();
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

        resp = HttpResponse::newHttpViewResponse("AdminView.csp", data);
    } catch (const DrogonDbException &e) {
        resp->setStatusCode(HttpStatusCode::k500InternalServerError);
        LOG_ERROR << "Controller::_admin: exception " << e.base().what();
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

            LOG_INFO << "Controller::print: menu to print, name :"
                    << menu.getValueOfName();

            HttpViewData data;
            data.insert("menu", menu);
            auto resp = HttpResponse::newHttpViewResponse("PrintView.csp", data);
            callback(resp);
        } catch (const DrogonDbException &e) {
            LOG_ERROR << "Controller::edit: exception " << e.base().what();
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

            LOG_INFO << "Controller::edit: menu to edit, name :"
                    << menu.getValueOfName();

            HttpViewData data;
            data.insert("menu", menu);
            auto resp = HttpResponse::newHttpViewResponse("EditView.csp", data);
            callback(resp);
        } catch (const DrogonDbException &e) {
            LOG_ERROR << "Controller::edit: exception " << e.base().what();
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
                LOG_INFO << "Controller::save: menu updated, id " << id;
                resp->addCookie("message",
                        "Modificado menu id "
                                + std::to_string(menu.getValueOfId()));
            } else {
                LOG_INFO << "Controller::save: menu not found, id " << id;
                resp->addCookie("message",
                        "Error, no se ha encontrado el menu id "
                                + req->getParameter("id"));
            }
        } else {
            mp.insert(menu);
            LOG_INFO << "Controller::save: menu added, id "
                    << menu.getValueOfId();
            resp->addCookie("message",
                    "Añadido nuevo menu id "
                            + std::to_string(menu.getValueOfId()));
        }
    } catch (const DrogonDbException &e) {
        LOG_ERROR << "Controller::update: exception " << e.base().what();
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
                LOG_INFO << "Controller::del: deleted " << id;
                resp->addCookie("message",
                        "Eliminado menu id " + req->getParameter("id"));
            } else {
                LOG_INFO << "Controller::del: not found menu " << id;
                resp->addCookie("message",
                        "Error, no se ha encontrado el menu id "
                                + req->getParameter("id"));
            }
        } catch (const DrogonDbException &e) {
            LOG_ERROR << "Controller::del: exception " << e.base().what();
            resp->addCookie("message", e.base().what());
        }
    } else {
        LOG_ERROR << "Controller::del: no id to delete, nothing to do";
        resp->addCookie("message",
                "Error, no se ha encontrado el menu id "
                        + req->getParameter("id"));
    }
    callback(resp);
}
