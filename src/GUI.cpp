#include "GUI.h"
#include "DataStore.h"
#include <iostream>
#include <cstring>

// Helper function for SFML 3.x with proper UTF-8 support
static sf::Text makeText(const sf::Font& font, const std::string& str, unsigned int size) {
    sf::Text text(font);
    
    // Convert UTF-8 string to sf::String properly
    text.setString(sf::String::fromUtf8(str.begin(), str.end()));
    text.setCharacterSize(size);
    
    return text;
}

// Helper to create UTF-8 sf::String from u8 literal
static sf::String makeUtf8String(const char* u8str) {
    std::string str(u8str);
    return sf::String::fromUtf8(str.begin(), str.end());
}

GUI::GUI() 
    : window(sf::VideoMode({1280, 800}), 
             makeUtf8String(u8"Đặt Lịch Khám - Hệ Thống Đặt Lịch Khám Bệnh")),
      currentScreen(Screen::REGISTER_SELECT_ROLE),
      selectedRole(UserRole::NONE),
      activeInputField(-1),
      passwordVisible(false),
      showCursor(true) {
    window.setFramerateLimit(60);
}

GUI::~GUI() {}

bool GUI::initialize() {
    // Use Roboto font for best Vietnamese support
    std::vector<std::string> fontPaths = {
        "assets/Roboto.ttf",                 // Roboto - excellent Vietnamese support
        "assets/fonts/arial.ttf",            // Local Arial
        "C:/Windows/Fonts/segoeui.ttf",      // Segoe UI
        "C:/Windows/Fonts/arial.ttf"         // System Arial fallback
    };
    
    bool fontLoaded = false;
    for (const auto& path : fontPaths) {
        if (font.openFromFile(path)) {
            std::cout << "Font loaded successfully: " << path << std::endl;
            fontLoaded = true;
            break;
        } else {
            std::cout << "Failed to load: " << path << std::endl;
        }
    }
    
    if (!fontLoaded) {
        std::cerr << "ERROR: Could not load any font!" << std::endl;
        return false;
    }
    
    Account::ensureDefaultAdminExists();
    DataStore::ensureAppointmentsDirExists();
    return true;
}

void GUI::run() {
    std::cout << "Running main loop..." << std::endl;
    
    // Configure window for smooth performance
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(false);
    
    sf::Clock deltaClock;
    
    while (window.isOpen()) {
        handleEvents();
        render();
    }
}

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

void GUI::handleMouseClick(const sf::Vector2f& mousePos) {
    switch (currentScreen) {
        case Screen::REGISTER_SELECT_ROLE: {
            // Role cards: Patient {340, 360}, Doctor {640, 360}, size {280, 130}
            if (isMouseOverRect(mousePos, {340, 360}, {280, 130})) {
                selectedRole = UserRole::PATIENT;
            }
            else if (isMouseOverRect(mousePos, {640, 360}, {280, 130})) {
                selectedRole = UserRole::DOCTOR;
            }
            // Continue button: {490, 520}, {300, 55}
            else if (isMouseOverRect(mousePos, {490, 520}, {300, 55})) {
                if (selectedRole != UserRole::NONE) {
                    switchToScreen(Screen::REGISTER_INFO);
                }
            }
            // Login link: wider area for easier clicking
            else if (isMouseOverRect(mousePos, {400, 590}, {480, 40})) {
                switchToScreen(Screen::LOGIN);
            }
            break;
        }
        
        case Screen::REGISTER_INFO: {
            // Input fields: startY=265, spacing=80, size={600, 50}
            for (int i = 0; i < 4; i++) {
                float yPos = 265 + i * 80;
                if (isMouseOverRect(mousePos, {340, yPos}, {600, 50})) {
                    activeInputField = i;
                    return;
                }
            }
            // Register button: {490, 600}, {300, 55}
            if (isMouseOverRect(mousePos, {490, 600}, {300, 55})) {
                completeRegistration();
            }
            // Back link: moved below white panel at y=710
            else if (isMouseOverRect(mousePos, {500, 700}, {280, 35})) {
                switchToScreen(Screen::REGISTER_SELECT_ROLE);
            }
            activeInputField = -1;
            break;
        }
        
        case Screen::LOGIN: {
            // Input fields: {400, 300} and {400, 390}, size {480, 50}
            if (isMouseOverRect(mousePos, {400, 300}, {480, 50})) {
                activeInputField = 0;
            }
            else if (isMouseOverRect(mousePos, {400, 390}, {480, 50})) {
                activeInputField = 1;
            }
            // Login button: {540, 480}, {200, 55}
            else if (isMouseOverRect(mousePos, {540, 480}, {200, 55})) {
                completeLogin();
            }
            // Register link: wider clickable area
            else if (isMouseOverRect(mousePos, {400, 560}, {400, 40})) {
                switchToScreen(Screen::REGISTER_SELECT_ROLE);
            }
            else {
                activeInputField = -1;
            }
            break;
        }
        
        case Screen::UPDATE_PATIENT_INFO: {
            for (int i = 0; i < 8; i++) {
                float yPos = 180 + i * 65;
                if (isMouseOverRect(mousePos, {340, yPos}, {600, 45})) {
                    activeInputField = i;
                    return;
                }
            }
            if (isMouseOverRect(mousePos, {440, 730}, {200, 50})) {
                savePatientInfo();
                switchToScreen(Screen::PATIENT_DASHBOARD);
            }
            activeInputField = -1;
            break;
        }
        
        case Screen::UPDATE_DOCTOR_INFO: {
            for (int i = 0; i < 9; i++) {
                float yPos = 160 + i * 60;
                if (isMouseOverRect(mousePos, {340, yPos}, {600, 45})) {
                    activeInputField = i;
                    return;
                }
            }
            if (isMouseOverRect(mousePos, {440, 740}, {200, 50})) {
                saveDoctorInfo();
                switchToScreen(Screen::DOCTOR_DASHBOARD);
            }
            activeInputField = -1;
            break;
        }
        
        case Screen::PATIENT_DASHBOARD: {
            if (isMouseOverRect(mousePos, {440, 350}, {400, 50})) {
                loadPatientInfo();
                switchToScreen(Screen::UPDATE_PATIENT_INFO);
            }
            break;
        }
        
        case Screen::DOCTOR_DASHBOARD: {
            if (isMouseOverRect(mousePos, {440, 350}, {400, 50})) {
                loadDoctorInfo();
                switchToScreen(Screen::UPDATE_DOCTOR_INFO);
            }
            break;
        }
    }
}

// [CONTINUED IN NEXT PART - handleTextInput]
void GUI::handleTextInput(char32_t unicode) {
    if (activeInputField < 0) return;
    
    if (unicode == 8) {  // Backspace
        std::string* target = nullptr;
        switch (currentScreen) {
            case Screen::REGISTER_INFO:
                if (activeInputField == 0) target = &inputUsername;
                else if (activeInputField == 1) target = &inputPassword;
                else if (activeInputField == 2) target = &inputConfirmPassword;
                else if (activeInputField == 3) target = &inputEmail;
                break;
            case Screen::UPDATE_PATIENT_INFO:
                if (activeInputField == 0) target = &inputFullName;
                else if (activeInputField == 1) target = &inputPhone;
                else if (activeInputField == 2) target = &inputEmail;
                else if (activeInputField == 3) target = &inputAddress;
                else if (activeInputField == 4) target = &inputDOB;
                else if (activeInputField == 5) target = &inputGender;
                else if (activeInputField == 6) target = &inputBloodType;
                else if (activeInputField == 7) target = &inputCCCD;
                break;
            case Screen::UPDATE_DOCTOR_INFO:
                if (activeInputField == 0) target = &inputFullName;
                else if (activeInputField == 1) target = &inputPhone;
                else if (activeInputField == 2) target = &inputEmail;
                else if (activeInputField == 3) target = &inputAddress;
                else if (activeInputField == 4) target = &inputDOB;
                else if (activeInputField == 5) target = &inputGender;
                else if (activeInputField == 6) target = &inputCCCD;
                else if (activeInputField == 7) target = &inputSpecialization;
                else if (activeInputField == 8) target = &inputFaculty;
                break;
            case Screen::LOGIN:
                if (activeInputField == 0) target = &inputUsername;
                else if (activeInputField == 1) target = &inputPassword;
                break;
            default: break;
        }
        if (target && !target->empty()) target->pop_back();
        return;
    }
    
    if (unicode >= 32 && unicode < 127) {  // Printable chars
        std::string* target = nullptr;
        switch (currentScreen) {
            case Screen::REGISTER_INFO:
                if (activeInputField == 0) target = &inputUsername;
                else if (activeInputField == 1) target = &inputPassword;
                else if (activeInputField == 2) target = &inputConfirmPassword;
                else if (activeInputField == 3) target = &inputEmail;
                break;
            case Screen::UPDATE_PATIENT_INFO:
                if (activeInputField == 0) target = &inputFullName;
                else if (activeInputField == 1) target = &inputPhone;
                else if (activeInputField == 2) target = &inputEmail;
                else if (activeInputField == 3) target = &inputAddress;
                else if (activeInputField == 4) target = &inputDOB;
                else if (activeInputField == 5) target = &inputGender;
                else if (activeInputField == 6) target = &inputBloodType;
                else if (activeInputField == 7) target = &inputCCCD;
                break;
            case Screen::UPDATE_DOCTOR_INFO:
                if (activeInputField == 0) target = &inputFullName;
                else if (activeInputField == 1) target = &inputPhone;
                else if (activeInputField == 2) target = &inputEmail;
                else if (activeInputField == 3) target = &inputAddress;
                else if (activeInputField == 4) target = &inputDOB;
                else if (activeInputField == 5) target = &inputGender;
                else if (activeInputField == 6) target = &inputCCCD;
                else if (activeInputField == 7) target = &inputSpecialization;
                else if (activeInputField == 8) target = &inputFaculty;
                break;
            case Screen::LOGIN:
                if (activeInputField == 0) target = &inputUsername;
                else if (activeInputField == 1) target = &inputPassword;
                break;
            default: break;
        }
        if (target) *target += static_cast<char>(unicode);
    }
}

void GUI::handleKeyPress(sf::Keyboard::Key key) {
    if (key == sf::Keyboard::Key::Enter) {
        if (currentScreen == Screen::LOGIN) {
            completeLogin();
        } else if (currentScreen == Screen::REGISTER_INFO) {
            completeRegistration();
        }
    }
}

void GUI::render() {
    window.clear(sf::Color(240, 248, 255));  // Light blue background
    
    switch (currentScreen) {
        case Screen::REGISTER_SELECT_ROLE:
            renderRegisterSelectRole();
            break;
        case Screen::REGISTER_INFO:
            renderRegisterInfo();
            break;
        case Screen::LOGIN:
            renderLogin();
            break;
        case Screen::UPDATE_PATIENT_INFO:
            renderUpdatePatientInfo();
            break;
        case Screen::UPDATE_DOCTOR_INFO:
            renderUpdateDoctorInfo();
            break;
        case Screen::PATIENT_DASHBOARD:
            renderPatientDashboard();
            break;
        case Screen::DOCTOR_DASHBOARD:
            renderDoctorDashboard();
            break;
    }
    
    window.display();
}

// [RENDER FUNCTIONS - PART 1]
void GUI::renderRegisterSelectRole() {
    // Title bar - gradient effect with darker blue
    sf::RectangleShape titleBar({1280, 70});
    titleBar.setPosition({0, 0});
    titleBar.setFillColor(sf::Color(30, 136, 229));  // Material Blue
    window.draw(titleBar);
    
    sf::Text appTitle = makeText(font, u8"Đặt LỊCH KHÁM", 32);
    appTitle.setPosition({40, 18});
    appTitle.setFillColor(sf::Color::White);
    appTitle.setStyle(sf::Text::Bold);
    window.draw(appTitle);
    
    // Central panel with shadow effect - better positioning
    sf::RectangleShape shadow({764, 494});
    shadow.setPosition({263, 153});
    shadow.setFillColor(sf::Color(200, 200, 200, 80));
    window.draw(shadow);
    
    sf::RectangleShape panel({760, 490});
    panel.setPosition({260, 150});
    panel.setFillColor(sf::Color::White);
    window.draw(panel);
    
    // Medical cross icon - centered
    sf::Text logo = makeText(font, "+", 80);
    sf::FloatRect logoBounds = logo.getLocalBounds();
    logo.setPosition({640 - logoBounds.size.x / 2, 170});
    logo.setFillColor(sf::Color(30, 136, 229));
    logo.setStyle(sf::Text::Bold);
    window.draw(logo);
    
    // Title with better spacing
    sf::Text title = makeText(font, u8"Đăng Ký Tài Khoản Mới", 32);
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setPosition({640 - titleBounds.size.x / 2, 270});
    title.setFillColor(sf::Color(33, 33, 33));
    title.setStyle(sf::Text::Bold);
    window.draw(title);
    
    // Subtitle
    sf::Text subtitle = makeText(font, u8"Vui lòng chọn loại tài khoản", 16);
    sf::FloatRect subtitleBounds = subtitle.getLocalBounds();
    subtitle.setPosition({640 - subtitleBounds.size.x / 2, 315});
    subtitle.setFillColor(sf::Color(100, 100, 100));
    window.draw(subtitle);
    
    // Role cards - centered in panel (260 + (760-600)/2 = 340)
    float cardsStartX = 340;
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
    
    // Continue button - centered, wider
    sf::Color buttonColor = (selectedRole != UserRole::NONE) ? 
                           sf::Color(76, 175, 80) : sf::Color(189, 189, 189);
    drawButton({490, 520}, {300, 55}, u8"Tiếp Tục", buttonColor, sf::Color::White, window);
    
    // Login link - centered
    sf::Text loginLink = makeText(font, u8"Đã có tài khoản? Đăng Nhập", 16);
    sf::FloatRect linkBounds = loginLink.getLocalBounds();
    loginLink.setPosition({640 - linkBounds.size.x / 2, 595});
    loginLink.setFillColor(sf::Color(30, 136, 229));
    window.draw(loginLink);
}

void GUI::renderRegisterInfo() {
    // Title bar
    sf::RectangleShape titleBar({1280, 70});
    titleBar.setPosition({0, 0});
    titleBar.setFillColor(sf::Color(30, 136, 229));
    window.draw(titleBar);
    
    std::string roleText = (selectedRole == UserRole::PATIENT) ? u8"Bệnh Nhân" : u8"Bác Sĩ";
    sf::Text appTitle = makeText(font, u8"Đăng Ký - " + roleText, 28);
    appTitle.setPosition({40, 20});
    appTitle.setFillColor(sf::Color::White);
    appTitle.setStyle(sf::Text::Bold);
    window.draw(appTitle);
    
    // Enlarged white panel for better visibility
    sf::RectangleShape panel({760, 580});
    panel.setPosition({260, 110});
    panel.setFillColor(sf::Color::White);
    window.draw(panel);
    
    sf::Text title = makeText(font, u8"Thông Tin Đăng Ký", 32);
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setPosition({640 - titleBounds.size.x / 2, 145});
    title.setFillColor(sf::Color(33, 33, 33));
    title.setStyle(sf::Text::Bold);
    window.draw(title);
    
    sf::Text subtitle = makeText(font, u8"Vui lòng điền đầy đủ thông tin", 16);
    sf::FloatRect subtitleBounds = subtitle.getLocalBounds();
    subtitle.setPosition({640 - subtitleBounds.size.x / 2, 190});
    subtitle.setFillColor(sf::Color(120, 120, 120));
    window.draw(subtitle);
    
    // Input fields - simplified to 4 fields
    float startY = 265;
    float spacing = 80;
    
    drawInputField({340, startY}, {600, 50}, u8"Tên Đăng Nhập *", inputUsername,
                   activeInputField == 0, false, window);
    drawInputField({340, startY + spacing}, {600, 50}, u8"Mật Khẩu *", inputPassword,
                   activeInputField == 1, true, window);
    drawInputField({340, startY + spacing * 2}, {600, 50}, u8"Xác Nhận Mật Khẩu *", 
                   inputConfirmPassword, activeInputField == 2, true, window);
    drawInputField({340, startY + spacing * 3}, {600, 50}, "Email *", inputEmail,
                   activeInputField == 3, false, window);
    
    // Register button - centered
    drawButton({490, 600}, {300, 55}, u8"Đăng Ký", 
               sf::Color(76, 175, 80), sf::Color::White, window);
    
    // Back link - moved below white panel
    sf::Text backText = makeText(font, "< Quay Lại", 16);
    sf::FloatRect backBounds = backText.getLocalBounds();
    backText.setPosition({640 - backBounds.size.x / 2, 710});
    backText.setFillColor(sf::Color(30, 136, 229));
    window.draw(backText);
}

// [CONTINUED - PART 2]
void GUI::renderLogin() {
    sf::RectangleShape titleBar({1280, 70});
    titleBar.setPosition({0, 0});
    titleBar.setFillColor(sf::Color(30, 136, 229));
    window.draw(titleBar);
    
    sf::Text appTitle = makeText(font, u8"Đăng Nhập", 28);
    appTitle.setPosition({40, 20});
    appTitle.setFillColor(sf::Color::White);
    appTitle.setStyle(sf::Text::Bold);
    window.draw(appTitle);
    
    // Enlarged white panel for better visibility
    sf::RectangleShape panel({700, 450});
    panel.setPosition({290, 180});
    panel.setFillColor(sf::Color::White);
    window.draw(panel);
    
    sf::Text title = makeText(font, u8"Chào Mừng Trở Lại", 32);
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setPosition({640 - titleBounds.size.x / 2, 215});
    title.setFillColor(sf::Color(33, 33, 33));
    title.setStyle(sf::Text::Bold);
    window.draw(title);
    
    drawInputField({400, 300}, {480, 50}, u8"Tên Đăng Nhập", inputUsername,
                   activeInputField == 0, false, window);
    drawInputField({400, 390}, {480, 50}, u8"Mật Khẩu", inputPassword,
                   activeInputField == 1, true, window);
    
    drawButton({540, 480}, {200, 55}, u8"Đăng Nhập",
               sf::Color(30, 136, 229), sf::Color::White, window);
    
    sf::Text registerLink = makeText(font, u8"Chưa có tài khoản? Đăng Ký", 16);
    sf::FloatRect linkBounds = registerLink.getLocalBounds();
    registerLink.setPosition({640 - linkBounds.size.x / 2, 570});
    registerLink.setFillColor(sf::Color(30, 136, 229));
    window.draw(registerLink);
}

void GUI::renderUpdatePatientInfo() {
    sf::RectangleShape titleBar({1280, 70});
    titleBar.setPosition({0, 0});
    titleBar.setFillColor(sf::Color(30, 136, 229));
    window.draw(titleBar);
    
    sf::Text appTitle = makeText(font, u8"Cập Nhật Thông Tin Bệnh Nhân", 28);
    appTitle.setPosition({40, 20});
    appTitle.setFillColor(sf::Color::White);
    appTitle.setStyle(sf::Text::Bold);
    window.draw(appTitle);
    
    sf::RectangleShape panel({720, 650});
    panel.setPosition({280, 100});
    panel.setFillColor(sf::Color::White);
    window.draw(panel);
    
    sf::Text title = makeText(font, u8"Thông Tin Ca Nhan", 28);
    title.setPosition({490, 130});
    title.setFillColor(sf::Color(33, 33, 33));
    title.setStyle(sf::Text::Bold);
    window.draw(title);
    
    float startY = 180;
    float spacing = 65;
    
    drawInputField({340, startY}, {600, 50}, u8"Họ và tên *", inputFullName,
                   activeInputField == 0, false, window);
    drawInputField({340, startY + spacing}, {600, 50}, u8"Số điện thoại *", inputPhone,
                   activeInputField == 1, false, window);
    drawInputField({340, startY + spacing * 2}, {600, 50}, "Email *", inputEmail,
                   activeInputField == 2, false, window);
    drawInputField({340, startY + spacing * 3}, {600, 50}, u8"Địa chỉ", inputAddress,
                   activeInputField == 3, false, window);
    drawInputField({340, startY + spacing * 4}, {600, 50}, u8"Ngày sinh (DD/MM/YYYY)", inputDOB,
                   activeInputField == 4, false, window);
    drawInputField({340, startY + spacing * 5}, {600, 50}, u8"Giới tính", inputGender,
                   activeInputField == 5, false, window);
    drawInputField({340, startY + spacing * 6}, {600, 50}, u8"Nhóm máu", inputBloodType,
                   activeInputField == 6, false, window);
    drawInputField({340, startY + spacing * 7}, {600, 50}, "CCCD", inputCCCD,
                   activeInputField == 7, false, window);
    
    drawButton({440, 730}, {200, 55}, "Luu Thông Tin",
               sf::Color(76, 175, 80), sf::Color::White, window);
}

void GUI::renderUpdateDoctorInfo() {
    sf::RectangleShape titleBar({1280, 70});
    titleBar.setPosition({0, 0});
    titleBar.setFillColor(sf::Color(30, 136, 229));
    window.draw(titleBar);
    
    sf::Text appTitle = makeText(font, u8"Cập Nhật Thông Tin Bác Sĩ", 28);
    appTitle.setPosition({40, 20});
    appTitle.setFillColor(sf::Color::White);
    appTitle.setStyle(sf::Text::Bold);
    window.draw(appTitle);
    
    sf::RectangleShape panel({720, 690});
    panel.setPosition({280, 90});
    panel.setFillColor(sf::Color::White);
    window.draw(panel);
    
    sf::Text title = makeText(font, u8"Thông Tin Ca Nhan", 28);
    title.setPosition({490, 120});
    title.setFillColor(sf::Color(33, 33, 33));
    title.setStyle(sf::Text::Bold);
    window.draw(title);
    
    float startY = 160;
    float spacing = 60;
    
    drawInputField({340, startY}, {600, 50}, u8"Họ và tên *", inputFullName,
                   activeInputField == 0, false, window);
    drawInputField({340, startY + spacing}, {600, 50}, u8"Số điện thoại *", inputPhone,
                   activeInputField == 1, false, window);
    drawInputField({340, startY + spacing * 2}, {600, 50}, "Email *", inputEmail,
                   activeInputField == 2, false, window);
    drawInputField({340, startY + spacing * 3}, {600, 50}, u8"Địa chỉ", inputAddress,
                   activeInputField == 3, false, window);
    drawInputField({340, startY + spacing * 4}, {600, 50}, u8"Ngày sinh (DD/MM/YYYY)", inputDOB,
                   activeInputField == 4, false, window);
    drawInputField({340, startY + spacing * 5}, {600, 50}, u8"Giới tính", inputGender,
                   activeInputField == 5, false, window);
    drawInputField({340, startY + spacing * 6}, {600, 50}, "CCCD", inputCCCD,
                   activeInputField == 6, false, window);
    drawInputField({340, startY + spacing * 7}, {600, 50}, u8"Chuyên môn *", inputSpecialization,
                   activeInputField == 7, false, window);
    drawInputField({340, startY + spacing * 8}, {600, 50}, "Khoa *", inputFaculty,
                   activeInputField == 8, false, window);
    
    drawButton({440, 740}, {200, 55}, "Luu Thông Tin",
               sf::Color(76, 175, 80), sf::Color::White, window);
}

void GUI::renderPatientDashboard() {
    sf::RectangleShape titleBar({1280, 70});
    titleBar.setPosition({0, 0});
    titleBar.setFillColor(sf::Color(30, 136, 229));
    window.draw(titleBar);
    
    sf::Text appTitle = makeText(font, "Dashboard - Bệnh Nhân", 28);
    appTitle.setPosition({40, 20});
    appTitle.setFillColor(sf::Color::White);
    appTitle.setStyle(sf::Text::Bold);
    window.draw(appTitle);
    
    sf::Text welcomeText = makeText(font, u8"Chào Mừng, " + inputUsername + "!", 32);
    welcomeText.setPosition({400, 200});
    welcomeText.setFillColor(sf::Color(33, 33, 33));
    welcomeText.setStyle(sf::Text::Bold);
    window.draw(welcomeText);
    
    sf::Text idText = makeText(font, "ID: " + currentUserId, 20);
    idText.setPosition({400, 250});
    idText.setFillColor(sf::Color(100, 100, 100));
    window.draw(idText);
    
    drawButton({440, 350}, {400, 55}, u8"Cập Nhật Thông Tin Ca Nhan",
               sf::Color(30, 136, 229), sf::Color::White, window);
}

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
    
    drawButton({440, 350}, {400, 55}, u8"Cập Nhật Thông Tin Ca Nhan",
               sf::Color(30, 136, 229), sf::Color::White, window);
}

// Helper drawing functions
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

bool GUI::isMouseOverRect(const sf::Vector2f& mousePos, const sf::Vector2f& rectPos,
                          const sf::Vector2f& rectSize) {
    return mousePos.x >= rectPos.x && mousePos.x <= rectPos.x + rectSize.x &&
           mousePos.y >= rectPos.y && mousePos.y <= rectPos.y + rectSize.y;
}

void GUI::drawInputField(sf::Vector2f position, sf::Vector2f size, const std::string& label,
                        const std::string& value, bool isActive, bool isPassword,
                        sf::RenderWindow& win) {
    sf::Text labelText = makeText(font, label, 14);
    labelText.setPosition({position.x, position.y - 25});
    labelText.setFillColor(sf::Color(80, 80, 80));
    win.draw(labelText);
    
    sf::RectangleShape inputBox(size);
    inputBox.setPosition(position);
    inputBox.setFillColor(sf::Color(250, 250, 250));
    inputBox.setOutlineThickness(2);
    inputBox.setOutlineColor(isActive ? sf::Color(30, 136, 229) : sf::Color(220, 220, 220));
    win.draw(inputBox);
    
    std::string displayValue = value;
    if (isPassword && !value.empty()) {
        displayValue = std::string(value.length(), '*');
    }
    
    sf::Text valueText = makeText(font, displayValue, 16);
    valueText.setPosition({position.x + 15, position.y + 15});
    valueText.setFillColor(sf::Color(50, 50, 50));
    win.draw(valueText);
    
    if (isActive && showCursor) {
        sf::FloatRect bounds = valueText.getLocalBounds();
        sf::RectangleShape cursor({2, 30});
        cursor.setPosition({position.x + 15 + bounds.size.x + 2, position.y + 10});
        cursor.setFillColor(sf::Color(50, 50, 50));
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

void GUI::switchToScreen(Screen newScreen) {
    currentScreen = newScreen;
    activeInputField = -1;
    
    if (newScreen == Screen::REGISTER_SELECT_ROLE) {
        resetInputFields();
        selectedRole = UserRole::NONE;
    }
}

void GUI::resetInputFields() {
    inputUsername.clear();
    inputPassword.clear();
    inputConfirmPassword.clear();
    inputEmail.clear();
    inputFullName.clear();
    inputPhone.clear();
    inputAddress.clear();
    inputDOB.clear();
    inputGender.clear();
    inputBloodType.clear();
    inputCCCD.clear();
    inputSpecialization.clear();
    inputFaculty.clear();
}

void GUI::completeRegistration() {
    // Validate empty fields
    if (inputUsername.empty() || inputPassword.empty() || inputEmail.empty()) {
        std::cout << u8"❌ Lỗi: Vui lòng điền đầy đủ thông tin!" << std::endl;
        return;
    }
    
    // Validate password match
    if (inputPassword != inputConfirmPassword) {
        std::cout << u8"❌ Lỗi: Mật khẩu không khớp!" << std::endl;
        return;
    }
    
    // Validate username length
    if (inputUsername.length() < 3) {
        std::cout << u8"❌ Lỗi: Tên đăng nhập phải có ít nhất 3 ký tự!" << std::endl;
        return;
    }
    
    // Validate password length
    if (inputPassword.length() < 6) {
        std::cout << u8"❌ Lỗi: Mật khẩu phải có ít nhất 6 ký tự!" << std::endl;
        return;
    }
    
    std::cout << u8"⏳ Đang đăng ký..." << std::endl;
    
    std::string role = (selectedRole == UserRole::PATIENT) ? "Patient" : "Doctor";
    std::string generatedId;
    
    bool success = accountSystem.registerWithCredentials(inputUsername, inputPassword, role, generatedId);
    
    if (success) {
        currentUserId = generatedId;
        currentUserRole = role;
        
        // Save email to account file
        std::ofstream emailFile("data/" + role + "/" + generatedId + "_email.txt");
        if (emailFile.is_open()) {
            emailFile << inputEmail;
            emailFile.close();
        }
        
        std::cout << u8"✅ Đăng ký thành công! ID: " << generatedId << std::endl;
        
        // Clear password fields for security
        inputPassword.clear();
        inputConfirmPassword.clear();
        
        // After registration, go to update info screen
        if (selectedRole == UserRole::PATIENT) {
            switchToScreen(Screen::UPDATE_PATIENT_INFO);
        } else {
            switchToScreen(Screen::UPDATE_DOCTOR_INFO);
        }
    } else {
        std::cout << u8"❌ Đăng ký thất bại! Tên đăng nhập đã tồn tại." << std::endl;
    }
}

bool GUI::validateRegistrationInputs() {
    return !inputUsername.empty() && !inputPassword.empty() && 
           !inputEmail.empty() && (inputPassword == inputConfirmPassword);
}

void GUI::completeLogin() {
    // Validate empty fields
    if (inputUsername.empty() || inputPassword.empty()) {
        std::cout << u8"❌ Lỗi: Vui lòng nhập tên đăng nhập và mật khẩu!" << std::endl;
        return;
    }
    
    std::cout << u8"⏳ Đang đăng nhập..." << std::endl;
    
    std::string outRole, outId;
    bool success = accountSystem.loginWithCredentials(inputUsername, inputPassword, outRole, outId);
    
    if (success) {
        currentUserId = outId;
        currentUserRole = outRole;
        
        std::cout << u8"✅ Đăng nhập thành công! Role: " << outRole << ", ID: " << outId << std::endl;
        
        // Clear password for security
        inputPassword.clear();
        
        if (outRole == "Patient") {
            switchToScreen(Screen::PATIENT_DASHBOARD);
        } else if (outRole == "Doctor") {
            switchToScreen(Screen::DOCTOR_DASHBOARD);
        } else {
            switchToScreen(Screen::PATIENT_DASHBOARD);
        }
    } else {
        std::cout << u8"Đăng nhập thất bại!" << std::endl;
    }
}

void GUI::loadPatientInfo() {
    DataStore::PatientInfo info = DataStore::readPatientInfo(currentUserId);
    inputFullName = info.name;
    inputPhone = info.phone;
    inputEmail = info.email;
    inputAddress = info.address;
    inputDOB = info.dateOfBirth;
    inputGender = info.gender;
}

void GUI::savePatientInfo() {
    DataStore::PatientInfo info;
    info.name = inputFullName;
    info.phone = inputPhone;
    info.email = inputEmail;
    info.address = inputAddress;
    info.dateOfBirth = inputDOB;
    info.gender = inputGender;
    
    if (DataStore::writePatientInfo(currentUserId, info)) {
        std::cout << u8"Lưu thông tin thành công!" << std::endl;
    } else {
        std::cout << u8"Lưu thông tin thất bại!" << std::endl;
    }
}

void GUI::loadDoctorInfo() {
    DataStore::DoctorInfo info = DataStore::readDoctorInfo(currentUserId);
    inputFullName = info.name;
    inputPhone = info.phone;
    inputEmail = info.email;
    inputSpecialization = info.specialization;
    inputFaculty = info.experience;
}

void GUI::saveDoctorInfo() {
    DataStore::DoctorInfo info;
    info.name = inputFullName;
    info.phone = inputPhone;
    info.email = inputEmail;
    info.specialization = inputSpecialization;
    info.experience = inputFaculty;
    
    if (DataStore::writeDoctorInfo(currentUserId, info)) {
        std::cout << u8"Lưu thông tin thành công!" << std::endl;
    } else {
        std::cout << u8"Lưu thông tin thất bại!" << std::endl;
    }
}


