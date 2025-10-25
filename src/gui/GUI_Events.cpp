#include "GUI.h"
#include "DataStore.h"
#include "gui/GuiInternal.h"
#include <iostream>
#include <optional>

void GUI::handleEvents() {
    try {
        while (std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            else if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseButtonPressed->button == sf::Mouse::Button::Left) {
                    sf::Vector2f mousePos(static_cast<float>(mouseButtonPressed->position.x), 
                                         static_cast<float>(mouseButtonPressed->position.y));
                    handleMouseClick(mousePos);
                }
            }
            else if (const auto* textEntered = event->getIf<sf::Event::TextEntered>()) {
                handleTextInput(textEntered->unicode);
            }
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                handleKeyPress(keyPressed->code);
            }
        }
        if (cursorClock.getElapsedTime().asSeconds() > 0.5f) {
            showCursor = !showCursor;
            cursorClock.restart();
        }
    } catch (const std::exception& e) {
        std::cerr << "Error in handleEvents: " << e.what() << std::endl;
    }
}
