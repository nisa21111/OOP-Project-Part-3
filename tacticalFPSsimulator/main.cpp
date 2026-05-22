#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include "ui/mainMenuScreen.h"
#include "ui/assetManager.h"
#include "logger.h" 
int main() {
    sf::RenderWindow window(sf::VideoMode({ 1280, 720 }), "Tactical FPS Simulator - Menu Test");
    window.setFramerateLimit(60);

    std::vector<std::string> availableMaps = { "Bind", "Haven", "Ascent" };
    std::vector<std::string> availableAgents = { "Jett", "Omen", "Sova", "Reyna" };

    MainMenuScreen mainMenu(window.getSize(), availableMaps, availableAgents);
    mainMenu.onEnter("");

    sf::Clock clock;

    while (window.isOpen()) {
        while (std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            mainMenu.handleEvent(*event);
        }

        ScreenResult result = mainMenu.result();
        if (result.status == ScreenStatus::Quit) {
            std::cout << "[MAIN] Utilizatorul a ales sa iasa din joc.\n";
            window.close();
        }
        else if (result.status == ScreenStatus::ReplaceScreen) {
            std::cout << "[MAIN] Tranzitie detectata catre ecranul: " << result.nextScreen << "\n";
            std::cout << "[MAIN] Payload primit (Map|Agent|Strat): " << result.payload << "\n";

            mainMenu.onEnter("");
        }

        sf::Time dt = clock.restart();
        mainMenu.update(dt);

        window.clear(sf::Color(10, 10, 12));

        mainMenu.draw(window);

        window.display();
    }

    AssetManager::instance().clearCache();
    return 0;
}