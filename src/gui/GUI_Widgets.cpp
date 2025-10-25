#include "GUI.h"
#include "gui/GuiInternal.h"

void GUI::drawRoundedRect(sf::Vector2f position, sf::Vector2f size, float radius,
                          sf::Color fillColor, sf::RenderWindow& win) {
    sf::RectangleShape rect(size);
    rect.setPosition(position);
    rect.setFillColor(fillColor);
    win.draw(rect);
}

void GUI::drawButton(sf::Vector2f position, sf::Vector2f size, const std::string& text,
                    sf::Color bgColor, sf::Color textColor, sf::RenderWindow& win) {
    sf::RectangleShape button(size);
    button.setPosition(position);
    button.setFillColor(bgColor);
    win.draw(button);
    sf::Text buttonText = makeText(font, text, 20);
    buttonText.setFillColor(textColor);
    buttonText.setStyle(sf::Text::Bold);
    sf::FloatRect textBounds = buttonText.getLocalBounds();
    buttonText.setOrigin({textBounds.size.x / 2.0f, textBounds.size.y / 2.0f});
    buttonText.setPosition({position.x + size.x / 2.0f, position.y + size.y / 2.0f});
    win.draw(buttonText);
}

void GUI::drawButton(sf::Vector2f position, sf::Vector2f size, const std::string& text,
                    sf::Color baseBg, sf::Color baseText,
                    bool enabled, bool hovered, sf::RenderWindow& win) {
    sf::Color bg = baseBg;
    sf::Color fg = baseText;
    if (!enabled) {
        bg = sf::Color(189, 189, 189);
        fg = sf::Color(255, 255, 255, 200);
    } else if (hovered) {
        int r = std::max(0, baseBg.r - 12);
        int g = std::max(0, baseBg.g - 12);
        int b = std::max(0, baseBg.b - 12);
        bg = sf::Color(r, g, b);
    }
    sf::RectangleShape button(size);
    button.setPosition(position);
    button.setFillColor(bg);
    win.draw(button);
    sf::Text buttonText = makeText(font, text, 20);
    buttonText.setFillColor(fg);
    buttonText.setStyle(sf::Text::Bold);
    sf::FloatRect textBounds = buttonText.getLocalBounds();
    buttonText.setOrigin({textBounds.size.x / 2.0f, textBounds.size.y / 2.0f});
    buttonText.setPosition({position.x + size.x / 2.0f, position.y + size.y / 2.0f});
    win.draw(buttonText);
}

bool GUI::isMouseOverRect(const sf::Vector2f& mousePos, const sf::Vector2f& rectPos,
                          const sf::Vector2f& rectSize) {
    return mousePos.x >= rectPos.x && mousePos.x <= rectPos.x + rectSize.x &&
           mousePos.y >= rectPos.y && mousePos.y <= rectPos.y + rectSize.y;
}

void GUI::drawInputField(sf::Vector2f position, sf::Vector2f size, const std::string& label,
                        const std::string& value, bool isActive, bool isPassword,
                        sf::RenderWindow& win) {
    sf::Text labelText = makeText(font, label, 14);
    // Left-align label above the input box
    labelText.setPosition({position.x, position.y - 28});
    labelText.setFillColor(sf::Color(80, 80, 80));
    labelText.setStyle(sf::Text::Bold);
    win.draw(labelText);
    sf::RectangleShape inputBox(size);
    inputBox.setPosition(position);
    inputBox.setFillColor(sf::Color(255, 255, 255));
    inputBox.setOutlineThickness(2);
    inputBox.setOutlineColor(isActive ? sf::Color(30, 136, 229) : sf::Color(220, 220, 220));
    win.draw(inputBox);
    std::string displayValue = value;
    if (isPassword && !value.empty()) {
        displayValue = std::string(value.length(), '*');
    }
    sf::Text valueText = makeText(font, displayValue, 16);
    valueText.setPosition({position.x + 18, position.y + 15});
    valueText.setFillColor(sf::Color(33, 33, 33));
    win.draw(valueText);
    if (isActive && showCursor) {
        sf::FloatRect bounds = valueText.getLocalBounds();
        sf::RectangleShape cursor({2, 28});
        cursor.setPosition({position.x + 18 + bounds.size.x + 3, position.y + 11});
        cursor.setFillColor(sf::Color(30, 136, 229));
        win.draw(cursor);
    }
}

// Draw input without a top label, showing a placeholder inside the box when value is empty
void GUI::drawInputFieldWithPlaceholder(sf::Vector2f position, sf::Vector2f size, const std::string& placeholder,
                                       const std::string& value, bool isActive, bool isPassword,
                                       sf::RenderWindow& win) {
    sf::RectangleShape inputBox(size);
    inputBox.setPosition(position);
    inputBox.setFillColor(sf::Color(255, 255, 255));
    inputBox.setOutlineThickness(2);
    inputBox.setOutlineColor(isActive ? sf::Color(30, 136, 229) : sf::Color(220, 220, 220));
    win.draw(inputBox);

    // Choose text: placeholder (grey) when empty, otherwise actual value
    bool showPlaceholder = value.empty();
    std::string display = value;
    if (isPassword && !value.empty()) {
        display = std::string(value.length(), '*');
    }
    sf::Text valueText = makeText(font, showPlaceholder ? placeholder : display, 16);
    valueText.setPosition({position.x + 18, position.y + 15});
    valueText.setFillColor(showPlaceholder ? sf::Color(160, 170, 180) : sf::Color(33, 33, 33));
    win.draw(valueText);

    // Caret when active and not showing placeholder (or at start if empty)
    if (isActive && showCursor) {
        sf::FloatRect bounds = valueText.getLocalBounds();
        float caretX = position.x + 18 + (showPlaceholder ? 0.f : bounds.size.x) + 3.f;
        sf::RectangleShape cursor({2, 28});
        cursor.setPosition({caretX, position.y + 11});
        cursor.setFillColor(sf::Color(30, 136, 229));
        win.draw(cursor);
    }
}

void GUI::drawRoleCard(sf::Vector2f position, sf::Vector2f size, const std::string& title,
                      const std::string& description, const std::string& icon,
                      bool isSelected, sf::RenderWindow& win) {
    sf::RectangleShape card(size);
    card.setPosition(position);
    card.setFillColor(sf::Color::White);
    card.setOutlineThickness(3);
    card.setOutlineColor(isSelected ? sf::Color(30, 136, 229) : sf::Color(230, 230, 230));
    win.draw(card);
    sf::Text iconText = makeText(font, icon, 48);
    iconText.setPosition({position.x + 25, position.y + 15});
    iconText.setFillColor(sf::Color(30, 136, 229));
    iconText.setStyle(sf::Text::Bold);
    win.draw(iconText);
    sf::CircleShape radioOuter(12);
    radioOuter.setPosition({position.x + size.x - 40, position.y + 20});
    radioOuter.setFillColor(sf::Color::White);
    radioOuter.setOutlineThickness(2);
    radioOuter.setOutlineColor(isSelected ? sf::Color(30, 136, 229) : sf::Color(180, 180, 180));
    win.draw(radioOuter);
    if (isSelected) {
        sf::CircleShape radioInner(7);
        radioInner.setPosition({position.x + size.x - 35, position.y + 25});
        radioInner.setFillColor(sf::Color(30, 136, 229));
        win.draw(radioInner);
    }
    sf::Text titleText = makeText(font, title, 22);
    titleText.setPosition({position.x + 25, position.y + 70});
    titleText.setFillColor(sf::Color(33, 33, 33));
    titleText.setStyle(sf::Text::Bold);
    win.draw(titleText);
    sf::Text descText = makeText(font, description, 14);
    descText.setPosition({position.x + 25, position.y + 100});
    descText.setFillColor(sf::Color(120, 120, 120));
    win.draw(descText);
}

void GUI::drawNavigationMenu(sf::Vector2f position, sf::Vector2f size, sf::RenderWindow& win) {
    sf::RectangleShape navBg(size);
    navBg.setPosition(position);
    navBg.setFillColor(sf::Color(248, 250, 252));
    win.draw(navBg);
    sf::RectangleShape border({2, size.y});
    border.setPosition({position.x + size.x, position.y});
    border.setFillColor(sf::Color(220, 230, 240));
    win.draw(border);
    float menuItemHeight = 62;
    float startY = position.y + 45;
    struct MenuItem { PatientMenuOption option; std::string icon; std::string text; };
    MenuItem items[] = {
        {PatientMenuOption::BOOK_APPOINTMENT, u8"⊕", u8"Đặt lịch khám"},
        {PatientMenuOption::MY_APPOINTMENTS, u8"☰", u8"Lịch khám của tôi"},
        {PatientMenuOption::NOTIFICATIONS, u8"◉", u8"Thông báo"},
        {PatientMenuOption::UPDATE_INFO, u8"◈", u8"Cập nhật thông tin"}
    };
    for (int i = 0; i < 4; i++) {
        bool isActive = (items[i].option == activePatientMenu);
        float itemY = startY + i * (menuItemHeight + 12);
        if (isActive) {
            sf::RectangleShape highlight({size.x - 20, menuItemHeight});
            highlight.setPosition({position.x + 10, itemY});
            highlight.setFillColor(sf::Color(30, 136, 229));
            win.draw(highlight);
        }
        sf::Text icon = makeSymbolText(symbolFont, items[i].icon, 30);
        icon.setPosition({position.x + 22, itemY + 14});
        icon.setFillColor(isActive ? sf::Color::White : sf::Color(80, 100, 120));
        win.draw(icon);
        sf::Text text = makeText(font, items[i].text, 14);
        text.setPosition({position.x + 65, itemY + 22});
        text.setFillColor(isActive ? sf::Color::White : sf::Color(50, 60, 80));
        if (isActive) text.setStyle(sf::Text::Bold);
        win.draw(text);
    }
}

void GUI::drawAppointmentCard(sf::Vector2f position, sf::Vector2f size,
                              const std::string& doctorName, const std::string& specialty,
                              const std::string& dateTime, bool isCancelled, sf::RenderWindow& win) {
    sf::RectangleShape shadow(size);
    shadow.setPosition({position.x + 2, position.y + 2});
    shadow.setFillColor(sf::Color(220, 230, 240, 80));
    win.draw(shadow);
    sf::RectangleShape card(size);
    card.setPosition(position);
    card.setFillColor(isCancelled ? sf::Color(245, 245, 245) : sf::Color::White);
    card.setOutlineThickness(1);
    card.setOutlineColor(sf::Color(220, 230, 240));
    win.draw(card);
    sf::Text avatar = makeSymbolText(symbolFont, u8"⚕", 34);
    avatar.setPosition({position.x + 18, position.y + 15});
    avatar.setFillColor(isCancelled ? sf::Color(150, 150, 150) : sf::Color(30, 136, 229));
    win.draw(avatar);
    sf::Text doctor = makeText(font, doctorName, 16);
    doctor.setPosition({position.x + 65, position.y + 18});
    doctor.setFillColor(sf::Color(25, 35, 50));
    doctor.setStyle(sf::Text::Bold);
    win.draw(doctor);
    sf::RectangleShape specBadge({110, 24});
    specBadge.setPosition({position.x + 65, position.y + 48});
    specBadge.setFillColor(sf::Color(230, 245, 255));
    win.draw(specBadge);
    sf::Text specIcon = makeSymbolText(symbolFont, u8"●", 10);
    specIcon.setPosition({position.x + 70, position.y + 54});
    specIcon.setFillColor(sf::Color(30, 136, 229));
    win.draw(specIcon);
    sf::Text spec = makeText(font, specialty, 12);
    spec.setPosition({position.x + 83, position.y + 52});
    spec.setFillColor(sf::Color(30, 136, 229));
    win.draw(spec);
    sf::Text clockIcon = makeSymbolText(symbolFont, u8"◷", 18);
    clockIcon.setPosition({position.x + 18, position.y + size.y - 32});
    clockIcon.setFillColor(sf::Color(100, 120, 140));
    win.draw(clockIcon);
    sf::Text date = makeText(font, dateTime, 14);
    date.setPosition({position.x + 45, position.y + size.y - 29});
    date.setFillColor(sf::Color(80, 100, 120));
    win.draw(date);
    
    // Always show cancel button, but styled differently if cancelled
    sf::RectangleShape cancelBtn({105, 32});
    cancelBtn.setPosition({position.x + size.x - 120, position.y + size.y - 42});
    cancelBtn.setFillColor(isCancelled ? sf::Color(150, 150, 150) : sf::Color(244, 67, 54));
    win.draw(cancelBtn);
    
    sf::Text cancelText = makeText(font, isCancelled ? u8"ĐÃ HỦY" : u8"✕ Hủy", 13);
    sf::FloatRect cancelBounds = cancelText.getLocalBounds();
    cancelText.setPosition({position.x + size.x - 67 - cancelBounds.size.x / 2, 
                           position.y + size.y - 34});
    cancelText.setFillColor(sf::Color::White);
    win.draw(cancelText);
    
    sf::Text arrow = makeSymbolText(symbolFont, u8"›", 28);
    arrow.setPosition({position.x + size.x - 35, position.y + 38});
    arrow.setFillColor(sf::Color(150, 170, 190));
    win.draw(arrow);
}

void GUI::drawDoctorListItem(sf::Vector2f position, sf::Vector2f size,
                             const std::string& doctorName, const std::string& specialty,
                             const std::string& doctorId, sf::RenderWindow& win) {
    sf::RectangleShape card(size);
    card.setPosition(position);
    card.setFillColor(sf::Color::White);
    card.setOutlineThickness(1);
    card.setOutlineColor(sf::Color(220, 230, 240));
    win.draw(card);
    sf::Text avatar = makeSymbolText(symbolFont, u8"⚕", 28);
    avatar.setPosition({position.x + 14, position.y + 12});
    avatar.setFillColor(sf::Color(30, 136, 229));
    win.draw(avatar);
    sf::Text name = makeText(font, doctorName, 16);
    name.setPosition({position.x + 54, position.y + 10});
    name.setFillColor(sf::Color(25, 35, 50));
    name.setStyle(sf::Text::Bold);
    win.draw(name);
    sf::RectangleShape specBadge({150, 22});
    specBadge.setPosition({position.x + 54, position.y + 40});
    specBadge.setFillColor(sf::Color(230, 245, 255));
    win.draw(specBadge);
    sf::Text spec = makeText(font, specialty, 12);
    spec.setPosition({position.x + 64, position.y + 42});
    spec.setFillColor(sf::Color(30, 136, 229));
    win.draw(spec);
    sf::Text idText = makeText(font, "#" + doctorId, 12);
    sf::FloatRect idBounds = idText.getLocalBounds();
    idText.setPosition({position.x + size.x - 12 - idBounds.size.x, position.y + 10});
    idText.setFillColor(sf::Color(150, 160, 170));
    win.draw(idText);
    drawButton({position.x + size.x - 120, position.y + size.y - 40}, {110, 32}, u8"Đặt lịch",
               sf::Color(76, 175, 80), sf::Color::White, win);
}
