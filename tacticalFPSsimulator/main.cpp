#include "agent.h"

#include <iostream>
#include <memory>
#include <vector>

int main() {
	std::vector<std::unique_ptr<Agent>> agents;

	agents.push_back(std::make_unique<Brimstone>());
	agents.push_back(std::make_unique<Jett>());
	agents.push_back(std::make_unique<Sova>());
	agents.push_back(std::make_unique<Sage>());

	for (const auto& agent : agents) {
		agent->display();

		std::cout
			<< agent->callout("start")
			<< "\n\n";
		std::cout << "Abilities:\n";

		for (const auto& ability : agent->abilities()) {
			std::cout << " - " << ability << "\n";
		}
		std::cout << "\n====================\n\n";
	}

	return 0;


}