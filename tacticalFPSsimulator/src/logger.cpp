#include "logger.h"

#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>

Logger::Logger() {
    file_.open("game.log", std::ios::app);
}

Logger& Logger::instance() {
    static Logger inst;
    return inst;
}

std::string Logger::levelToString(LogLevel level) {
    switch (level) {
    case LogLevel::INFO: return "INFO";
    case LogLevel::WARNING: return "WARN";
    case LogLevel::ERROR: return "ERROR";
    }
    return "UNKNOWN";
}

std::string Logger::timestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);

    std::tm tm{};
    localtime_s(&tm, &time);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%H:%M:%S");
    return oss.str();
}

void Logger::log(LogLevel level, const std::string& msg) {
    std::lock_guard<std::mutex> lock(mtx_);

    std::string line =
        "[" + timestamp() + "]" +
        "[" + levelToString(level) + "] " +
        msg;

    std::cout << line << "\n";

    if (file_.is_open()) {
        file_ << line << "\n";
    }
}

void Logger::info(const std::string& msg) {
    log(LogLevel::INFO, msg);
}

void Logger::warning(const std::string& msg) {
    log(LogLevel::WARNING, msg);
}

void Logger::error(const std::string& msg) {
    log(LogLevel::ERROR, msg);
}