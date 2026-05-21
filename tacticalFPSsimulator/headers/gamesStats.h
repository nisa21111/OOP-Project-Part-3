#pragma once
#include "Observer.h"
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>

class GameStats : public IObserver {
    int roundsPlayed_ = 0;
    int roundsWon_ = 0;
    int spikeDrops_ = 0;
    int teamIgnored_ = 0;
    int rotations_ = 0;
    std::vector<std::string> eventLog_;

public:
    GameStats() = default;

    // IObserver
    void onEvent(const GameEvent& event) override {
        eventLog_.push_back("[" + event.type + "] " + event.detail);
        if (event.type == "ROUND_END") {
            ++roundsPlayed_;
            if (event.detail == "win") ++roundsWon_;
        }
        else if (event.type == "SPIKE_DROPPED") ++spikeDrops_;
        else if (event.type == "TEAM_IGNORED")  ++teamIgnored_;
        else if (event.type == "ROTATE")        ++rotations_;
    }

    void recordRound(bool win) {
        onEvent({ "ROUND_END", win ? "win" : "lose" });
    }

    void printSummary() const {
        std::cout << "       SESSION SUMMARY        \n";
        std::cout << " Rounds played : " << std::setw(3) << roundsPlayed_
            << "          \n";
        std::cout << " Rounds won    : " << std::setw(3) << roundsWon_
            << "          \n";
        std::cout << " Spike drops   : " << std::setw(3) << spikeDrops_
            << "          \n";
        std::cout << " Team ignored  : " << std::setw(3) << teamIgnored_
            << "          \n";
        std::cout << " Rotations     : " << std::setw(3) << rotations_
            << "          \n";
    }

    void printEventLog() const {
        std::cout << "\n--- Event Log ---\n";
        for (auto& e : eventLog_) std::cout << "  " << e << "\n";
        std::cout << "-----------------\n";
    }

    [[nodiscard]] int roundsPlayed() const { return roundsPlayed_; }
    [[nodiscard]] int roundsWon()    const { return roundsWon_; }
};