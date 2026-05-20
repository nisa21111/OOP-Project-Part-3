#include "agent.h"

#include <iostream>
#include <utility>

Agent::Agent (std::string name,
			AgentRole role,
			std::string portrait)
	: name_(std::move(name)),
	role_(role),
	portraitAscii_(std::move(portrait)){}

const std::string& Agent::name() const {
	return name_;
}

AgentRole Agent::role() const {
	return role_;
}

std::string Agent::id() const {
	return name_;
}

std::string Agent::portrait() const {
	return portraitAscii_;
}

void Agent::display() const {
	std::cout << portraitAscii_ << "\n";
	std::cout << "[ "
		<< name_
		<< " | "
		<< roleStr()
		<< " ]\n";
}

//BRIMSTONE

static constexpr const char* BRIMSTONE_PORTRAIT = R"(

      ____
     /o  o\
    | ---- |
    | IGL  |
     \____/
   [BRIMSTONE]

)";

Brimstone::Brimstone()
	: Agent("Brimstone",
			AgentRole::CONTROLLER,
			BRIMSTONE_PORTRAIT){}

std::string Brimstone::roleStr() const {
	return "Controller";
}

std::string Brimstone::callout(const std::string& situation) const {
	if (situation == "start")
		return "Okay kids, we've got company. Pretend you all get along. Where are we going?";

	if (situation == "rotate")
		return "Rotate fast. Don't give them time";

	if (situation == "spike_dropped")
		return "Spike down! Recover it fast!";

	if (situation == "win")
		return "Good execute. That's how we play.";

	if (situation == "lose")
		return "Reset mentality. Next round is ours.";

	return "Brimstone: " + situation;
}

std::vector<std::string> Brimstone::abilities() const {
	return {
		"Stim Beacon",
		"Incendiary",
		"Sky smoke",
		"Orbital Strike"
	};
}

//JETT

static constexpr const char* JETT_PORTRAIT = R"(

     /\  /\
    (  \/  )
    | dash |
     \    /
   [ JETT ]

)";

Jett::Jett()
	:Agent("Jett",
			AgentRole::DUELIST,
			JETT_PORTRAIT){}

std::string Jett::roleStr() const {
	return "Duelist";
}

std::string Jett::callout(const std::string& situation) const {
	if (situation == "start")
		return "Alright, get on site, plant spike, protect spike, go home. Easy peasy.";

	if (situation == "spike_dropped")
		return "Our spike carrier is killed.";

	if (situation == "win")
		return "Sweet, that went well!";

	if (situation == "lose")
		return "Unlucky.";

	if (situation == "rotate")
		return "Already rotating.";

	return "Jett: " + situation;
}

std::vector<std::string> Jett::abilities() const {
	return {
		"Cloudburst",
		"Updraft",
		"Tailwind",
		"Bladestorm"
	};
}

//SAGE

static constexpr const char* SAGE_PORTRAIT = R"(

     (   )
    | heal |
    | wall |
     \   /
   [ SAGE ]

)";

Sage::Sage()
	:Agent("Sage",
			AgentRole::SENTINEL,
			SAGE_PORTRAIT){}

std::string Sage::roleStr() const {
	return "Sentinel";
}

std::string Sage::callout(const std::string& situation) const {
	if (situation == "start")
		return "I wasn't stron enough before. But now, now I am strong enough for us all";

	if (situation == "win")
		return "Now, to mend what we have broken.";

	if (situation == "lose")
		return "They will stumble. Now is our chance!";

	if (situation == "rotate")
		return "The best action here is to rotate.";

	if (situation == "spike_dropped")
		return "Our spike is down";

	return "Sage: " + situation;
}

std::vector<std::string> Sage::abilities() const {
	return {
		"Barrier Orb",
		"Slow Orb",
		"Healing Orb",
		"Resurrection"
	};
}

//SOVA

static constexpr const char* SOVA_PORTRAIT = R"(

     >====>
    | recon|
    | owl  |
     \    /
   [ SOVA ]

)";

Sova::Sova()
	: Agent("Sova",
			AgentRole::INITIATOR,
		SOVA_PORTRAIT) {}

std::string Sova::roleStr() const {
	return "Initiator";
}

std::string Sova::callout(const std::string& situation) const {
	if (situation == "start")
		return "There is no retreat. Not even when annihilation is at stake. This is our day.";

	if (situation == "win")
		return "Today is not our day to die";

	if (situation == "lose")
		return "The wind will shift in our favor next time.";

	if (situation == "spike_dropped")
		return "Our spike carrier is dead";

	if (situation == "rotate")
		return "I will rotate";

	return "Sova: " + situation;
}

std::vector<std::string> Sova::abilities() const {
	return {
		"Recon Bolt",
		"Shock Bolt",
		"Owl Drone",
		"Hunter's Fury"
	};
}