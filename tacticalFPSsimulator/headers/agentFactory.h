#pragma once
#include "agent.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <functional>
#include <vector>

class AgentFactory {
    using Creator = std::function<std::unique_ptr<Agent>()>;
    std::unordered_map<std::string, Creator> registry_;

    AgentFactory();

public:
    static AgentFactory& instance();

    AgentFactory(const AgentFactory&) = delete;
    AgentFactory& operator=(const AgentFactory&) = delete;
    AgentFactory(AgentFactory&&) = delete;
    AgentFactory& operator=(AgentFactory&&) = delete;

    [[nodiscard]] std::unique_ptr<Agent>     create(const std::string& agentName) const;
    [[nodiscard]] std::vector<std::string>   available() const;
};
