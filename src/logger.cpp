#include "logger.h"

Logger::Logger() {
    log.open("OhBoy_GBDoctor_Log.log", std::ios::trunc);
}

void Logger::write(std::string s) {
    if(log.is_open()) {
        log << s;
    }
}

void Logger::close() {
    if (log.is_open()) log.close();
}


