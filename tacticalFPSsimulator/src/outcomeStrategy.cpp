#include "outcomeStrategy.h"
#include <chrono>

static unsigned seed() {
    return static_cast<unsigned>(
        std::chrono::steady_clock::now().time_since_epoch().count());
}

//agro

AggressiveStrategy::AggressiveStrategy() : rng_(seed()) {}

bool AggressiveStrategy::resolve(int teamCoordination, int mapControl) const {
    std::uniform_int_distribution<int> dist(0, 15);
    return (mapControl * 2 + teamCoordination) + dist(rng_) > 12;
}

std::string AggressiveStrategy::name() const { return "Aggressive"; }

// coord (nu s-a intamplat vreodata intr-un meci adv de valo fmm)

CoordinatedStrategy::CoordinatedStrategy() : rng_(seed() + 1) {}

bool CoordinatedStrategy::resolve(int teamCoordination, int mapControl) const {
    std::uniform_int_distribution<int> dist(0, 10);
    return (teamCoordination * 2 + mapControl) + dist(rng_) > 10;
}

std::string CoordinatedStrategy::name() const { return "Coordinated"; }

// balanced

BalancedStrategy::BalancedStrategy() : rng_(seed() + 2) {}

bool BalancedStrategy::resolve(int teamCoordination, int mapControl) const {
    std::uniform_int_distribution<int> dist(0, 10);
    return (teamCoordination + mapControl) + dist(rng_) > 8;
}

std::string BalancedStrategy::name() const { return "Balanced"; }