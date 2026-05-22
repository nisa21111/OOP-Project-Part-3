#pragma once
#include "scenarioNode.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <stdexcept>

// Clasa de exceptie pentru erorile de incarcare a scenariilor
class ScenarioLoadException : public std::runtime_error {
public:
    explicit ScenarioLoadException(const std::string& message)
        : std::runtime_error("ScenarioLoadException: " + message) {}
};

class GameMap {
    std::string              name_;
    std::string              startNodeId_;
    std::string              defaultBackground_;

    // Înlocuit Repository cu unordered_map nativ pentru stabilitate absoluta a memoriei
    std::unordered_map<std::string, std::shared_ptr<ScenarioNode>> nodes_;

public:
    explicit GameMap(std::string name);

    ~GameMap() = default;
    GameMap(const GameMap&) = default;
    GameMap& operator=(const GameMap&) = default;
    GameMap(GameMap&&) = default;
    GameMap& operator=(GameMap&&) = default;

    void addNode(std::shared_ptr<ScenarioNode> node);
    void setStartNode(const std::string& nodeId);
    void setDefaultBackground(std::string path);

    [[nodiscard]] const std::string& name()              const { return name_; }
    [[nodiscard]] std::string        id()                const { return name_; }
    [[nodiscard]] const std::string& startNodeId()       const { return startNodeId_; }
    [[nodiscard]] const std::string& defaultBackground() const { return defaultBackground_; }

    [[nodiscard]] std::shared_ptr<ScenarioNode> getNode(const std::string& id) const;
    [[nodiscard]] bool                          hasNode(const std::string& id) const;

    static GameMap loadFromFile(const std::string& path);
};