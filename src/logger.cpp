#include "logger.h"
#include "spdlog/async.h"
#include "spdlog/sinks/basic_file_sink.h"

Logger::Logger(std::string logger_name) {
    logger = spdlog::basic_logger_mt<spdlog::async_factory>(logger_name, "ohboy_log.txt", true);
    logger->set_level(spdlog::level::debug);
}

std::shared_ptr<spdlog::logger> Logger::get_logger() {
    return logger;
}



