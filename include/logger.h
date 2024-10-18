//
// Created by ADMIN on 18/10/2024.
//

#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>

class Logger {
private:
    std::ofstream log;
public:
    Logger();
    void write(std::string s);
    void close();
};
#endif //LOGGER_H
