#include "scenarioNode.h"
#include <utility>

ScenarioNode::ScenarioNode(std::string id, std::string calloutKey,
    std::string narrative, NodeType type)
    : id_(std::move(id))
    , calloutKey_(std::move(calloutKey))
    , narrativeText_(std::move(narrative))
    , type_(type)
    , choices_()
    , isTerminal_(false)      // IMPORTANT: Previne gunoiul din memorie (Access Violation)
    , isWin_(false)           // IMPORTANT: Previne gunoiul din memorie
    , backgroundImage_("")
{
}

void ScenarioNode::addChoice(Choice c) {
    choices_.push_back(std::move(c));
}

void ScenarioNode::setTerminal(bool win) {
    isTerminal_ = true;
    isWin_ = win;
}

void ScenarioNode::setBackgroundImage(std::string path) {
    backgroundImage_ = std::move(path);
}