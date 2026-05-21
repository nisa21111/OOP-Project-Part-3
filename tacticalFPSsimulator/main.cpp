#include <iostream>
#include "repository.h"
#include "scenarionode.h"

int main() {

    std::cout << "=== SCENARIO NODE + REPOSITORY TEST ===\n\n";

    // =====================================================
    // 1. Create repository
    // =====================================================
    Repository<ScenarioNode> repo;

    auto node1 = std::make_shared<ScenarioNode>(
        "spawn",
        "start",
        "You spawn on Ascent attacker side.",
        NodeType::NARRATIVE
    );

    auto node2 = std::make_shared<ScenarioNode>(
        "mid",
        "rotate",
        "Team is rotating mid-round.",
        NodeType::EVENT
    );

    repo.add("spawn", node1);
    repo.add("mid", node2);

    // =====================================================
    // 2. Basic access
    // =====================================================
    std::cout << "All keys:\n";
    for (const auto& k : repo.keys()) {
        std::cout << " - " << k << "\n";
    }

    std::cout << "\nDisplaying spawn node:\n";
    repo.get("spawn")->display();

    // =====================================================
    // 3. findIf template
    // =====================================================
    auto found = findIf<ScenarioNode>(
        repo,
        [](const std::shared_ptr<ScenarioNode>& node) {
            return node->id() == "mid";
        }
    );

    if (found) {
        std::cout << "\nFound node via findIf:\n";
        (*found)->display();
    }

    // =====================================================
    // 4. filterAll template
    // =====================================================
    auto results = filterAll<ScenarioNode>(
        repo,
        [](const std::shared_ptr<ScenarioNode>& node) {
            return node->type() == NodeType::EVENT;
        }
    );

    std::cout << "\nEvent nodes count: " << results.size() << "\n";

    for (auto& n : results) {
        n->display();
    }

    std::cout << "\n=== TEST DONE ===\n";

    return 0;
}