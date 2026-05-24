#include "agent.h"
#include <iostream>

Agent::Agent(std::string name, AgentRole role, std::string portraitPath)
    : name_(std::move(name))
    , role_(std::move(role))
    , portraitPath_(std::move(portraitPath))
{
}

// brimmy

Brimstone::Brimstone()
    : Agent("Brimstone", AgentRole::CONTROLLER, "data/images/brimstone.png")
{
}

std::string Brimstone::roleStr() const { return "Controller"; }

std::string Brimstone::callout(const std::string& situation) const {
    if (situation == "start")
        return "Alright team, listen up. Where are we going?";
    if (situation == "spike_dropped")
        return "Spike's been dropped! Somebody pick that up NOW!";
    if (situation == "rotate")
        return "We're losing this site - rotate mid, let's go!";
    if (situation == "team_ignored")
        return "...They didn't listen. Classic.";
    if (situation == "win")
        return "GG. Good comms, good execution. That's how we do it.";
    if (situation == "lose")
        return "Tough round. Reset and regroup.";
    return "Brimstone: " + situation;
}

std::vector<std::string> Brimstone::abilities() const {
    return { "Stim Beacon", "Incendiary", "Sky Smoke", "Orbital Strike" };
}

// jett

Jett::Jett()
    : Agent("Jett", AgentRole::DUELIST, "data/images/jett.png")
{
}

std::string Jett::roleStr() const { return "Duelist"; }

std::string Jett::callout(const std::string& situation) const {
    if (situation == "start")        return "I'll entry, just follow me in.";
    if (situation == "spike_dropped") return "Leave the spike, I'll frag out first!";
    if (situation == "rotate")        return "Rotation? Already there. *whoosh*";
    if (situation == "team_ignored")  return "Ugh, fine. Going solo.";
    if (situation == "win")           return "Easy. Let's run it back.";
    if (situation == "lose")          return "Whatever. Next round.";
    return "Jett: " + situation;
}

std::vector<std::string> Jett::abilities() const {
    return { "Cloudburst", "Updraft", "Tailwind", "Blade Storm" };
}

// sage

Sage::Sage()
    : Agent("Sage", AgentRole::SENTINEL, "data/images/sage.png")
{
}

std::string Sage::roleStr() const { return "Sentinel"; }

std::string Sage::callout(const std::string& situation) const {
    if (situation == "start")         return "I'll hold flank. Be safe, everyone.";
    if (situation == "spike_dropped") return "I can res - but someone plant first!";
    if (situation == "rotate")        return "Walling off the flank, then rotating.";
    if (situation == "team_ignored")  return "...I'll pray for them.";
    if (situation == "win")           return "Healing done. Victory achieved.";
    if (situation == "lose")          return "Next time, listen to Sage.";
    return "Sage: " + situation;
}

std::vector<std::string> Sage::abilities() const {
    return { "Barrier Orb", "Slow Orb", "Healing Orb", "Resurrection" };
}

// sova

Sova::Sova()
    : Agent("Sova", AgentRole::INITIATOR, "data/images/sova.png")
{
}

std::string Sova::roleStr() const { return "Initiator"; }

std::string Sova::callout(const std::string& situation) const {
    if (situation == "start")         return "Dart first, then we push. Trust the info.";
    if (situation == "spike_dropped") return "Scanning for enemy location...";
    if (situation == "rotate")        return "I have info on mid. Rotate now.";
    if (situation == "team_ignored")  return "My dart showed nothing there. Nobody listened.";
    if (situation == "win")           return "Information wins games. Proved it again.";
    if (situation == "lose")          return "Insufficient data. Recalibrating.";
    return "Sova: " + situation;
}

std::vector<std::string> Sova::abilities() const {
    return { "Owl Drone", "Shock Bolt", "Recon Bolt", "Hunter's Fury" };
}
