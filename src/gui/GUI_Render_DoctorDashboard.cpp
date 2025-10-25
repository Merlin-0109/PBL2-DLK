#include "GUI.h"
#include "gui/GuiInternal.h"

void GUI::renderDoctorDashboard() {
    sf::RectangleShape titleBar({1280, 70});
    titleBar.setPosition({0, 0});
    titleBar.setFillColor(sf::Color(30, 136, 229));
    window.draw(titleBar);
    sf::Text appTitle = makeText(font, "Dashboard - Bác Sĩ", 28);
    appTitle.setPosition({40, 20});
    appTitle.setFillColor(sf::Color::White);
    appTitle.setStyle(sf::Text::Bold);
    window.draw(appTitle);
    sf::Text welcomeText = makeText(font, u8"Chào Mừng Bác Sĩ, " + inputUsername + "!", 32);
    welcomeText.setPosition({350, 200});
    welcomeText.setFillColor(sf::Color(33, 33, 33));
    welcomeText.setStyle(sf::Text::Bold);
    window.draw(welcomeText);
    sf::Text idText = makeText(font, "ID: " + currentUserId, 20);
    idText.setPosition({400, 250});
    idText.setFillColor(sf::Color(100, 100, 100));
    window.draw(idText);
    drawButton({440, 350}, {400, 55}, u8"Cập Nhật Thông Tin Cá Nhân",
               sf::Color(30, 136, 229), sf::Color::White, window);
}
