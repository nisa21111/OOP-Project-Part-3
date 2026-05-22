#include "agentFactory.h"
#include "exceptions.h"
#include <algorithm>

AgentFactory::AgentFactory() {
    registry_["brimstone"] = []() { return std::make_unique<Brimstone>(); };
    registry_["jett"] = []() { return std::make_unique<Jett>(); };
    registry_["sage"] = []() { return std::make_unique<Sage>(); };
    registry_["sova"] = []() { return std::make_unique<Sova>(); };
}

AgentFactory& AgentFactory::instance() {
    static AgentFactory inst;
    return inst;
}

std::unique_ptr<Agent> AgentFactory::create(const std::string& agentName) const {
    std::string key = agentName;
    std::transform(key.begin(), key.end(), key.begin(), ::tolower);
    auto it = registry_.find(key);
    if (it == registry_.end())
        throw AgentNotFoundException(agentName);
    return it->second();
}

std::vector<std::string> AgentFactory::available() const {
    std::vector<std::string> names;
    names.reserve(registry_.size());
    for (auto& [k, v] : registry_) names.push_back(k);
    return names;
}