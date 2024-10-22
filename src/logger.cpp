#include "logger.h"

Logger::Logger() {
    line = 0;
    log.open("OhBoy_GBDoctor_Log.log", std::ios::trunc);
}

void Logger::write(std::string s) {
    ++line;
    write_buffer << s;
    if (line%100000 == 0) {
        if(log.is_open()) {
            log << write_buffer.str();
            write_buffer.clear();
            write_buffer.str(std::string());
        }
    }

}
void Logger::flush() {
    log << write_buffer.str();
    write_buffer.clear();
    write_buffer.str(std::string());
}


void Logger::close() {
    if (log.is_open()) log.close();
}


