#include "gameMap.h"
#include <fstream>
#include <sstream>
#include <iostream>

GameMap::GameMap(std::string name) : name_(std::move(name)) {}

void GameMap::addNode(std::shared_ptr<ScenarioNode> node) {
    if (node) {
        nodes_[node->id()] = node;
    }
}

void GameMap::setStartNode(const std::string& nodeId) {
    startNodeId_ = nodeId;
}

void GameMap::setDefaultBackground(std::string path) {
    defaultBackground_ = std::move(path);
}

std::shared_ptr<ScenarioNode> GameMap::getNode(const std::string& id) const {
    auto it = nodes_.find(id);
    if (it == nodes_.end() || it->second == nullptr) {
        throw ScenarioLoadException("Nodul '" + id + "' nu a fost gasit in harta '" + name_ + "'");
    }
    return it->second;
}

bool GameMap::hasNode(const std::string& id) const {
    return nodes_.count(id) > 0;
}

GameMap GameMap::loadFromFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw ScenarioLoadException("Nu s-a putut deschide fisierul hartii la calea: " + path);
    }

    GameMap map("Incarcata_Din_Fisier");
    std::string line;

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::stringstream ss(line);
        std::string command;
        ss >> command;

        if (command == "MAP") {
            std::string mapName;
            ss >> mapName;
            map.name_ = mapName;
        }
        else if (command == "START") {
            std::string startId;
            ss >> startId;
            map.setStartNode(startId);
        }
        else if (command == "BG") {
            std::string bgPath;
            ss >> bgPath;
            map.setDefaultBackground(bgPath);
        }
        else if (command == "NODE") {
            std::string id, callout, typeStr;
            ss >> id >> callout >> typeStr;

            std::string narrative;
            std::getline(ss, narrative);
            if (!narrative.empty() && narrative[0] == ' ') {
                narrative = narrative.substr(1); // scoatem spatiul de dupa comanda
            }

            NodeType type = NodeType::NARRATIVE;
            if (typeStr == "CHOICE") type = NodeType::CHOICE;
            else if (typeStr == "EVENT") type = NodeType::EVENT;
            else if (typeStr == "OUTCOME") type = NodeType::OUTCOME;
            else if (typeStr == "TERMINAL") type = NodeType::TERMINAL;

            auto node = std::make_shared<ScenarioNode>(id, callout, narrative, type);
            map.addNode(node);
        }
        else if (command == "CHOICE") {
            std::string nodeId, key, targetId;
            ss >> nodeId >> key >> targetId;

            std::string label;
            std::getline(ss, label);
            if (!label.empty() && label[0] == ' ') label = label.substr(1);

            Choice c;
            c.key = key;
            c.label = label;
            c.nextNodeId = targetId;

            // Cautare nativa in unordered_map
            auto it = map.nodes_.find(nodeId);
            if (it != map.nodes_.end() && it->second) {
                it->second->addChoice(c);
            }
        }
        else if (command == "NODE_BG") {
            std::string nodeId, imgPath;
            ss >> nodeId >> imgPath;

            auto it = map.nodes_.find(nodeId);
            if (it != map.nodes_.end() && it->second) {
                it->second->setBackgroundImage(imgPath);
            }
        }
        else if (command == "TERMINAL") {
            std::string nodeId, status;
            ss >> nodeId >> status;

            auto it = map.nodes_.find(nodeId);
            if (it != map.nodes_.end() && it->second) {
                it->second->setTerminal(status == "WIN");
            }
        }
    }

    return map;
}