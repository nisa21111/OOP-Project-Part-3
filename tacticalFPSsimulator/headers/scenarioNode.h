#pragma once
#include <string>
#include <vector>
#include <optional>

struct Choice {
    std::string key;
    std::string label;
    std::string nextNodeId;
    std::optional<std::string> eventTrigger;
};

enum class NodeType { NARRATIVE, CHOICE, EVENT, OUTCOME, TERMINAL };

class ScenarioNode {
    std::string          id_;
    std::string          calloutKey_;
    std::string          narrativeText_;
    NodeType             type_;
    std::vector<Choice>  choices_;
    bool                 isTerminal_ = false;
    bool                 isWin_ = false;
    std::string          backgroundImage_;

public:
    ScenarioNode(std::string id, std::string calloutKey,
        std::string narrative, NodeType type);

    ~ScenarioNode() = default;

    void addChoice(Choice c);
    void setTerminal(bool win);
    void setBackgroundImage(std::string path);

    [[nodiscard]] const std::string& id()              const { return id_; }
    [[nodiscard]] const std::string& calloutKey()      const { return calloutKey_; }
    [[nodiscard]] const std::string& narrative()       const { return narrativeText_; }
    [[nodiscard]] NodeType                   type()            const { return type_; }
    [[nodiscard]] const std::vector<Choice>& choices()         const { return choices_; }
    [[nodiscard]] bool                       isTerminal()      const { return isTerminal_; }
    [[nodiscard]] bool                       isWin()           const { return isWin_; }
    [[nodiscard]] const std::string& backgroundImage() const { return backgroundImage_; }
    [[nodiscard]] std::string                display_id()      const { return id_; }
};