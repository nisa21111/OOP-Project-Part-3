#pragma once
#include <string>
#include <vector>

enum class AgentRole { CONTROLLER, DUELIST, INITIATOR, SENTINEL };

class Agent {
    std::string name_;
    AgentRole   role_;
    std::string portraitPath_;   // CALE PNG !!!!!!

protected:
    Agent(std::string name, AgentRole role, std::string portraitPath);

public:
    virtual ~Agent() = default;
    Agent(const Agent&) = default;
    Agent& operator=(const Agent&) = default;
    Agent(Agent&&) = default;
    Agent& operator=(Agent&&) = default;

    [[nodiscard]] const std::string& name()        const { return name_; }
    [[nodiscard]] AgentRole          role()        const { return role_; }
    [[nodiscard]] const std::string& portraitPath() const { return portraitPath_; }
    [[nodiscard]] std::string        id()          const { return name_; }

    virtual std::string roleStr()                            const = 0;
    virtual std::string callout(const std::string& situation) const = 0;
    virtual std::vector<std::string> abilities()             const = 0;

    void display() const;
};

class Brimstone : public Agent {
public:
    Brimstone();
    std::string roleStr()                             const override;
    std::string callout(const std::string& situation) const override;
    std::vector<std::string> abilities()              const override;
};

class Jett : public Agent {
public:
    Jett();
    std::string roleStr()                             const override;
    std::string callout(const std::string& situation) const override;
    std::vector<std::string> abilities()              const override;
};

class Sage : public Agent {
public:
    Sage();
    std::string roleStr()                             const override;
    std::string callout(const std::string& situation) const override;
    std::vector<std::string> abilities()              const override;
};

class Sova : public Agent {
public:
    Sova();
    std::string roleStr()                             const override;
    std::string callout(const std::string& situation) const override;
    std::vector<std::string> abilities()              const override;
};
