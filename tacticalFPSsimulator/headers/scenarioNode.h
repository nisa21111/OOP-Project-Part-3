#pragma once

#include <string>
#include <vector>
#include <optional>
#include <functional>
#include <iostream>
#include <memory>

class ScenarioNode;

struct Choice {
	std::string label;
	std::string key;
	std::string nextNodeId;
	std::optional <std::string> eventTrigger;
};

enum class NodeType{ NARRATIVE, CHOICE, EVENT, OUTCOME};

class ScenarioNode {
private:
	std::string id_;
	std::string agentCalloutKey_;
	std::string narrativeText_;
	NodeType type_;
	std::vector<Choice> choices_;
	bool isTerminal_ = false;
	bool isWin_ = false;

public:
	ScenarioNode(std::string id, std::string calloutKey,
				std::string narrative, NodeType type)
		: id_(std::move(id))
		, agentCalloutKey_(std::move(calloutKey))
		, narrativeText_(std::move(narrative))
		, type_(type) { }

	~ScenarioNode() = default;
	ScenarioNode(const ScenarioNode&) = default;
	ScenarioNode& operator= (const ScenarioNode&) = default;
	ScenarioNode(ScenarioNode&&) = default;
	ScenarioNode& operator= (ScenarioNode&&) = default;

	void addChoice(Choice c) { choices_.push_back(std::move(c)); }
	void setTerminal(bool win) { isTerminal_ = true; isWin_ = win; }

	[[nodiscard]] const std::string& id() const { return id_; }
	[[nodiscard]] const std::string& calloutKey() const { return agentCalloutKey_; }
	[[nodiscard]] const std::string& narrative() const { return narrativeText_;}
	[[nodiscard]] NodeType type() const { return type_; }
	[[nodiscard]] const std::vector<Choice>& choices() const { return choices_; }
	[[nodiscard]] bool isTerminal() const { return isTerminal_; }
	[[nodiscard]] bool isWin() const { return isWin_; }

	void display() const {
		std::cout << narrativeText_ << "\n";
	}
};