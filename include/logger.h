//
// Created by ADMIN on 18/10/2024.
//

#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>
#include <sstream>

class Logger {
private:
    std::ofstream log;
    std::stringstream write_buffer;
public:
    int line;
    Logger();
    void write(std::string s);
    void close();
    void flush();
};
#endif //LOGGER_H
