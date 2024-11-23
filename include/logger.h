//
// Created by ADMIN on 18/10/2024.
//

#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <spdlog/spdlog.h>

class Logger {
private:
    std::shared_ptr<spdlog::logger> logger;
public:
    Logger(std::string logger_name);
    std::shared_ptr<spdlog::logger> get_logger();
};
#endif //LOGGER_H
