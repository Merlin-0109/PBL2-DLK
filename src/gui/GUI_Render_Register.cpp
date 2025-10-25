#include "GUI.h"
#include "gui/GuiInternal.h"

void GUI::renderRegisterSelectRole() {
    auto ws = window.getSize();
    float W = static_cast<float>(ws.x);
    float H = static_cast<float>(ws.y);
    sf::RectangleShape titleBar({W, 70});
    titleBar.setPosition({0, 0});
    titleBar.setFillColor(sf::Color(30, 136, 229));
    window.draw(titleBar);
    sf::Text appTitle = makeText(font, u8"Đặt LỊCH KHÁM", 32);
    appTitle.setPosition({40, 18});
    appTitle.setFillColor(sf::Color::White);
    appTitle.setStyle(sf::Text::Bold);
    window.draw(appTitle);
    float panelW = 760.f;
    float panelH = 490.f;
    float panelX = W / 2.f - panelW / 2.f;
    float panelY = 150.f;
    sf::RectangleShape shadow({764, 494});
    shadow.setPosition({panelX + 3.f, panelY + 3.f});
    shadow.setFillColor(sf::Color(200, 200, 200, 80));
    window.draw(shadow);
    sf::RectangleShape panel({panelW, panelH});
    panel.setPosition({panelX, panelY});
    panel.setFillColor(sf::Color::White);
    window.draw(panel);
    sf::Text logo = makeText(font, "+", 80);
    sf::FloatRect logoBounds = logo.getLocalBounds();
    logo.setPosition({W / 2.f - logoBounds.size.x / 2.f, 170});
    logo.setFillColor(sf::Color(30, 136, 229));
    logo.setStyle(sf::Text::Bold);
    window.draw(logo);
    sf::Text title = makeText(font, u8"Đăng Ký Tài Khoản Mới", 32);
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setPosition({W / 2.f - titleBounds.size.x / 2.f, 270});
    title.setFillColor(sf::Color(33, 33, 33));
    title.setStyle(sf::Text::Bold);
    window.draw(title);
    sf::Text subtitle = makeText(font, u8"Vui lòng chọn loại tài khoản", 16);
    sf::FloatRect subtitleBounds = subtitle.getLocalBounds();
    subtitle.setPosition({W / 2.f - subtitleBounds.size.x / 2.f, 315});
    subtitle.setFillColor(sf::Color(100, 100, 100));
    window.draw(subtitle);
    float cardsStartX = panelX + (panelW - (2 * 280.f + 20.f)) / 2.f;
    float cardY = 360;
    float cardWidth = 280;
    float cardHeight = 130;
    float cardSpacing = 20;
    drawRoleCard({cardsStartX, cardY}, {cardWidth, cardHeight}, u8"Bệnh Nhân", 
                 u8"Đặt lịch khám bệnh", "P", 
                 selectedRole == UserRole::PATIENT, window);
    drawRoleCard({cardsStartX + cardWidth + cardSpacing, cardY}, {cardWidth, cardHeight}, u8"Bác Sĩ",
                 u8"Quản lý lịch khám", "D",
                 selectedRole == UserRole::DOCTOR, window);
    sf::Color buttonColor = (selectedRole != UserRole::NONE) ? 
                           sf::Color(76, 175, 80) : sf::Color(189, 189, 189);
    drawButton({W / 2.f - 150.f, 520}, {300, 55}, u8"Tiếp Tục", buttonColor, sf::Color::White, window);
    sf::Text loginLink = makeText(font, u8"Đã có tài khoản? Đăng Nhập", 16);
    sf::FloatRect linkBounds = loginLink.getLocalBounds();
    loginLink.setPosition({W / 2.f - linkBounds.size.x / 2.f, 595});
    loginLink.setFillColor(sf::Color(30, 136, 229));
    window.draw(loginLink);
}

void GUI::renderRegisterInfo() {
    auto ws = window.getSize();
    float W = static_cast<float>(ws.x);
    sf::RectangleShape titleBar({W, 70});
    titleBar.setPosition({0, 0});
    titleBar.setFillColor(sf::Color(30, 136, 229));
    window.draw(titleBar);
    std::string roleText = (selectedRole == UserRole::PATIENT) ? u8"Bệnh Nhân" : u8"Bác Sĩ";
    sf::Text appTitle = makeText(font, u8"Đăng Ký - " + roleText, 28);
    appTitle.setPosition({40, 20});
    appTitle.setFillColor(sf::Color::White);
    appTitle.setStyle(sf::Text::Bold);
    window.draw(appTitle);
    float panelW = 760.f;
    float panelX = W / 2.f - panelW / 2.f;
    sf::RectangleShape panel({panelW, 580});
    panel.setPosition({panelX, 110});
    panel.setFillColor(sf::Color::White);
    window.draw(panel);
    sf::Text title = makeText(font, u8"Thông Tin Đăng Ký", 32);
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setPosition({W / 2.f - titleBounds.size.x / 2.f, 145});
    title.setFillColor(sf::Color(33, 33, 33));
    title.setStyle(sf::Text::Bold);
    window.draw(title);
    sf::Text subtitle = makeText(font, u8"Vui lòng điền đầy đủ thông tin", 16);
    sf::FloatRect subtitleBounds = subtitle.getLocalBounds();
    subtitle.setPosition({W / 2.f - subtitleBounds.size.x / 2.f, 190});
    subtitle.setFillColor(sf::Color(120, 120, 120));
    window.draw(subtitle);
    float startY = 265;
    float spacing = 80;
    float baseX = panelX + 80.f;
    drawInputField({baseX, startY}, {600, 50}, u8"Tên Đăng Nhập *", inputUsername,
                   activeInputField == 0, false, window);
    drawInputField({baseX, startY + spacing}, {600, 50}, u8"Mật Khẩu *", inputPassword,
                   activeInputField == 1, true, window);
    drawInputField({baseX, startY + spacing * 2}, {600, 50}, u8"Xác Nhận Mật Khẩu *", 
                   inputConfirmPassword, activeInputField == 2, true, window);
    drawInputField({baseX, startY + spacing * 3}, {600, 50}, "Email *", inputEmail,
                   activeInputField == 3, false, window);
    drawButton({W / 2.f - 150.f, 600}, {300, 55}, u8"Đăng Ký", 
               sf::Color(76, 175, 80), sf::Color::White, window);
    sf::Text backText = makeText(font, "< Quay Lại", 16);
    sf::FloatRect backBounds = backText.getLocalBounds();
    backText.setPosition({W / 2.f - backBounds.size.x / 2.f, 710});
    backText.setFillColor(sf::Color(30, 136, 229));
    window.draw(backText);
}

void GUI::renderLogin() {
    auto ws = window.getSize();
    float W = static_cast<float>(ws.x);
    sf::RectangleShape titleBar({W, 70});
    titleBar.setPosition({0, 0});
    titleBar.setFillColor(sf::Color(30, 136, 229));
    window.draw(titleBar);
    sf::Text appTitle = makeText(font, u8"Đăng Nhập", 28);
    appTitle.setPosition({40, 20});
    appTitle.setFillColor(sf::Color::White);
    appTitle.setStyle(sf::Text::Bold);
    window.draw(appTitle);
    float panelW = 700.f;
    float panelX = W / 2.f - panelW / 2.f;
    sf::RectangleShape panel({panelW, 450});
    panel.setPosition({panelX, 180});
    panel.setFillColor(sf::Color::White);
    window.draw(panel);
    sf::Text title = makeText(font, u8"Chào Mừng Trở Lại", 32);
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setPosition({W / 2.f - titleBounds.size.x / 2.f, 215});
    title.setFillColor(sf::Color(33, 33, 33));
    title.setStyle(sf::Text::Bold);
    window.draw(title);
    float baseX = panelX + 110.f;
    drawInputField({baseX, 300}, {480, 50}, u8"Tên Đăng Nhập", inputUsername,
                   activeInputField == 0, false, window);
    drawInputField({baseX, 390}, {480, 50}, u8"Mật Khẩu", inputPassword,
                   activeInputField == 1, true, window);
    drawButton({W / 2.f - 100.f, 480}, {200, 55}, u8"Đăng Nhập",
               sf::Color(30, 136, 229), sf::Color::White, window);
    sf::Text registerLink = makeText(font, u8"Chưa có tài khoản? Đăng Ký", 16);
    sf::FloatRect linkBounds = registerLink.getLocalBounds();
    registerLink.setPosition({W / 2.f - linkBounds.size.x / 2.f, 570});
    registerLink.setFillColor(sf::Color(30, 136, 229));
    window.draw(registerLink);
}
