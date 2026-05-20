#pragma once
#include <stdexcept>
#include <string>

class GameException : public std::exception {
    std::string msg_;
public:
    explicit GameException(std::string msg) : msg_(std::move(msg)) {}
    const char* what() const noexcept override { return msg_.c_str(); }
};

class ScenarioLoadException : public GameException {
public:
    explicit ScenarioLoadException(const std::string& file)
        : GameException("Failed to load scenario file: " + file) {
    }
};

class InvalidChoiceException : public GameException {
public:
    explicit InvalidChoiceException(const std::string& choice)
        : GameException("Invalid choice: " + choice) {
    }
};

class AgentNotFoundException : public GameException {
public:
    explicit AgentNotFoundException(const std::string& name)
        : GameException("Agent not found: " + name) {
    }
};