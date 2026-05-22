#include "exceptions.h"
#include "logger.h"
#include "configManager.h"
#include "repository.h"
#include "agent.h"
#include "agentFactory.h"
#include "scenarioNode.h"
#include "gameMap.h"
#include "observer.h"
#include "command.h"
#include "outcomeStrategy.h"

#include <iostream>
#include <cassert>
#include <memory>


static int passed = 0;
static int failed = 0;

static void ok(const std::string& name) {
    std::cout << "  [PASS] " << name << "\n";
    ++passed;
}

static void fail(const std::string& name, const std::string& reason) {
    std::cerr << "  [FAIL] " << name << " — " << reason << "\n";
    ++failed;
}

#define TEST(name, expr) \
    try { \
        if (expr) ok(name); \
        else fail(name, "conditia e falsa"); \
    } catch (const std::exception& e) { \
        fail(name, std::string("exceptie neasteptata: ") + e.what()); \
    }

#define TEST_THROWS(name, expr, ExcType) \
    try { \
        expr; \
        fail(name, "nu s-a aruncat exceptia asteptata"); \
    } catch (const ExcType&) { \
        ok(name); \
    } catch (const std::exception& e) { \
        fail(name, std::string("tip gresit de exceptie: ") + e.what()); \
    }

//logger

void testLogger() {
    std::cout << "\n[Logger]\n";
    auto& log = Logger::instance();
    auto& log2 = Logger::instance();
    TEST("Singleton — aceeasi instanta", &log == &log2);
    try {
        log.info("test info");
        log.warning("test warning");
        ok("log.info / log.warning nu arunca");
    }
    catch (...) {
        fail("log.info / log.warning nu arunca", "a aruncat");
    }
}

// config manager

void testConfig() {
    std::cout << "\n[ConfigManager]\n";
    auto& cfg = ConfigManager::instance();
    auto& cfg2 = ConfigManager::instance();
    TEST("Singleton — aceeasi instanta", &cfg == &cfg2);

    cfg.loadFromFile("data/config/settings.cfg");

    TEST("get cu default (cheie inexistenta)", cfg.get("cheie_inexistenta", "def") == "def");
    TEST("getInt cu default", cfg.getInt("cheie_inexistenta", 42) == 42);
    TEST("getBool cu default", cfg.getBool("cheie_inexistenta", true) == true);

    TEST("window_width exista in cfg", cfg.getInt("window_width", 0) > 0);
    TEST("window_height exista in cfg", cfg.getInt("window_height", 0) > 0);
}

// repo

void testRepository() {
    std::cout << "\n[Repository<T>]\n";

    Repository<Agent> repo;
    TEST("empty() initial", repo.empty());
    TEST("size() == 0 initial", repo.size() == 0);

    auto brim = std::make_shared<Brimstone>();
    repo.add("brimstone", brim);
    TEST("size() == 1 dupa add", repo.size() == 1);
    TEST("contains() true", repo.contains("brimstone"));
    TEST("contains() false pt alta cheie", !repo.contains("jett"));

    auto found = repo.find("brimstone");
    TEST("find() returneaza optional plin", found.has_value());
    TEST("find() returneaza obiectul corect", (*found)->name() == "Brimstone");

    auto notFound = repo.find("sova");
    TEST("find() returneaza nullopt pt cheie inexistenta", !notFound.has_value());

    TEST_THROWS("get() arunca out_of_range pt cheie inexistenta",
        repo.get("inexistent"), std::out_of_range);

    repo.remove("brimstone");
    TEST("empty() dupa remove", repo.empty());

    Repository<ScenarioNode> nodeRepo;
    auto node = std::make_shared<ScenarioNode>("n1", "start", "Test narrative", NodeType::NARRATIVE);
    nodeRepo.add("n1", node);
    TEST("Repository<ScenarioNode> — add + find", nodeRepo.find("n1").has_value());

    Repository<Agent> agentRepo;
    agentRepo.add("brimstone", std::make_shared<Brimstone>());
    agentRepo.add("jett", std::make_shared<Jett>());
    agentRepo.add("sage", std::make_shared<Sage>());
    agentRepo.add("sova", std::make_shared<Sova>());

    auto controllers = filterAll<Agent>(agentRepo, [](const std::shared_ptr<Agent>& a) {
        return a->role() == AgentRole::CONTROLLER;
        });
    TEST("filterAll<Agent> — gaseste 1 controller (Brimstone)", controllers.size() == 1);

    auto sentinel = findIf<Agent>(agentRepo, [](const std::shared_ptr<Agent>& a) {
        return a->role() == AgentRole::SENTINEL;
        });
    TEST("findIf<Agent> — gaseste Sage (Sentinel)", sentinel.has_value() && (*sentinel)->name() == "Sage");

    Repository<std::vector<std::string>> eventQueue;
    eventQueue.append("round1", "SPIKE_DROPPED");
    eventQueue.append("round1", "ROTATE");
    auto events = eventQueue.find("round1");
    TEST("Repository<vector<T>> specializare partiala — append + find", events.has_value() && events->size() == 2);
}

// agent si factory

void testAgents() {
    std::cout << "\n[Agent + AgentFactory]\n";

    std::vector<std::unique_ptr<Agent>> agents;
    agents.push_back(std::make_unique<Brimstone>());
    agents.push_back(std::make_unique<Jett>());
    agents.push_back(std::make_unique<Sage>());
    agents.push_back(std::make_unique<Sova>());

    TEST("Brimstone — roleStr()", agents[0]->roleStr() == "Controller");
    TEST("Jett — roleStr()", agents[1]->roleStr() == "Duelist");
    TEST("Sage — roleStr()", agents[2]->roleStr() == "Sentinel");
    TEST("Sova — roleStr()", agents[3]->roleStr() == "Initiator");

    TEST("Brimstone — callout(start) nu e gol", !agents[0]->callout("start").empty());
    TEST("Jett — callout(win) nu e gol", !agents[1]->callout("win").empty());

    TEST("Brimstone — portraitPath nu e gol", !agents[0]->portraitPath().empty());
    TEST("Brimstone — abilities() are 4", agents[0]->abilities().size() == 4);

    std::cout << "  [INFO] Callout polimorfic pe colectie:\n";
    for (auto& a : agents) {
        std::cout << "         " << a->name() << ": \"" << a->callout("start") << "\"\n";
    }
    ok("Polimorfism virtual — callout() pe colectie de unique_ptr<Agent>");

    auto& factory = AgentFactory::instance();
    auto& factory2 = AgentFactory::instance();
    TEST("AgentFactory Singleton", &factory == &factory2);

    auto brim = factory.create("brimstone");
    TEST("Factory create(brimstone)", brim != nullptr && brim->name() == "Brimstone");

    auto jett = factory.create("JETT");
    TEST("Factory create(JETT) — case insensitive", jett != nullptr && jett->name() == "Jett");

    TEST_THROWS("Factory create(inexistent) arunca AgentNotFoundException",
        factory.create("phoenix_nonexistent"), AgentNotFoundException);

    auto available = factory.available();
    TEST("Factory available() nu e gol", !available.empty());
}

// scen node

void testScenarioNode() {
    std::cout << "\n[ScenarioNode]\n";

    ScenarioNode node("spawn", "start", "Attacker spawn. Where do you go?", NodeType::NARRATIVE);
    TEST("id()", node.id() == "spawn");
    TEST("calloutKey()", node.calloutKey() == "start");
    TEST("narrative()", !node.narrative().empty());
    TEST("isTerminal() initial false", !node.isTerminal());
    TEST("choices() initial gol", node.choices().empty());

    Choice c1; c1.key = "a"; c1.label = "Push A"; c1.nextNodeId = "a_site";
    Choice c2; c2.key = "b"; c2.label = "Push B"; c2.nextNodeId = "b_site";
    c2.eventTrigger = "SPIKE_DROPPED";
    node.addChoice(c1);
    node.addChoice(c2);

    TEST("choices() are 2 dupa addChoice x2", node.choices().size() == 2);
    TEST("choice[1] are eventTrigger", node.choices()[1].eventTrigger.has_value());
    TEST("choice[1] eventTrigger == SPIKE_DROPPED",
        *node.choices()[1].eventTrigger == "SPIKE_DROPPED");

    node.setTerminal(true);
    TEST("isTerminal() true dupa setTerminal(true)", node.isTerminal());
    TEST("isWin() true", node.isWin());

    node.setBackgroundImage("data/images/ascent_asite.png");
    TEST("backgroundImage() setat corect", node.backgroundImage() == "data/images/ascent_asite.png");
}

// game map URELETE SI PLANSETE 

void testGameMap() {
    std::cout << "\n[GameMap]\n";

    try {
        auto map = GameMap::loadFromFile("data/maps/ascent.map");
        TEST("loadFromFile — name() == ascent", map.name() == "ascent");
        TEST("loadFromFile — startNodeId() != gol", !map.startNodeId().empty());
        TEST("loadFromFile — defaultBackground != gol", !map.defaultBackground().empty());

        auto startNode = map.getNode(map.startNodeId());
        TEST("getNode(startNodeId) returneaza nod valid", startNode != nullptr);
        TEST("startNode are choices", !startNode->choices().empty());

        TEST_THROWS("getNode(inexistent) arunca ScenarioLoadException",
            map.getNode("nod_care_nu_exista_niciodata"), ScenarioLoadException);

        std::cout << "  [INFO] Choices din nodul start (" << startNode->id() << "):\n";
        for (auto& c : startNode->choices())
            std::cout << "         [" << c.key << "] " << c.label << " -> " << c.nextNodeId << "\n";
        ok("Choices din nodul start sunt accesibile");

    }
    catch (const ScenarioLoadException& e) {
        fail("loadFromFile ascent.map", e.what());
    }

    try {
        auto bind = GameMap::loadFromFile("data/maps/bind.map");
        TEST("loadFromFile bind — name() == bind", bind.name() == "bind");
        ok("loadFromFile bind.map reusit");
    }
    catch (const ScenarioLoadException& e) {
        fail("loadFromFile bind.map", e.what());
    }

    TEST_THROWS("loadFromFile(inexistent) arunca ScenarioLoadException",
        GameMap::loadFromFile("data/maps/nu_exista.map"), ScenarioLoadException);
}

// observer

void testObserver() {
    std::cout << "\n[Observer / EventBus]\n";

    class CountingObserver : public IObserver {
    public:
        int count = 0;
        std::string lastType;
        void onEvent(const GameEvent& e) override {
            ++count;
            lastType = e.type;
        }
    };

    auto bus = std::make_shared<EventBus>();
    auto obs1 = std::make_shared<CountingObserver>();
    auto obs2 = std::make_shared<CountingObserver>();

    bus->subscribe(obs1);
    bus->subscribe(obs2);

    bus->notify({ "SPIKE_DROPPED", "dropped at A main" });
    TEST("obs1 a primit 1 event", obs1->count == 1);
    TEST("obs2 a primit 1 event", obs2->count == 1);
    TEST("obs1 lastType == SPIKE_DROPPED", obs1->lastType == "SPIKE_DROPPED");

    bus->notify({ "ROTATE", "mid rotation" });
    TEST("obs1 a primit 2 events total", obs1->count == 2);

    obs2.reset();
    bus->notify({ "TEAM_IGNORED", "nobody listened" });
    TEST("obs1 inca primeste dupa ce obs2 a expirat", obs1->count == 3);
    // obs2 e expired — nu mai primeste, nu crapa
    ok("weak_ptr expirat curatat automat din EventBus");
}

// command

void testCommand() {
    std::cout << "\n[Command]\n";

    int counter = 0;

    CommandHistory history;
    TEST("canUndo() false initial", !history.canUndo());

    history.execute(std::make_unique<FunctionCommand>(
        "increment", [&]() { ++counter; }));
    TEST("execute ruleaza actiunea", counter == 1);
    TEST("canUndo() false (no undo fn)", !history.canUndo());

    history.execute(std::make_unique<FunctionCommand>(
        "add 10", [&]() { counter += 10; }, [&]() { counter -= 10; }));
    TEST("execute cu undo — counter == 11", counter == 11);
    TEST("canUndo() true dupa comanda cu undo", history.canUndo());

    history.undo();
    TEST("undo() — counter revine la 1", counter == 1);
    TEST("canUndo() false dupa undo", !history.canUndo());

    history.execute(std::make_unique<FunctionCommand>(
        "add 5", [&]() { counter += 5; }, [&]() { counter -= 5; }));
    history.execute(std::make_unique<FunctionCommand>(
        "add 3", [&]() { counter += 3; }, [&]() { counter -= 3; }));
    TEST("counter == 9 dupa add5 + add3", counter == 9);

    history.undo();
    TEST("undo add3 — counter == 6", counter == 6);
    history.undo();
    TEST("undo add5 — counter == 1", counter == 1);
}

// outcome

void testStrategy() {
    std::cout << "\n[OutcomeStrategy]\n";

    AggressiveStrategy  agg;
    CoordinatedStrategy coord;
    BalancedStrategy    bal;

    TEST("AggressiveStrategy  name()", agg.name() == "Aggressive");
    TEST("CoordinatedStrategy name()", coord.name() == "Coordinated");
    TEST("BalancedStrategy    name()", bal.name() == "Balanced");

    try {
        bool r1 = agg.resolve(10, 10);
        bool r2 = coord.resolve(0, 0);
        bool r3 = bal.resolve(5, 5);
        (void)r1; (void)r2; (void)r3;
        ok("resolve() nu arunca exceptii pe valori extreme");
    }
    catch (...) {
        fail("resolve() nu arunca exceptii", "a aruncat");
    }

    std::vector<std::unique_ptr<IOutcomeStrategy>> strategies;
    strategies.push_back(std::make_unique<AggressiveStrategy>());
    strategies.push_back(std::make_unique<CoordinatedStrategy>());
    strategies.push_back(std::make_unique<BalancedStrategy>());

    std::cout << "  [INFO] resolve(8, 8) pe fiecare strategie:\n";
    for (auto& s : strategies) {
        bool r = s->resolve(8, 8);
        std::cout << "         " << s->name() << " -> " << (r ? "WIN" : "LOSE") << "\n";
    }
    ok("Polimorfism virtual — resolve() pe vector<unique_ptr<IOutcomeStrategy>>");
}

// excep

void testExceptions() {
    std::cout << "\n[Exceptions]\n";

    try {
        throw ScenarioLoadException("test.map");
    }
    catch (const GameException& e) {
        TEST("ScenarioLoadException e prinsa ca GameException", true);
        TEST("ScenarioLoadException what() contine numele fisierului",
            std::string(e.what()).find("test.map") != std::string::npos);
    }

    try {
        throw AgentNotFoundException("Phoenix");
    }
    catch (const std::exception& e) {
        TEST("AgentNotFoundException e prinsa ca std::exception", true);
        TEST("AgentNotFoundException what() contine numele agentului",
            std::string(e.what()).find("Phoenix") != std::string::npos);
    }

    try {
        throw InvalidChoiceException("xyz");
    }
    catch (const GameException& e) {
        TEST("InvalidChoiceException e prinsa ca GameException", true);
    }
}


int main() {
    std::cout << "========================================\n";
    std::cout << "  Tactical FPS Sim — Test Suite (non-UI)\n";
    std::cout << "========================================\n";

    try {
        testLogger();
        testConfig();
        testRepository();
        testAgents();
        testScenarioNode();
        testGameMap();
        testObserver();
        testCommand();
        testStrategy();
        testExceptions();
    }
    catch (const std::exception& e) {
        std::cerr << "\n[FATAL] Exceptie necaptata in test: " << e.what() << "\n";
        return 1;
    }

    std::cout << "\n========================================\n";
    std::cout << "  PASSED: " << passed << "\n";
    std::cout << "  FAILED: " << failed << "\n";
    std::cout << "========================================\n";

    return failed > 0 ? 1 : 0;
}