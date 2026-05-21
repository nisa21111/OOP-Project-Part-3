#include <iostream>
#include <memory>
#include "observer.h"
#include "gamesStats.h"

int main() {

    std::cout << "=== OBSERVER TEST ===\n\n";

    // =====================================================
    // 1. Setup Subject + Observer
    // =====================================================

    auto bus = std::make_shared<EventBus>();
    auto stats = std::make_shared<GameStats>();

    bus->subscribe(stats);

    // =====================================================
    // 2. Fire events manually
    // =====================================================

    bus->notify({ "SPIKE_DROPPED", "Spike dropped in mid" });
    bus->notify({ "TEAM_IGNORED", "Teammates went A instead of B" });
    bus->notify({ "ROTATE", "Player rotated mid-round" });
    bus->notify({ "SPIKE_DROPPED", "Spike dropped again" });

    // =====================================================
    // 3. Simulate round end
    // =====================================================

    stats->recordRound(true);
    stats->recordRound(false);
    stats->recordRound(true);

    // =====================================================
    // 4. Print results
    // =====================================================

    std::cout << "\n--- STATS SUMMARY ---\n";
    stats->printSummary();

    std::cout << "\n--- EVENT LOG ---\n";
    stats->printEventLog();

    std::cout << "\n=== TEST DONE ===\n";

    return 0;
}