// ============================================
// File: src/GUI.cpp - SFML 3.0 Implementation
// ============================================
#include "GUI.h"
#include <iostream>

// Button Implementation
Button::Button(float x, float y, float width, float height, 
               const std::string& label, const sf::Font& font) 
    : text(font, label, 18) {
    
    shape.setPosition({x, y});
    shape.setSize({width, height});
    normalColor = sf::Color(102, 126, 234);
    hoverColor = sf::Color(118, 75, 162);
    shape.setFillColor(normalColor);
    
    text.setFillColor(sf::Color::White);
    
    auto textBounds = text.getLocalBounds();
    text.setOrigin({textBounds.size.x/2.0f, textBounds.size.y/2.0f});
    text.setPosition({x + width/2.0f, y + height/2.0f});
    
    isHovered = false;
}

void Button::setOnClick(std::function<void()> callback) {
    onClick = callback;
}

void Button::handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::FloatRect bounds = shape.getGlobalBounds();
    
    isHovered = bounds.contains(sf::Vector2f(mousePos));
    
    if (const auto* mouseReleased = event.getIf<sf::Event::MouseButtonReleased>()) {
        if (mouseReleased->button == sf::Mouse::Button::Left && isHovered) {
            if (onClick) onClick();
        }
    }
}

void Button::update() {
    shape.setFillColor(isHovered ? hoverColor : normalColor);
}

void Button::draw(sf::RenderWindow& window) {
    window.draw(shape);
    window.draw(text);
}

// InputBox Implementation
InputBox::InputBox(float x, float y, float width, float height, 
                   const std::string& labelText, const sf::Font& font, bool password)
    : text(font, "", 16), label(font, labelText, 16), fontPtr(&font) {
    
    box.setPosition({x, y});
    box.setSize({width, height});
    box.setFillColor(sf::Color::White);
    box.setOutlineThickness(2);
    box.setOutlineColor(sf::Color(200, 200, 200));
    
    text.setFillColor(sf::Color::Black);
    text.setPosition({x + 10, y + height/2 - 10});
    
    label.setFillColor(sf::Color(50, 50, 50));
    label.setPosition({x, y - 25});
    
    isFocused = false;
    isPassword = password;
    content = "";
    showCursor = true;
}

void InputBox::handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
    if (const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>()) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        isFocused = box.getGlobalBounds().contains(sf::Vector2f(mousePos));
        box.setOutlineColor(isFocused ? sf::Color(102, 126, 234) : sf::Color(200, 200, 200));
    }
    
    if (isFocused) {
        if (const auto* textEntered = event.getIf<sf::Event::TextEntered>()) {
            if (textEntered->unicode == 8) { // Backspace
                if (!content.empty()) {
                    content.pop_back();
                }
            } else if (textEntered->unicode < 128 && textEntered->unicode >= 32) {
                content += static_cast<char>(textEntered->unicode);
            }
            
            text.setString(isPassword ? std::string(content.length(), '*') : content);
        }
    }
}

void InputBox::update() {
    if (blinkClock.getElapsedTime().asSeconds() > 0.5f) {
        showCursor = !showCursor;
        blinkClock.restart();
    }
}

void InputBox::draw(sf::RenderWindow& window) {
    window.draw(label);
    window.draw(box);
    window.draw(text);
    
    if (isFocused && showCursor) {
        sf::RectangleShape cursor({2, 20});
        cursor.setFillColor(sf::Color::Black);
        auto textBounds = text.getGlobalBounds();
        cursor.setPosition({textBounds.position.x + textBounds.size.x + 2, text.getPosition().y});
        window.draw(cursor);
    }
}

void InputBox::clear() {
    content = "";
    text.setString("");
}

void InputBox::setContent(const std::string& str) {
    content = str;
    text.setString(isPassword ? std::string(str.length(), '*') : str);
}

// MessageBox Implementation
MessageBox::MessageBox(const sf::Font& font) : title(font, "", 24), message(font, "", 18), fontPtr(&font) {
    background.setSize({1200, 800});
    background.setFillColor(sf::Color(0, 0, 0, 150));
    
    box.setSize({450, 220});
    box.setPosition({375, 290});
    box.setFillColor(sf::Color::White);
    box.setOutlineThickness(3);
    
    title.setFillColor(sf::Color::Black);
    title.setPosition({400, 310});
    
    message.setFillColor(sf::Color(50, 50, 50));
    message.setPosition({400, 360});
    
    okButton = new Button(500, 440, 200, 45, "OK", font);
    okButton->setOnClick([this]() { this->hide(); });
    
    isVisible = false;
}

MessageBox::~MessageBox() {
    delete okButton;
}

void MessageBox::show(const std::string& titleText, const std::string& msg, bool success) {
    title.setString(titleText);
    message.setString(msg);
    box.setOutlineColor(success ? sf::Color(40, 167, 69) : sf::Color(220, 53, 69));
    isVisible = true;
}

void MessageBox::hide() {
    isVisible = false;
}

void MessageBox::handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
    if (isVisible) {
        okButton->handleEvent(event, window);
        
        if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::Enter || 
                keyPressed->code == sf::Keyboard::Key::Escape) {
                hide();
            }
        }
    }
}

void MessageBox::update() {
    if (isVisible) {
        okButton->update();
    }
}

void MessageBox::draw(sf::RenderWindow& window) {
    if (isVisible) {
        window.draw(background);
        window.draw(box);
        window.draw(title);
        window.draw(message);
        okButton->draw(window);
    }
}

// HospitalGUI Implementation
HospitalGUI::HospitalGUI() 
    : window(sf::VideoMode({1200, 800}), "Datchikham - He Thong Quan Ly Benh Vien"),
      currentScreen(Screen::MAIN_MENU),
      isLoggedIn(false) {
    
    if (!font.openFromFile("assets/arial.ttf")) {
        std::cerr << "Error: Cannot load font from assets/arial.ttf" << std::endl;
        throw std::runtime_error("Font loading failed");
    }
    
    window.setFramerateLimit(60);
    Account::ensureDefaultAdminExists();
    DataStore::ensureAppointmentsDirExists();
    
    messageBox = std::make_unique<MessageBox>(font);
    setupMainMenu();
}

void HospitalGUI::setupMainMenu() {
    buttons.clear();
    inputBoxes.clear();
    
    auto loginBtn = std::make_unique<Button>(450, 300, 300, 60, "DANG NHAP", font);
    loginBtn->setOnClick([this]() {
        currentScreen = Screen::LOGIN;
        setupLoginScreen();
    });
    buttons.push_back(std::move(loginBtn));
    
    auto registerBtn = std::make_unique<Button>(450, 380, 300, 60, "DANG KY", font);
    registerBtn->setOnClick([this]() {
        currentScreen = Screen::REGISTER;
        setupRegisterScreen();
    });
    buttons.push_back(std::move(registerBtn));
    
    auto exitBtn = std::make_unique<Button>(450, 460, 300, 60, "THOAT", font);
    exitBtn->setOnClick([this]() {
        window.close();
    });
    buttons.push_back(std::move(exitBtn));
}

void HospitalGUI::setupLoginScreen() {
    buttons.clear();
    inputBoxes.clear();
    
    auto usernameBox = std::make_unique<InputBox>(400, 250, 400, 45, "Ten dang nhap:", font);
    inputBoxes.push_back(std::move(usernameBox));
    
    auto passwordBox = std::make_unique<InputBox>(400, 340, 400, 45, "Mat khau:", font, true);
    inputBoxes.push_back(std::move(passwordBox));
    
    auto loginBtn = std::make_unique<Button>(400, 440, 190, 50, "DANG NHAP", font);
    loginBtn->setOnClick([this]() { performLogin(); });
    buttons.push_back(std::move(loginBtn));
    
    auto backBtn = std::make_unique<Button>(610, 440, 190, 50, "QUAY LAI", font);
    backBtn->setOnClick([this]() {
        currentScreen = Screen::MAIN_MENU;
        setupMainMenu();
    });
    buttons.push_back(std::move(backBtn));
}

void HospitalGUI::setupRegisterScreen() {
    buttons.clear();
    inputBoxes.clear();
    
    auto usernameBox = std::make_unique<InputBox>(400, 220, 400, 45, "Ten dang nhap:", font);
    inputBoxes.push_back(std::move(usernameBox));
    
    auto passwordBox = std::make_unique<InputBox>(400, 300, 400, 45, "Mat khau:", font, true);
    inputBoxes.push_back(std::move(passwordBox));
    
    auto confirmBox = std::make_unique<InputBox>(400, 380, 400, 45, "Xac nhan mat khau:", font, true);
    inputBoxes.push_back(std::move(confirmBox));
    
    auto registerBtn = std::make_unique<Button>(400, 480, 190, 50, "DANG KY", font);
    registerBtn->setOnClick([this]() { performRegister(); });
    buttons.push_back(std::move(registerBtn));
    
    auto backBtn = std::make_unique<Button>(610, 480, 190, 50, "QUAY LAI", font);
    backBtn->setOnClick([this]() {
        currentScreen = Screen::MAIN_MENU;
        setupMainMenu();
    });
    buttons.push_back(std::move(backBtn));
}

void HospitalGUI::setupPatientMenu() {
    buttons.clear();
    inputBoxes.clear();
    
    auto bookBtn = std::make_unique<Button>(350, 250, 500, 60, "DAT LICH KHAM", font);
    bookBtn->setOnClick([this]() {
        currentScreen = Screen::BOOK_APPOINTMENT;
        setupBookAppointment();
    });
    buttons.push_back(std::move(bookBtn));
    
    auto viewBtn = std::make_unique<Button>(350, 330, 500, 60, "XEM LICH KHAM", font);
    viewBtn->setOnClick([this]() {
        currentScreen = Screen::VIEW_APPOINTMENTS;
        setupViewAppointments();
    });
    buttons.push_back(std::move(viewBtn));
    
    auto profileBtn = std::make_unique<Button>(350, 410, 500, 60, "CAP NHAT THONG TIN", font);
    profileBtn->setOnClick([this]() {
        currentScreen = Screen::UPDATE_PROFILE;
        setupUpdateProfile();
    });
    buttons.push_back(std::move(profileBtn));
    
    auto logoutBtn = std::make_unique<Button>(350, 490, 500, 60, "DANG XUAT", font);
    logoutBtn->setOnClick([this]() {
        isLoggedIn = false;
        currentScreen = Screen::MAIN_MENU;
        setupMainMenu();
    });
    buttons.push_back(std::move(logoutBtn));
}

void HospitalGUI::setupDoctorMenu() {
    buttons.clear();
    inputBoxes.clear();
    
    auto viewBtn = std::make_unique<Button>(350, 280, 500, 60, "XEM LICH HEN", font);
    viewBtn->setOnClick([this]() {
        currentScreen = Screen::VIEW_APPOINTMENTS;
        setupViewAppointments();
    });
    buttons.push_back(std::move(viewBtn));
    
    auto profileBtn = std::make_unique<Button>(350, 360, 500, 60, "CAP NHAT THONG TIN", font);
    profileBtn->setOnClick([this]() {
        currentScreen = Screen::UPDATE_PROFILE;
        setupUpdateProfile();
    });
    buttons.push_back(std::move(profileBtn));
    
    auto logoutBtn = std::make_unique<Button>(350, 440, 500, 60, "DANG XUAT", font);
    logoutBtn->setOnClick([this]() {
        isLoggedIn = false;
        currentScreen = Screen::MAIN_MENU;
        setupMainMenu();
    });
    buttons.push_back(std::move(logoutBtn));
}

void HospitalGUI::setupBookAppointment() {
    buttons.clear();
    inputBoxes.clear();
    
    messageBox->show("Dat Lich Kham", "Chuc nang dang phat trien...", true);
    
    auto backBtn = std::make_unique<Button>(500, 600, 200, 50, "QUAY LAI", font);
    backBtn->setOnClick([this]() {
        currentScreen = Screen::PATIENT_MENU;
        setupPatientMenu();
    });
    buttons.push_back(std::move(backBtn));
}

void HospitalGUI::setupViewAppointments() {
    buttons.clear();
    inputBoxes.clear();
    
    auto backBtn = std::make_unique<Button>(500, 600, 200, 50, "QUAY LAI", font);
    backBtn->setOnClick([this]() {
        if (currentRole == "Patient") {
            currentScreen = Screen::PATIENT_MENU;
            setupPatientMenu();
        } else {
            currentScreen = Screen::DOCTOR_MENU;
            setupDoctorMenu();
        }
    });
    buttons.push_back(std::move(backBtn));
}

void HospitalGUI::setupUpdateProfile() {
    buttons.clear();
    inputBoxes.clear();
    
    auto backBtn = std::make_unique<Button>(500, 600, 200, 50, "QUAY LAI", font);
    backBtn->setOnClick([this]() {
        if (currentRole == "Patient") {
            currentScreen = Screen::PATIENT_MENU;
            setupPatientMenu();
        } else {
            currentScreen = Screen::DOCTOR_MENU;
            setupDoctorMenu();
        }
    });
    buttons.push_back(std::move(backBtn));
}

void HospitalGUI::performLogin() {
    if (inputBoxes.size() < 2) return;
    
    std::string username = inputBoxes[0]->getContent();
    std::string password = inputBoxes[1]->getContent();
    
    if (username.empty() || password.empty()) {
        messageBox->show("Loi", "Vui long nhap day du thong tin!", false);
        return;
    }
    
    if (account.login(currentRole, currentId)) {
        isLoggedIn = true;
        messageBox->show("Thanh Cong", "Dang nhap thanh cong!", true);
        
        if (currentRole == "Patient") {
            currentScreen = Screen::PATIENT_MENU;
            setupPatientMenu();
        } else if (currentRole == "Doctor") {
            currentScreen = Screen::DOCTOR_MENU;
            setupDoctorMenu();
        }
    } else {
        messageBox->show("Loi", "Dang nhap that bai!", false);
    }
}

void HospitalGUI::performRegister() {
    if (inputBoxes.size() < 3) return;
    
    std::string username = inputBoxes[0]->getContent();
    std::string password = inputBoxes[1]->getContent();
    std::string confirm = inputBoxes[2]->getContent();
    
    if (username.empty() || password.empty() || confirm.empty()) {
        messageBox->show("Loi", "Vui long nhap day du thong tin!", false);
        return;
    }
    
    if (password != confirm) {
        messageBox->show("Loi", "Mat khau xac nhan khong khop!", false);
        return;
    }
    
    account.registerAccount();
    messageBox->show("Thanh Cong", "Dang ky tai khoan thanh cong!", true);
    currentScreen = Screen::MAIN_MENU;
    setupMainMenu();
}

void HospitalGUI::handleEvents() {
    while (std::optional<sf::Event> event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
        }
        
        if (messageBox->visible()) {
            messageBox->handleEvent(*event, window);
            continue;
        }
        
        for (auto& button : buttons) {
            button->handleEvent(*event, window);
        }
        
        for (auto& inputBox : inputBoxes) {
            inputBox->handleEvent(*event, window);
        }
    }
}

void HospitalGUI::update() {
    messageBox->update();
    
    for (auto& button : buttons) {
        button->update();
    }
    
    for (auto& inputBox : inputBoxes) {
        inputBox->update();
    }
}

void HospitalGUI::render() {
    window.clear(sf::Color(245, 245, 250));
    
    drawHeader();
    drawScreenTitle("");
    
    for (auto& inputBox : inputBoxes) {
        inputBox->draw(window);
    }
    
    for (auto& button : buttons) {
        button->draw(window);
    }
    
    messageBox->draw(window);
    
    window.display();
}

void HospitalGUI::drawHeader() {
    sf::RectangleShape header({1200, 120});
    header.setFillColor(sf::Color(102, 126, 234));
    window.draw(header);
    
    sf::Text title(font, "DATCHIKHAM", 48);
    title.setPosition({420, 20});
    title.setFillColor(sf::Color::White);
    window.draw(title);
    
    sf::Text subtitle(font, "He Thong Quan Ly Benh Vien", 20);
    subtitle.setPosition({420, 75});
    subtitle.setFillColor(sf::Color(230, 230, 255));
    window.draw(subtitle);
    
    if (isLoggedIn) {
        sf::Text userInfo(font, "User: " + currentId + " (" + currentRole + ")", 16);
        userInfo.setPosition({950, 90});
        userInfo.setFillColor(sf::Color::White);
        window.draw(userInfo);
    }
}

void HospitalGUI::drawScreenTitle(const std::string& title) {
    sf::Text screenTitle(font, "", 32);
    screenTitle.setFillColor(sf::Color(50, 50, 50));
    screenTitle.setPosition({400, 160});
    
    switch (currentScreen) {
        case Screen::MAIN_MENU:
            screenTitle.setString("MENU CHINH");
            break;
        case Screen::LOGIN:
            screenTitle.setString("DANG NHAP");
            break;
        case Screen::REGISTER:
            screenTitle.setString("DANG KY TAI KHOAN");
            break;
        case Screen::PATIENT_MENU:
            screenTitle.setString("MENU BENH NHAN");
            break;
        case Screen::DOCTOR_MENU:
            screenTitle.setString("MENU BAC SI");
            break;
        case Screen::BOOK_APPOINTMENT:
            screenTitle.setString("DAT LICH KHAM");
            break;
        case Screen::VIEW_APPOINTMENTS:
            screenTitle.setString("LICH KHAM");
            break;
        case Screen::UPDATE_PROFILE:
            screenTitle.setString("CAP NHAT THONG TIN");
            break;
    }
    
    window.draw(screenTitle);
}

void HospitalGUI::run() {
    while (window.isOpen()) {
        handleEvents();
        update();
        render();
    }
}
