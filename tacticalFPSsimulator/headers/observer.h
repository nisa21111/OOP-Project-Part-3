#pragma once
#include <vector>
#include <memory>
#include <string>
#include <algorithm>

// Observer pattern
struct GameEvent {
    std::string type;   // "SPIKE_DROPPED", "TEAM_IGNORED", "ROTATE", "ROUND_END" ...
    std::string detail;
};

class IObserver {
public:
    virtual ~IObserver() = default;
    virtual void onEvent(const GameEvent& event) = 0;
};

class ISubject {
public:
    virtual ~ISubject() = default;
    virtual void subscribe(std::weak_ptr<IObserver> obs) = 0;
    virtual void notify(const GameEvent& event) = 0;
};

class EventBus : public ISubject {
    std::vector<std::weak_ptr<IObserver>> observers_;

public:
    void subscribe(std::weak_ptr<IObserver> obs) override {
        observers_.push_back(std::move(obs));
    }

    void notify(const GameEvent& event) override {
        // Remove expired weak_ptrs, notify live ones
        auto it = std::remove_if(observers_.begin(), observers_.end(),
            [](const std::weak_ptr<IObserver>& wp) { return wp.expired(); });
        observers_.erase(it, observers_.end());

        for (auto& wp : observers_) {
            if (auto sp = wp.lock()) {
                sp->onEvent(event);
            }
        }
    }
};