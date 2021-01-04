#include <iostream>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <drogon/HttpController.h>
#include "filters/DigestAuthFilter.h"
using namespace spdlog;
using namespace drogon;

void init_log(const std::string &appname)
{
    auto console_sink = std::make_shared<sinks::stdout_color_sink_mt>();
    auto file_sink = std::make_shared<sinks::basic_file_sink_mt>("/tmp/friasApp.log", false);
    auto l = std::make_shared<logger>(appname, sinks_init_list{console_sink, file_sink});
    set_default_logger(l);
    spdlog::flush_every(std::chrono::seconds(3));
}

void print_handlers()
{
    spdlog::info("|||||||||||||||||||||||||||HANDLERS||||||||||||||||||||||||||");
    auto handlerInfo = app().getHandlersInfo();
    for (auto &info : handlerInfo)
    {
        switch (std::get<1>(info))
        {
        case Get:
            spdlog::info("{0} {1} {2}", std::get<0>(info), " (GET) ", std::get<2>(info));
            break;
        case Post:
            spdlog::info("{0} {1} {2}", std::get<0>(info), " (POST) ", std::get<2>(info));
            break;
        case Delete:
            spdlog::info("{0} {1} {2}", std::get<0>(info), " (DELETE) ", std::get<2>(info));
            break;
        case Put:
            spdlog::info("{0} {1} {2}", std::get<0>(info), " (PUT) ", std::get<2>(info));
            break;
        case Options:
            spdlog::info("{0} {1} {2}", std::get<0>(info), " (OPTIONS) ", std::get<2>(info));
            break;
        case Head:
            spdlog::info("{0} {1} {2}", std::get<0>(info), " (Head) ", std::get<2>(info));
            break;
        default:
            break;
        }
    }
    spdlog::info("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||");
}

int main(int argc, char *argv[])
{
    init_log(argv[0]);
    std::map<std::string, std::string> config_credentials;
    std::string realm("drogonRealm");
    std::string opaque("drogonOpaque");
    std::string config_file = "config.json";
    if (argc > 1)
    {
        config_file = argv[1];
    }
    app().loadConfigFile(config_file);
    auto &json = app().getCustomConfig();
    if (json.empty())
    {
        std::cout << "empty custom config!" << std::endl;
    }
    else
    {
        if (!json["realm"].empty())
        {
            realm = json["realm"].asString();
        }
        if (!json["opaque"].empty())
        {
            opaque = json["opaque"].asString();
        }
        for (auto &&i : json["credentials"])
        {
            config_credentials[i["user"].asString()] = i["password"].asString();
        }
    }

    auto auth_filter = std::make_shared<DigestAuthFilter>(config_credentials, realm, opaque);
    app().registerFilter(auth_filter);
 
//    app().registerBeginningAdvice(
//        []() { LOG_DEBUG << "Event loop is running!"; });
//    app().registerNewConnectionAdvice([](const trantor::InetAddress &peer,
//                                         const trantor::InetAddress &local) {
//        LOG_DEBUG << "New connection: " << peer.toIpPort() << "-->"
//                  << local.toIpPort();
//        return true;
//    });
//    app().registerPreRoutingAdvice([](const drogon::HttpRequestPtr &req,
//                                      drogon::AdviceCallback &&acb,
//                                      drogon::AdviceChainCallback &&accb) {
//        LOG_DEBUG << "preRouting1";
//        accb();
//    });
//    app().registerPostRoutingAdvice([](const drogon::HttpRequestPtr &req,
//                                       drogon::AdviceCallback &&acb,
//                                       drogon::AdviceChainCallback &&accb) {
//        LOG_DEBUG << "postRouting1";
//        LOG_DEBUG << "Matched path=" << req->matchedPathPatternData();
//        for (auto &cookie : req->cookies())
//        {
//            LOG_DEBUG << "cookie: " << cookie.first << "=" << cookie.second;
//        }
//        accb();
//    });
//    app().registerPreHandlingAdvice([](const drogon::HttpRequestPtr &req,
//                                       drogon::AdviceCallback &&acb,
//                                       drogon::AdviceChainCallback &&accb) {
//        LOG_DEBUG << "preHandling1";
//        accb();
//    });
//    app().registerPostHandlingAdvice([](const drogon::HttpRequestPtr &,
//                                        const drogon::HttpResponsePtr &resp) {
//        LOG_DEBUG << "postHandling1";
//        resp->addHeader("Access-Control-Allow-Origin", "*");
//    });
//    app().registerPreRoutingAdvice([](const drogon::HttpRequestPtr &req) {
//        LOG_DEBUG << "preRouting observer";
//    });
//    app().registerPostRoutingAdvice([](const drogon::HttpRequestPtr &req) {
//        LOG_DEBUG << "postRouting observer";
//    });
//    app().registerPreHandlingAdvice([](const drogon::HttpRequestPtr &req) {
//        LOG_DEBUG << "preHanding observer";
//    });

    print_handlers();
    spdlog::info("Date: {0}", std::string{drogon::utils::getHttpFullDate(
            trantor::Date::now())});
    app().run();
}
