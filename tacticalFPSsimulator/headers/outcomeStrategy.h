#pragma once
#include <string>
#include <random>

class IOutcomeStrategy {
public:
    virtual ~IOutcomeStrategy() = default;
    virtual bool resolve(int teamCoordination, int mapControl) const = 0;
    virtual std::string name() const = 0;
};

class AggressiveStrategy : public IOutcomeStrategy {
    mutable std::mt19937 rng_;
public:
    AggressiveStrategy();
    bool resolve(int teamCoordination, int mapControl) const override;
    std::string name() const override;
};

class CoordinatedStrategy : public IOutcomeStrategy {
    mutable std::mt19937 rng_;
public:
    CoordinatedStrategy();
    bool resolve(int teamCoordination, int mapControl) const override;
    std::string name() const override;
};

class BalancedStrategy : public IOutcomeStrategy {
    mutable std::mt19937 rng_;
public:
    BalancedStrategy();
    bool resolve(int teamCoordination, int mapControl) const override;
    std::string name() const override;
};