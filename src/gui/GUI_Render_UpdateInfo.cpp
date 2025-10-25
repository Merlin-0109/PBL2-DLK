#include "GUI.h"
#include "gui/GuiInternal.h"

void GUI::renderUpdatePatientInfo() {
    auto ws = window.getSize();
    float W = static_cast<float>(ws.x);
    float H = static_cast<float>(ws.y);
    sf::RectangleShape background({W, H});
    background.setPosition({0, 0});
    background.setFillColor(sf::Color(245, 248, 250));
    window.draw(background);
    sf::RectangleShape titleBar({W, 70});
    titleBar.setPosition({0, 0});
    titleBar.setFillColor(sf::Color(30, 136, 229));
    window.draw(titleBar);
    sf::Text titleIcon = makeText(font, u8"⚕", 28);
    titleIcon.setPosition({40, 20});
    titleIcon.setFillColor(sf::Color(76, 175, 80));
    window.draw(titleIcon);
    sf::Text appTitle = makeText(font, u8"Cập Nhật Thông Tin Bệnh Nhân", 24);
    appTitle.setPosition({80, 23});
    appTitle.setFillColor(sf::Color::White);
    appTitle.setStyle(sf::Text::Bold);
    window.draw(appTitle);
    float panelW = 800.f;
    float panelH = 740.f;
    float panelX = W / 2.f - panelW / 2.f;
    float panelY = 90.f;
    sf::RectangleShape panel({panelW, panelH});
    panel.setPosition({panelX, panelY});
    panel.setFillColor(sf::Color::White);
    window.draw(panel);
    sf::Text title = makeText(font, u8"◈ Thông Tin Cá Nhân", 26);
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setPosition({W / 2.f - titleBounds.size.x / 2.f, 120});
    title.setFillColor(sf::Color(33, 33, 33));
    title.setStyle(sf::Text::Bold);
    window.draw(title);
    sf::Text subtitle = makeText(font, u8"Vui lòng cập nhật đầy đủ thông tin của bạn", 14);
    sf::FloatRect subtitleBounds = subtitle.getLocalBounds();
    subtitle.setPosition({W / 2.f - subtitleBounds.size.x / 2.f, 155});
    subtitle.setFillColor(sf::Color(120, 120, 120));
    window.draw(subtitle);
    float startY = 200;
    float spacing = 75;
    float baseX = panelX + 80;
    drawInputField({baseX, startY}, {640, 50}, u8"Họ và tên *", inputFullName,
                   activeInputField == 0, false, window);
    drawInputField({baseX, startY + spacing}, {640, 50}, u8"Số điện thoại *", inputPhone,
                   activeInputField == 1, false, window);
    drawInputField({baseX, startY + spacing * 2}, {640, 50}, "Email *", inputEmail,
                   activeInputField == 2, false, window);
    drawInputField({baseX, startY + spacing * 3}, {640, 50}, u8"Địa chỉ", inputAddress,
                   activeInputField == 3, false, window);
    drawInputField({baseX, startY + spacing * 4}, {640, 50}, u8"Ngày sinh (DD/MM/YYYY)", inputDOB,
                   activeInputField == 4, false, window);
    drawInputField({baseX, startY + spacing * 5}, {640, 50}, u8"Giới tính", inputGender,
                   activeInputField == 5, false, window);
    drawInputField({baseX, startY + spacing * 6}, {640, 50}, u8"Nhóm máu", inputBloodType,
                   activeInputField == 6, false, window);
    drawInputField({baseX, startY + spacing * 7}, {640, 50}, "CCCD", inputCCCD,
                   activeInputField == 7, false, window);
    bool canSave = !inputFullName.empty() && !inputPhone.empty() && !inputEmail.empty();
    auto mp = sf::Mouse::getPosition(window);
    sf::Vector2f mpos = {static_cast<float>(mp.x), static_cast<float>(mp.y)};
    bool hovered = isMouseOverRect(mpos, {W / 2.f - 150.f, 800}, {300, 50});
    drawButton({W / 2.f - 150.f, 800}, {300, 50}, u8"Lưu Thông Tin",
               sf::Color(76, 175, 80), sf::Color::White, canSave, hovered, window);
}

void GUI::renderUpdateDoctorInfo() {
    auto ws = window.getSize();
    float W = static_cast<float>(ws.x);
    float H = static_cast<float>(ws.y);
    sf::RectangleShape background({W, H});
    background.setPosition({0, 0});
    background.setFillColor(sf::Color(245, 248, 250));
    window.draw(background);
    sf::RectangleShape titleBar({W, 70});
    titleBar.setPosition({0, 0});
    titleBar.setFillColor(sf::Color(30, 136, 229));
    window.draw(titleBar);
    sf::Text titleIcon = makeText(font, u8"⚕", 28);
    titleIcon.setPosition({40, 20});
    titleIcon.setFillColor(sf::Color(76, 175, 80));
    window.draw(titleIcon);
    sf::Text appTitle = makeText(font, u8"Cập Nhật Thông Tin Bác Sĩ", 24);
    appTitle.setPosition({80, 23});
    appTitle.setFillColor(sf::Color::White);
    appTitle.setStyle(sf::Text::Bold);
    window.draw(appTitle);
    float panelW = 800.f;
    float panelH = 780.f;
    float panelX = W / 2.f - panelW / 2.f;
    float panelY = 85.f;
    sf::RectangleShape panel({panelW, panelH});
    panel.setPosition({panelX, panelY});
    panel.setFillColor(sf::Color::White);
    window.draw(panel);
    sf::Text title = makeText(font, u8"◈ Thông Tin Bác Sĩ", 26);
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setPosition({W / 2.f - titleBounds.size.x / 2.f, 110});
    title.setFillColor(sf::Color(33, 33, 33));
    title.setStyle(sf::Text::Bold);
    window.draw(title);
    sf::Text subtitle = makeText(font, u8"Cập nhật thông tin hành nghề và chuyên môn", 14);
    sf::FloatRect subtitleBounds = subtitle.getLocalBounds();
    subtitle.setPosition({W / 2.f - subtitleBounds.size.x / 2.f, 145});
    subtitle.setFillColor(sf::Color(120, 120, 120));
    window.draw(subtitle);
    float startY = 185;
    float spacing = 70;
    float baseX = panelX + 80;
    drawInputField({baseX, startY}, {640, 50}, u8"Họ và tên *", inputFullName,
                   activeInputField == 0, false, window);
    drawInputField({baseX, startY + spacing}, {640, 50}, u8"Số điện thoại *", inputPhone,
                   activeInputField == 1, false, window);
    drawInputField({baseX, startY + spacing * 2}, {640, 50}, "Email *", inputEmail,
                   activeInputField == 2, false, window);
    drawInputField({baseX, startY + spacing * 3}, {640, 50}, u8"Địa chỉ", inputAddress,
                   activeInputField == 3, false, window);
    drawInputField({baseX, startY + spacing * 4}, {640, 50}, u8"Ngày sinh (DD/MM/YYYY)", inputDOB,
                   activeInputField == 4, false, window);
    drawInputField({baseX, startY + spacing * 5}, {640, 50}, u8"Giới tính", inputGender,
                   activeInputField == 5, false, window);
    drawInputField({baseX, startY + spacing * 6}, {640, 50}, "CCCD", inputCCCD,
                   activeInputField == 6, false, window);
    drawInputField({baseX, startY + spacing * 7}, {640, 50}, u8"Chuyên môn *", inputSpecialization,
                   activeInputField == 7, false, window);
    drawInputField({baseX, startY + spacing * 8}, {640, 50}, "Khoa *", inputFaculty,
                   activeInputField == 8, false, window);
    bool canSave = !inputFullName.empty() && !inputPhone.empty() && !inputEmail.empty()
                   && !inputSpecialization.empty() && !inputFaculty.empty();
    auto mp = sf::Mouse::getPosition(window);
    sf::Vector2f mpos = {static_cast<float>(mp.x), static_cast<float>(mp.y)};
    bool hovered = isMouseOverRect(mpos, {W / 2.f - 150.f, 820}, {300, 50});
    drawButton({W / 2.f - 150.f, 820}, {300, 50}, u8"Lưu Thông Tin",
               sf::Color(76, 175, 80), sf::Color::White, canSave, hovered, window);
}
