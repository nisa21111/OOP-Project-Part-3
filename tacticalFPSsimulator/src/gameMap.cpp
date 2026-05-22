#include "gameMap.h"
#include "exceptions.h"
#include "logger.h"
#include <fstream>
#include <sstream>

GameMap::GameMap(std::string name) : name_(std::move(name)) {}

void GameMap::addNode(std::shared_ptr<ScenarioNode> node) {
    if (node) {
        nodes_[node->id()] = node;
    }
}

void GameMap::setStartNode(const std::string& nodeId) { startNodeId_ = nodeId; }
void GameMap::setDefaultBackground(std::string path) { defaultBackground_ = std::move(path); }

std::shared_ptr<ScenarioNode> GameMap::getNode(const std::string& id) const {
    auto it = nodes_.find(id);
    if (it == nodes_.end() || it->second == nullptr) {
        throw ScenarioLoadException("node '" + id + "' in map '" + name_ + "'");
    }
    return it->second;
}

bool GameMap::hasNode(const std::string& id) const {
    return nodes_.count(id) > 0;
}

GameMap GameMap::loadFromFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) throw ScenarioLoadException(path);

    std::string mapName = path;
    auto slash = mapName.rfind('/');
    if (slash != std::string::npos) mapName = mapName.substr(slash + 1);
    auto dot = mapName.rfind('.');
    if (dot != std::string::npos) mapName = mapName.substr(0, dot);

    GameMap map(mapName);
    std::string line;

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream ss(line);
        std::string token;
        ss >> token;

        if (token == "NODE") {
            std::string id, calloutKey, narrative;
            ss >> id >> calloutKey;
            std::getline(ss, narrative);

            if (!narrative.empty() && narrative.front() == ' ')
                narrative = narrative.substr(1);
            if (narrative.size() >= 2 && narrative.front() == '"' && narrative.back() == '"')
                narrative = narrative.substr(1, narrative.size() - 2);

            map.addNode(std::make_shared<ScenarioNode>(id, calloutKey, narrative, NodeType::NARRATIVE));
            LOG_INFO("Loaded node: " + id);
        }
        else if (token == "BG") {
            std::string nodeId, imgPath;
            ss >> nodeId >> imgPath;

            auto it = map.nodes_.find(nodeId);
            if (it != map.nodes_.end() && it->second) {
                it->second->setBackgroundImage(imgPath);
            }
        }
        else if (token == "MAPBG") {
            std::string imgPath;
            ss >> imgPath;
            map.setDefaultBackground(imgPath);
        }
        else if (token == "CHOICE") {
            std::string nodeId, key, rest, labelText, nextId, eventType;
            ss >> nodeId >> key;
            std::getline(ss, rest);

            auto fq = rest.find('"');
            auto lq = rest.rfind('"');
            if (fq != std::string::npos && lq != fq) {
                labelText = rest.substr(fq + 1, lq - fq - 1);
                std::istringstream restss(rest.substr(lq + 1));
                restss >> nextId >> eventType;
            }
            else {
                labelText = "Option";
                std::istringstream restss(rest);
                restss >> nextId >> eventType;
            }

            auto it = map.nodes_.find(nodeId);
            if (it == map.nodes_.end() || !it->second) {
                LOG_WARN("CHOICE: nod necunoscut " + nodeId);
                continue;
            }

            Choice c;
            c.key = key;
            c.label = labelText;
            c.nextNodeId = nextId;
            if (!eventType.empty()) c.eventTrigger = eventType;

            it->second->addChoice(std::move(c));
        }
        else if (token == "TERMINAL") {
            std::string nodeId, outcome;
            ss >> nodeId >> outcome;

            auto it = map.nodes_.find(nodeId);
            if (it == map.nodes_.end() || !it->second) {
                LOG_WARN("TERMINAL: nod necunoscut " + nodeId);
                continue;
            }

            it->second->setTerminal(outcome == "win");
        }
        else if (token == "START") {
            std::string nodeId;
            ss >> nodeId;
            map.setStartNode(nodeId);
        }
    }

    if (map.startNodeId_.empty())
        throw ScenarioLoadException("Nu exista nod START in " + path);

    return map;
}