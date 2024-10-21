#include "logger.h"
#include <iostream>
Logger::Logger() {
    line = 0;
    log.open("OhBoy_GBDoctor_Log.log", std::ios::trunc);
}

void Logger::write(std::string s) {
    // if(log.is_open()) {
    //     log << s;
    // }
    // std::cout<<++line<<"\n";
    ++line;
}

void Logger::close() {
    if (log.is_open()) log.close();
}


