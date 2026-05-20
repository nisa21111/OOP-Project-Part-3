#pragma once

#include <string>
#include <vector>

enum class AgentRole {
	CONTROLLER,
	DUELIST,
	INITIATOR,
	SENTINEL
};

class Agent {
	std::string name_;
	AgentRole role_;
	std::string portraitAscii_;

protected:
	Agent(std::string name,
		AgentRole role,
		std::string portrait);

public:
	virtual ~Agent() = default; 

	Agent(const Agent&) = default;
	Agent& operator= (const Agent&) = default;
	Agent(Agent&&) = default;
	Agent& operator=(Agent&&) = default;

	[[nodiscard]]
	const std::string& name() const;

	[[nodiscard]]
	AgentRole role() const;

	[[nodiscard]]
	std::string id() const;

	[[nodiscard]]
	std::string portrait() const;

	void display() const;

	virtual std::string roleStr() const = 0;

	virtual std::string callout(const std::string& situation) const = 0;

	virtual std::vector<std::string> abilities() const = 0;
};

class Brimstone : public Agent {
public:
	Brimstone();

	std::string roleStr() const override;

	std::string callout(const std::string& situation) const override;

	std::vector<std::string> abilities() const override;
};

class Jett : public Agent {
public:
	Jett();

	std::string roleStr() const override;

	std::string callout(const std::string& situation) const override;

	std::vector<std::string> abilities() const override;
};

class Sage : public Agent {
public:
	Sage();

	std::string roleStr() const override;

	std::string callout(const std::string& situation) const override;

	std::vector<std::string> abilities() const override;
};

class Sova : public Agent {
public:
	Sova();

	std::string roleStr() const override;

	std::string callout(const std::string& situation) const override;

	std::vector<std::string> abilities() const override;
};