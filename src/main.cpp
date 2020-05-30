#include <iostream>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "exampleclass.hpp"

void init_log(const std::string& appname){
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("/tmp/friasApp.log", true);
    auto l = std::make_shared<spdlog::logger>(appname, spdlog::sinks_init_list{console_sink, file_sink});
    spdlog::set_default_logger(l);
}

int main(int argc, char* argv[]) {
    init_log(argv[0]);
    exampleclass c;
    int i = 10;
    do{
        spdlog::info("AOG! {}", i--);
    }while(i != 0);
}
