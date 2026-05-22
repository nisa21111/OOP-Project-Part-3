#include "gameMap.h"
#include <iostream>
#include <string>

// O funcție ajutătoare care creează o mini-hartă direct în memorie pentru teste rapide
GameMap createManualTestMap() {
    GameMap manualMap("Ascent_Simulation");

    // 1. Creare noduri
    auto startNode = std::make_shared<ScenarioNode>(
        "start", "Spawn Atacatori",
        "Te afli in baza (Spawn Atacatori). Runda a inceput. Ce directie alegi?",
        NodeType::CHOICE
    );

    auto siteANode = std::make_shared<ScenarioNode>(
        "site_A", "A Site Main",
        "Ai intrat pe A Site. Inamicul a lasat un Smoke pe Heaven, dar zona pare libera. Ce faci?",
        NodeType::CHOICE
    );

    auto winNode = std::make_shared<ScenarioNode>(
        "win_node", "Spike Plantat",
        "Ai plantat Spike-ul cu succes! Inamicii au salvat armele. Runda castigata!",
        NodeType::TERMINAL
    );
    winNode->setTerminal(true); // Marchează ca nod de câștig

    // 2. Adăugare opțiuni/alegeri (Choices)
    Choice c1;
    c1.key = "a";
    c1.label = "Avanseaza rapid spre A Site";
    c1.nextNodeId = "site_A";
    startNode->addChoice(c1);

    Choice c2;
    c2.key = "plant";
    c2.label = "Planteaza Spike-ul rapid";
    c2.nextNodeId = "win_node";
    siteANode->addChoice(c2);

    // 3. Salvare noduri în hartă și setare punct de pornire
    manualMap.addNode(startNode);
    manualMap.addNode(siteANode);
    manualMap.addNode(winNode);
    manualMap.setStartNode("start");

    return manualMap;
}

int main() {
    std::cout << "=== TACTICAL FPS SIMULATOR ===" << std::endl;

    GameMap currentMap("Empty");

    try {
        // Opțiunea A: Încarcă din fișier text (dacă fișierul ascent.map există lângă .exe)
        // currentMap = GameMap::loadFromFile("ascent.map");

        // Opțiunea B: Folosește harta din memorie pe care tocmai am testat-o cu succes
        currentMap = createManualTestMap();
    }
    catch (const std::exception& e) {
        std::cerr << "Eroare la incarcarea hartii: " << e.what() << std::endl;
        return 1;
    }

    // Începe Bucla de Joc Interactivă
    std::string currentId = currentMap.startNodeId();

    while (true) {
        // Luăm nodul curent din structura noastră stabilă
        auto currentNode = currentMap.getNode(currentId);

        // Afișăm povestea/textul narativ
        std::cout << "\n----------------------------------------\n";
        std::cout << "[" << currentNode->calloutKey() << "]\n";
        std::cout << currentNode->narrative() << "\n\n";

        // Dacă am ajuns la final (nod terminal), oprim jocul
        if (currentNode->isTerminal()) {
            if (currentNode->isWin()) {
                std::cout << "FELICITARI! Ai castigat runda!\n";
            }
            else {
                std::cout << "GAME OVER! Ai pierdut runda.\n";
            }
            break;
        }

        // Afișăm opțiunile folosind numele reparat 'ch' ca să nu se bată în cap cu tipul structurii
        std::cout << "Optiuni disponibile:\n";
        for (const auto& ch : currentNode->choices()) {
            std::cout << "  [" << ch.key << "] -> " << ch.label << "\n";
        }

        // Citim decizia jucătorului
        std::cout << "\nAlege o optiune: ";
        std::string input;
        std::cin >> input;

        // Căutăm dacă input-ul introdus este o opțiune validă
        bool choiceFound = false;
        for (const auto& ch : currentNode->choices()) {
            if (ch.key == input) {
                currentId = ch.nextNodeId; // Schimbăm ID-ul curent cu cel spre care duce alegerea
                choiceFound = true;
                break;
            }
        }

        if (!choiceFound) {
            std::cout << "\n[Eroare] Optiune invalida! Incearca din nou.\n";
        }
    }

    std::cout << "\nMultumim ca ai jucat! Apasa orice tasta pentru a iesi...";
    std::cin.ignore();
    std::cin.get();
    return 0;
}