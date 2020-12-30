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
    menu.setFirst7(req->getParameter("first7"));
    menu.setFirst8(req->getParameter("first8"));
    menu.setFirst9(req->getParameter("first9"));
    menu.setFirst10(req->getParameter("second10"));
    menu.setSecond1(req->getParameter("second1"));
    menu.setSecond2(req->getParameter("second2"));
    menu.setSecond3(req->getParameter("second3"));
    menu.setSecond4(req->getParameter("second4"));
    menu.setSecond5(req->getParameter("second5"));
    menu.setSecond6(req->getParameter("second6"));
    menu.setSecond7(req->getParameter("second7"));
    menu.setSecond8(req->getParameter("second8"));
    menu.setSecond9(req->getParameter("second9"));
    menu.setSecond10(req->getParameter("second10"));
    menu.setComment1(req->getParameter("comment1"));
    menu.setComment2(req->getParameter("comment2"));
    menu.setComment3(req->getParameter("comment3"));
    int enabled = 0;
    try {
        std::string::size_type sz;
        enabled = std::stoi(req->getParameter("enabled"), &sz);
    } catch (...) {
    }
    menu.setEnabled(enabled);
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
        LOG_ERROR << e.base().what();
    }
    callback(resp);
}

void Controller::_admin(const HttpRequestPtr &req,
        std::function<void(const HttpResponsePtr&)> &&callback,
        const std::string &errorMsg) const {
    auto resp = HttpResponse::newHttpResponse();
    try {
        HttpViewData data;
        data.insert("title", "Admin");
        if (!errorMsg.empty()) {
            data.insert("errorMsg", errorMsg);
        }

        Mapper<Menu> mpMenu(app().getDbClient());
        auto menus = mpMenu.findAll();
        data.insert("menus", menus);

        resp = HttpResponse::newHttpViewResponse("AdminView.csp", data);
    } catch (const DrogonDbException &e) {
        resp->setStatusCode(HttpStatusCode::k500InternalServerError);
        LOG_ERROR << e.base().what();
    }
    callback(resp);
}

void Controller::admin(const HttpRequestPtr &req,
        std::function<void(const HttpResponsePtr&)> &&callback) const {
    _admin(req, std::move(callback));
}

void Controller::add(const HttpRequestPtr &req,
        std::function<void(const HttpResponsePtr&)> &&callback) const {
    std::string errorMsg;
    try {
        Menu menu;
        menuFromReq(req, menu);

        Mapper<Menu> mp(app().getDbClient());
        mp.insert(menu);

        LOG_INFO << "Controller::add: new menu inserted "
                << menu.getValueOfName();
    } catch (const DrogonDbException &e) {
        LOG_ERROR << "Controller::add: exception " << e.base().what();
        errorMsg = e.base().what();
    }
    _admin(req, std::move(callback), errorMsg);
}

void Controller::del(const HttpRequestPtr &req,
        std::function<void(const HttpResponsePtr&)> &&callback) const {
    std::string errorMsg;
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
            } else {
                LOG_INFO << "Controller::del: not found menu " << id;
                errorMsg = "menu id " + req->getParameter("id") + " not found";
            }
        } catch (const DrogonDbException &e) {
            LOG_ERROR << "Controller::del: exception " << e.base().what();
            errorMsg = e.base().what();
        }
    } else {
        errorMsg = "menu id " + req->getParameter("id") + " not found";
    }
    _admin(req, std::move(callback), errorMsg);
}

void Controller::update(const HttpRequestPtr &req,
        std::function<void(const HttpResponsePtr&)> &&callback) const {
    std::string errorMsg;
    int id = 0;
    try {
        std::string::size_type sz;
        id = std::stoi(req->getParameter("id"), &sz);
    } catch (...) {
    }
    if (id) {
        try {
            Menu menu;
            menu.setId(id);
            menuFromReq(req, menu);

            Mapper<Menu> mp(app().getDbClient());
            if (mp.update(menu)) {
                LOG_INFO << "Controller::update: menu updated, id " << id;
            } else {
                LOG_INFO << "Controller::update: menu not found, id " << id;
                errorMsg = "menu id " + req->getParameter("id") + " not found";
            }
        } catch (const DrogonDbException &e) {
            LOG_ERROR << "Controller::update: exception " << e.base().what();
            errorMsg = e.base().what();
        }
    } else {
        errorMsg = "menu id " + req->getParameter("id") + " not found";
    }
    _admin(req, std::move(callback), errorMsg);
}
