#include "logger.h"
#include <iostream>

int main() {

    LOG_INFO("Game started");
    LOG_WARN("This is a warning");
    LOG_ERROR("Something went wrong");

    std::cout << "Logger test done\n";

    return 0;
}