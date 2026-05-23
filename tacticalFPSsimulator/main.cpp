#include "ui/application.h"
#include "logger.h"
#include "exceptions.h"
#include <iostream>

int main() {
    try {
        Application app;
        app.run();
    }
    catch (const GameException& e) {
        LOG_ERROR(std::string("Fatal GameException: ") + e.what());
        std::cerr << "[FATAL] " << e.what() << "\n";
        return 1;
    }
    catch (const std::exception& e) {
        LOG_ERROR(std::string("Fatal std::exception: ") + e.what());
        std::cerr << "[FATAL] " << e.what() << "\n";
        return 1;
    }
    return 0;
}
