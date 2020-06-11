#include "aog.h"
#include "models/Users.h"
#include <chrono>

using namespace std::chrono_literals;
using namespace drogon_model::sqlite3;

void aog::hello(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback,
                const std::string &userName) const
{
    auto resp = HttpResponse::newHttpResponse();
    resp->setBody("<P>" + userName + "</P>");
    callback(resp);
}

void aog::users(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback) const
{
    std::unordered_map<std::string, std::string> users;
#if 1
    Mapper<Users> mp(app().getDbClient());
    auto users_vector = mp.findAll();
    for (auto &&user : users_vector)
    {
        users[*user.getName()] = *user.getPass();
    }
#else
    if (app().getDbClient()->hasAvailableConnections())
    {
        try
        {
            auto r = app().getDbClient()->execSqlSync("select * from users");
            for (auto row : r)
            {
                users[row["name"].as<std::string>()] = row["pass"].as<std::string>();
            }
        }
        catch (const orm::DrogonDbException &e)
        {
            std::cerr << "error:" << e.base().what() << std::endl;
        }
    }
    else
    {
        LOG_ERROR << "db not available!";
    }
#endif
    HttpViewData data;
    data.insert("title", "ListUsers");
    data.insert("users", users);
    auto resp = HttpResponse::newHttpViewResponse("ListUsers.csp", data);
    callback(resp);
}
