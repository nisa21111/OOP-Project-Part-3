#pragma once

#include <string>
#include <fstream>
#include <mutex>

enum class LogLevel {
    INFO,
    WARNING,
    ERROR
};

class Logger {
private:
    std::ofstream file_;
    std::mutex mtx_;

    Logger();

    std::string levelToString(LogLevel level);
    std::string timestamp();

public:
    static Logger& instance();

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void log(LogLevel level, const std::string& msg);

    void info(const std::string& msg);
    void warning(const std::string& msg);
    void error(const std::string& msg);
};

#define LOG_INFO(msg)    Logger::instance().info(msg)
#define LOG_WARN(msg)    Logger::instance().warning(msg)
#define LOG_ERROR(msg)   Logger::instance().error(msg)