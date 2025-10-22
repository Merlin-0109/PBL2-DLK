// ============================================
// File: src/GUI.cpp - Implementation
// ============================================
#include "GUI.h"
#include <iostream>
#include <cmath>

// ModernButton Implementation
ModernButton::ModernButton(float x, float y, float width, float height, 
                           const std::string& label, const sf::Font& font, sf::Color color)
    : text(font, label, 16), cornerRadius(8.0f) {
    
    shape.setPosition({x, y});
    shape.setSize({width, height});
    normalColor = color;
    hoverColor = sf::Color(color.r + 20, color.g + 20, color.b + 20);
    pressColor = sf::Color(color.r - 20, color.g - 20, color.b - 20);
    shape.setFillColor(normalColor);
    
    text.setFillColor(sf::Color::White);
    auto textBounds = text.getLocalBounds();
    text.setOrigin({textBounds.size.x/2.0f, textBounds.size.y/2.0f});
    text.setPosition({x + width/2.0f, y + height/2.0f});
    
    isHovered = false;
    isPressed = false;
}

void ModernButton::setOnClick(std::function<void()> callback) {
    onClick = callback;
}

void ModernButton::handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::FloatRect bounds = shape.getGlobalBounds();
    
    isHovered = bounds.contains(sf::Vector2f(mousePos));
    
    if (const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mousePressed->button == sf::Mouse::Button::Left && isHovered) {
            isPressed = true;
        }
    }
    
    if (const auto* mouseReleased = event.getIf<sf::Event::MouseButtonReleased>()) {
        if (mouseReleased->button == sf::Mouse::Button::Left) {
            if (isPressed && isHovered && onClick) {
                onClick();
            }
            isPressed = false;
        }
    }
}

void ModernButton::update() {
    if (isPressed) {
        shape.setFillColor(pressColor);
    } else if (isHovered) {
        shape.setFillColor(hoverColor);
    } else {
        shape.setFillColor(normalColor);
    }
}

void ModernButton::draw(sf::RenderWindow& window) {
    window.draw(shape);
    window.draw(text);
}

// ModernInputBox Implementation
ModernInputBox::ModernInputBox(float x, float y, float width, float height, 
                               const std::string& labelText, const std::string& placeholderText,
                               const sf::Font& font, bool password)
    : text(font, "", 16), label(font, labelText, 14), 
      placeholder(font, placeholderText, 16), fontPtr(&font) {
    
    box.setPosition({x, y});
    box.setSize({width, height});
    box.setFillColor(sf::Color(249, 250, 251));
    box.setOutlineThickness(1);
    box.setOutlineColor(sf::Color(209, 213, 219));
    
    text.setFillColor(sf::Color(17, 24, 39));
    text.setPosition({x + 15, y + height/2 - 10});
    
    placeholder.setFillColor(sf::Color(156, 163, 175));
    placeholder.setPosition({x + 15, y + height/2 - 10});
    
    label.setFillColor(sf::Color(75, 85, 99));
    label.setPosition({x, y - 25});
    
    isFocused = false;
    isPassword = password;
    showPassword = false;
    content = "";
    showCursor = true;
    
    if (password) {
        eyeIcon.setSize({20, 20});
        eyeIcon.setFillColor(sf::Color(156, 163, 175));
        eyeIcon.setPosition({x + width - 35, y + height/2 - 10});
    }
}

void ModernInputBox::handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
    if (const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>()) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        isFocused = box.getGlobalBounds().contains(sf::Vector2f(mousePos));
        
        if (isFocused) {
            box.setOutlineColor(sf::Color(74, 144, 226));
            box.setOutlineThickness(2);
        } else {
            box.setOutlineColor(sf::Color(209, 213, 219));
            box.setOutlineThickness(1);
        }
        
        // Check eye icon click
        if (isPassword && eyeIcon.getGlobalBounds().contains(sf::Vector2f(mousePos))) {
            showPassword = !showPassword;
            if (showPassword) {
                text.setString(content);
            } else {
                text.setString(std::string(content.length(), '*'));
            }
        }
    }
    
    if (isFocused) {
        if (const auto* textEntered = event.getIf<sf::Event::TextEntered>()) {
            if (textEntered->unicode == 8) { // Backspace
                if (!content.empty()) {
                    content.pop_back();
                }
            } else if (textEntered->unicode == 13) { // Enter
                isFocused = false;
                box.setOutlineColor(sf::Color(209, 213, 219));
                box.setOutlineThickness(1);
            } else if (textEntered->unicode < 128 && textEntered->unicode >= 32) {
                content += static_cast<char>(textEntered->unicode);
            }
            
            if (isPassword && !showPassword) {
                text.setString(std::string(content.length(), '*'));
            } else {
                text.setString(content);
            }
        }
    }
}

void ModernInputBox::update() {
    if (blinkClock.getElapsedTime().asSeconds() > 0.5f) {
        showCursor = !showCursor;
        blinkClock.restart();
    }
}

void ModernInputBox::draw(sf::RenderWindow& window) {
    window.draw(label);
    window.draw(box);
    
    if (content.empty() && !isFocused) {
        window.draw(placeholder);
    } else {
        window.draw(text);
    }
    
    if (isFocused && showCursor) {
        sf::RectangleShape cursor({2, 20});
        cursor.setFillColor(sf::Color(74, 144, 226));
        auto textBounds = text.getGlobalBounds();
        cursor.setPosition({textBounds.position.x + textBounds.size.x + 2, text.getPosition().y});
        window.draw(cursor);
    }
    
    if (isPassword) {
        // Draw eye icon (simplified)
        sf::CircleShape eye(8);
        eye.setFillColor(sf::Color::Transparent);
        eye.setOutlineThickness(2);
        eye.setOutlineColor(sf::Color(156, 163, 175));
        eye.setPosition(eyeIcon.getPosition());
        window.draw(eye);
    }
}

void ModernInputBox::clear() {
    content = "";
    text.setString("");
}

void ModernInputBox::setContent(const std::string& str) {
    content = str;
    if (isPassword && !showPassword) {
        text.setString(std::string(str.length(), '*'));
    } else {
        text.setString(str);
    }
}

// Card Implementation
Card::Card(float x, float y, float width, float height, 
           const std::string& titleText, const std::string& subtitleText,
           const sf::Font& font)
    : title(font, titleText, 28), subtitle(font, subtitleText, 16), fontPtr(&font) {
    
    // Shadow
    shadow.setPosition({x + 4, y + 4});
    shadow.setSize({width, height});
    shadow.setFillColor(sf::Color(0, 0, 0, 20));
    
    // Main shape
    shape.setPosition({x, y});
    shape.setSize({width, height});
    shape.setFillColor(sf::Color::White);
    
    // Title
    title.setFillColor(sf::Color(17, 24, 39));
    auto titleBounds = title.getLocalBounds();
    title.setPosition({x + width/2 - titleBounds.size.x/2, y + 40});
    
    // Subtitle
    subtitle.setFillColor(sf::Color(107, 114, 128));
    auto subtitleBounds = subtitle.getLocalBounds();
    subtitle.setPosition({x + width/2 - subtitleBounds.size.x/2, y + 80});
}

void Card::draw(sf::RenderWindow& window) {
    window.draw(shadow);
    window.draw(shape);
    window.draw(title);
    window.draw(subtitle);
}

// MessageBox Implementation
MessageBox::MessageBox(const sf::Font& font) 
    : title(font, "", 22), message(font, "", 16), fontPtr(&font) {
    
    background.setSize({1200, 800});
    background.setFillColor(sf::Color(0, 0, 0, 150));
    
    box.setSize({450, 220});
    box.setPosition({375, 290});
    box.setFillColor(sf::Color::White);
    
    title.setFillColor(sf::Color(17, 24, 39));
    title.setPosition({400, 310});
    
    message.setFillColor(sf::Color(75, 85, 99));
    message.setPosition({400, 360});
    
    okButton = new ModernButton(475, 430, 120, 45, "OK", font);
    okButton->setOnClick([this]() { this->hide(); });
    
    isVisible = false;
}

MessageBox::~MessageBox() {
    delete okButton;
}

void MessageBox::show(const std::string& titleText, const std::string& msg, bool success) {
    title.setString(titleText);
    message.setString(msg);
    isSuccess = success;
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

// AppointmentCard Implementation
AppointmentCard::AppointmentCard(float x, float y, float width,
                                 const std::string& doctor, const std::string& dt,
                                 const std::string& reasonText, const std::string& statusText,
                                 const sf::Font& font)
    : doctorName(font, doctor, 18), dateTime(font, dt, 14),
      reason(font, reasonText, 14), status(font, statusText, 12), fontPtr(&font) {
    
    shape.setPosition({x, y});
    shape.setSize({width, 120});
    shape.setFillColor(sf::Color(249, 250, 251));
    shape.setOutlineThickness(1);
    shape.setOutlineColor(sf::Color(229, 231, 235));
    
    doctorName.setFillColor(sf::Color(17, 24, 39));
    doctorName.setPosition({x + 20, y + 15});
    
    dateTime.setFillColor(sf::Color(107, 114, 128));
    dateTime.setPosition({x + 20, y + 45});
    
    reason.setFillColor(sf::Color(75, 85, 99));
    reason.setPosition({x + 20, y + 70});
    
    status.setFillColor(sf::Color(16, 185, 129));
    status.setPosition({x + width - 120, y + 15});
}

void AppointmentCard::draw(sf::RenderWindow& window) {
    window.draw(shape);
    window.draw(doctorName);
    window.draw(dateTime);
    window.draw(reason);
    window.draw(status);
}

// HospitalGUI Implementation
HospitalGUI::HospitalGUI() 
    : window(sf::VideoMode({1200, 800}), "Datchikham - He Thong Quan Ly Benh Vien"),
      currentScreen(Screen::MAIN_MENU),
      isLoggedIn(false),
      selectedDoctorIndex(-1) {
    
    if (!font.openFromFile("assets/arial.ttf")) {
        std::cerr << "Error: Cannot load font" << std::endl;
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
    loginCard.reset();
    
    auto loginBtn = std::make_unique<ModernButton>(450, 350, 300, 55, "DANG NHAP", font);
    loginBtn->setOnClick([this]() {
        currentScreen = Screen::LOGIN;
        setupLoginScreen();
    });
    buttons.push_back(std::move(loginBtn));
    
    auto registerBtn = std::make_unique<ModernButton>(450, 420, 300, 55, "DANG KY", font, sf::Color(52, 211, 153));
    registerBtn->setOnClick([this]() {
        currentScreen = Screen::REGISTER;
        setupRegisterScreen();
    });
    buttons.push_back(std::move(registerBtn));
    
    auto exitBtn = std::make_unique<ModernButton>(450, 490, 300, 55, "THOAT", font, sf::Color(239, 68, 68));
    exitBtn->setOnClick([this]() {
        window.close();
    });
    buttons.push_back(std::move(exitBtn));
}

void HospitalGUI::setupLoginScreen() {
    buttons.clear();
    inputBoxes.clear();
    
    loginCard = std::make_unique<Card>(375, 180, 450, 500, "Sign In", 
                                       "Enter your credentials to continue", font);
    
    auto cardPos = loginCard->getPosition();
    float centerX = cardPos.x + 50;
    
    auto usernameBox = std::make_unique<ModernInputBox>(centerX, 310, 350, 50, 
                                                         "Email or Username", "Enter your email",
                                                         font);
    inputBoxes.push_back(std::move(usernameBox));
    
    auto passwordBox = std::make_unique<ModernInputBox>(centerX, 400, 350, 50, 
                                                         "Password", "Enter your password",
                                                         font, true);
    inputBoxes.push_back(std::move(passwordBox));
    
    auto loginBtn = std::make_unique<ModernButton>(centerX, 490, 350, 50, "Sign In", font);
    loginBtn->setOnClick([this]() { performLogin(); });
    buttons.push_back(std::move(loginBtn));
    
    auto backBtn = std::make_unique<ModernButton>(centerX, 560, 170, 45, "Back", font, sf::Color(156, 163, 175));
    backBtn->setOnClick([this]() {
        currentScreen = Screen::MAIN_MENU;
        setupMainMenu();
    });
    buttons.push_back(std::move(backBtn));
}

void HospitalGUI::setupRegisterScreen() {
    buttons.clear();
    inputBoxes.clear();
    
    loginCard = std::make_unique<Card>(375, 140, 450, 580, "Create Account", 
                                       "Register to get started", font);
    
    auto cardPos = loginCard->getPosition();
    float centerX = cardPos.x + 50;
    
    auto usernameBox = std::make_unique<ModernInputBox>(centerX, 270, 350, 50, 
                                                         "Username", "Choose a username",
                                                         font);
    inputBoxes.push_back(std::move(usernameBox));
    
    auto emailBox = std::make_unique<ModernInputBox>(centerX, 350, 350, 50, 
                                                      "Email", "Enter your email",
                                                      font);
    inputBoxes.push_back(std::move(emailBox));
    
    auto passwordBox = std::make_unique<ModernInputBox>(centerX, 430, 350, 50, 
                                                         "Password", "Create a password",
                                                         font, true);
    inputBoxes.push_back(std::move(passwordBox));
    
    auto confirmBox = std::make_unique<ModernInputBox>(centerX, 510, 350, 50, 
                                                        "Confirm Password", "Confirm your password",
                                                        font, true);
    inputBoxes.push_back(std::move(confirmBox));
    
    auto registerBtn = std::make_unique<ModernButton>(centerX, 590, 350, 50, "Create Account", font, sf::Color(52, 211, 153));
    registerBtn->setOnClick([this]() { performRegister(); });
    buttons.push_back(std::move(registerBtn));
    
    auto backBtn = std::make_unique<ModernButton>(centerX + 90, 660, 170, 45, "Back", font, sf::Color(156, 163, 175));
    backBtn->setOnClick([this]() {
        currentScreen = Screen::MAIN_MENU;
        setupMainMenu();
    });
    buttons.push_back(std::move(backBtn));
}

void HospitalGUI::setupPatientMenu() {
    buttons.clear();
    inputBoxes.clear();
    loginCard.reset();
    
    auto bookBtn = std::make_unique<ModernButton>(350, 250, 500, 65, "DAT LICH KHAM", font);
    bookBtn->setOnClick([this]() {
        currentScreen = Screen::DOCTOR_LIST;
        setupDoctorList();
    });
    buttons.push_back(std::move(bookBtn));
    
    auto viewBtn = std::make_unique<ModernButton>(350, 330, 500, 65, "XEM LICH KHAM", font, sf::Color(52, 211, 153));
    viewBtn->setOnClick([this]() {
        currentScreen = Screen::VIEW_APPOINTMENTS;
        setupViewAppointments();
    });
    buttons.push_back(std::move(viewBtn));
    
    auto profileBtn = std::make_unique<ModernButton>(350, 410, 500, 65, "CAP NHAT THONG TIN", font, sf::Color(251, 146, 60));
    profileBtn->setOnClick([this]() {
        currentScreen = Screen::UPDATE_PROFILE;
        setupUpdateProfile();
    });
    buttons.push_back(std::move(profileBtn));
    
    auto logoutBtn = std::make_unique<ModernButton>(350, 490, 500, 65, "DANG XUAT", font, sf::Color(239, 68, 68));
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
    loginCard.reset();
    
    auto viewBtn = std::make_unique<ModernButton>(350, 300, 500, 65, "XEM LICH HEN", font);
    viewBtn->setOnClick([this]() {
        currentScreen = Screen::VIEW_APPOINTMENTS;
        setupViewAppointments();
    });
    buttons.push_back(std::move(viewBtn));
    
    auto profileBtn = std::make_unique<ModernButton>(350, 380, 500, 65, "CAP NHAT THONG TIN", font, sf::Color(251, 146, 60));
    profileBtn->setOnClick([this]() {
        currentScreen = Screen::UPDATE_PROFILE;
        setupUpdateProfile();
    });
    buttons.push_back(std::move(profileBtn));
    
    auto logoutBtn = std::make_unique<ModernButton>(350, 460, 500, 65, "DANG XUAT", font, sf::Color(239, 68, 68));
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
    loginCard.reset();
    
    loginCard = std::make_unique<Card>(300, 150, 600, 520, "Dat Lich Kham", 
                                       "Nhap thong tin lich kham", font);
    
    float centerX = 350;
    
    auto dateBox = std::make_unique<ModernInputBox>(centerX, 270, 500, 50, 
                                                     "Ngay kham", "YYYY-MM-DD (VD: 2025-01-15)",
                                                     font);
    inputBoxes.push_back(std::move(dateBox));
    
    auto timeBox = std::make_unique<ModernInputBox>(centerX, 350, 500, 50, 
                                                     "Gio kham", "HH:MM (VD: 14:30)",
                                                     font);
    inputBoxes.push_back(std::move(timeBox));
    
    auto reasonBox = std::make_unique<ModernInputBox>(centerX, 430, 500, 50, 
                                                       "Ly do kham", "Nhap ly do kham benh",
                                                       font);
    inputBoxes.push_back(std::move(reasonBox));
    
    auto bookBtn = std::make_unique<ModernButton>(centerX, 510, 240, 50, "Dat Lich", font);
    bookBtn->setOnClick([this]() { bookAppointment(); });
    buttons.push_back(std::move(bookBtn));
    
    auto backBtn = std::make_unique<ModernButton>(centerX + 260, 510, 240, 50, "Quay Lai", font, sf::Color(156, 163, 175));
    backBtn->setOnClick([this]() {
        currentScreen = Screen::DOCTOR_LIST;
        setupDoctorList();
    });
    buttons.push_back(std::move(backBtn));
}

void HospitalGUI::setupDoctorList() {
    buttons.clear();
    inputBoxes.clear();
    loginCard.reset();
    
    doctorIds = DoctorService::getDoctorIDs();
    
    if (doctorIds.empty()) {
        messageBox->show("Thong Bao", "Hien khong co bac si nao trong he thong!", false);
        currentScreen = Screen::PATIENT_MENU;
        setupPatientMenu();
        return;
    }
    
    loginCard = std::make_unique<Card>(200, 140, 800, 580, "Chon Bac Si", 
                                       "Click vao bac si de dat lich kham", font);
    
    // Display doctors as clickable cards
    float yPos = 250;
    for (size_t i = 0; i < doctorIds.size() && i < 5; i++) {
        auto info = DataStore::readDoctorInfo(doctorIds[i]);
        std::string btnLabel = "Dr. " + info.name;
        if (!info.specialization.empty()) {
            btnLabel += " - " + info.specialization;
        }
        
        auto doctorBtn = std::make_unique<ModernButton>(250, yPos, 700, 60, btnLabel, font, sf::Color(52, 211, 153));
        doctorBtn->setOnClick([this, i]() {
            selectedDoctorIndex = i;
            currentScreen = Screen::BOOK_APPOINTMENT;
            setupBookAppointment();
        });
        buttons.push_back(std::move(doctorBtn));
        yPos += 70;
    }
    
    auto backBtn = std::make_unique<ModernButton>(400, 650, 400, 50, "Quay Lai", font, sf::Color(156, 163, 175));
    backBtn->setOnClick([this]() {
        currentScreen = Screen::PATIENT_MENU;
        setupPatientMenu();
    });
    buttons.push_back(std::move(backBtn));
}

void HospitalGUI::setupViewAppointments() {
    buttons.clear();
    inputBoxes.clear();
    loginCard.reset();
    
    std::vector<DataStore::AppointmentDetails> appointments;
    
    if (currentRole == "Patient") {
        appointments = PatientService::getAppointments(currentId);
    } else if (currentRole == "Doctor") {
        appointments = DoctorService::getAppointments(currentId);
    }
    
    if (appointments.empty()) {
        loginCard = std::make_unique<Card>(350, 250, 500, 200, "Khong Co Lich Kham", 
                                           "Ban chua co lich kham nao", font);
    } else {
        loginCard = std::make_unique<Card>(150, 140, 900, 580, "Danh Sach Lich Kham", 
                                           "Cac lich kham cua ban", font);
        
        // Display appointments (max 4)
        float yPos = 260;
        for (size_t i = 0; i < appointments.size() && i < 4; i++) {
            const auto& appt = appointments[i];
            
            sf::RectangleShape card({850, 100});
            card.setPosition({175, yPos});
            card.setFillColor(sf::Color(249, 250, 251));
            card.setOutlineThickness(1);
            card.setOutlineColor(sf::Color(229, 231, 235));
            
            std::string info = currentRole == "Patient" ? 
                "Bac si: " + appt.doctorId : "Benh nhan: " + appt.patientId;
            
            yPos += 110;
        }
    }
    
    auto backBtn = std::make_unique<ModernButton>(400, 680, 400, 50, "Quay Lai", font, sf::Color(156, 163, 175));
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
    loginCard.reset();
    
    loginCard = std::make_unique<Card>(300, 150, 600, 520, "Cap Nhat Thong Tin", 
                                       "Chinh sua thong tin ca nhan", font);
    
    float centerX = 350;
    
    if (currentRole == "Patient") {
        auto info = DataStore::readPatientInfo(currentId);
        
        auto nameBox = std::make_unique<ModernInputBox>(centerX, 270, 500, 50, 
                                                         "Ho va ten", "Nhap ho va ten",
                                                         font);
        nameBox->setContent(info.name);
        inputBoxes.push_back(std::move(nameBox));
        
        auto phoneBox = std::make_unique<ModernInputBox>(centerX, 350, 500, 50, 
                                                          "So dien thoai", "Nhap so dien thoai",
                                                          font);
        phoneBox->setContent(info.phone);
        inputBoxes.push_back(std::move(phoneBox));
        
        auto emailBox = std::make_unique<ModernInputBox>(centerX, 430, 500, 50, 
                                                          "Email", "Nhap email",
                                                          font);
        emailBox->setContent(info.email);
        inputBoxes.push_back(std::move(emailBox));
        
    } else if (currentRole == "Doctor") {
        auto info = DataStore::readDoctorInfo(currentId);
        
        auto nameBox = std::make_unique<ModernInputBox>(centerX, 270, 500, 50, 
                                                         "Ho va ten", "Nhap ho va ten",
                                                         font);
        nameBox->setContent(info.name);
        inputBoxes.push_back(std::move(nameBox));
        
        auto phoneBox = std::make_unique<ModernInputBox>(centerX, 350, 500, 50, 
                                                          "So dien thoai", "Nhap so dien thoai",
                                                          font);
        phoneBox->setContent(info.phone);
        inputBoxes.push_back(std::move(phoneBox));
        
        auto specBox = std::make_unique<ModernInputBox>(centerX, 430, 500, 50, 
                                                         "Chuyen khoa", "Nhap chuyen khoa",
                                                         font);
        specBox->setContent(info.specialization);
        inputBoxes.push_back(std::move(specBox));
    }
    
    auto saveBtn = std::make_unique<ModernButton>(centerX, 510, 240, 50, "Luu", font, sf::Color(52, 211, 153));
    saveBtn->setOnClick([this]() {
        if (inputBoxes.size() < 3) return;
        
        if (currentRole == "Patient") {
            DataStore::PatientInfo info;
            info.name = inputBoxes[0]->getContent();
            info.phone = inputBoxes[1]->getContent();
            info.email = inputBoxes[2]->getContent();
            
            if (DataStore::writePatientInfo(currentId, info)) {
                messageBox->show("Thanh Cong", "Da cap nhat thong tin!", true);
            } else {
                messageBox->show("Loi", "Khong the cap nhat!", false);
            }
        } else if (currentRole == "Doctor") {
            DataStore::DoctorInfo info;
            info.name = inputBoxes[0]->getContent();
            info.phone = inputBoxes[1]->getContent();
            info.specialization = inputBoxes[2]->getContent();
            
            if (DataStore::writeDoctorInfo(currentId, info)) {
                messageBox->show("Thanh Cong", "Da cap nhat thong tin!", true);
            } else {
                messageBox->show("Loi", "Khong the cap nhat!", false);
            }
        }
    });
    buttons.push_back(std::move(saveBtn));
    
    auto backBtn = std::make_unique<ModernButton>(centerX + 260, 510, 240, 50, "Quay Lai", font, sf::Color(156, 163, 175));
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
    
    // Use new GUI-compatible login method
    if (account.loginWithCredentials(username, password, currentRole, currentId)) {
        isLoggedIn = true;
        messageBox->show("Thanh Cong", "Dang nhap thanh cong!", true);
        
        if (currentRole == "Patient") {
            currentScreen = Screen::PATIENT_MENU;
            setupPatientMenu();
        } else if (currentRole == "Doctor") {
            currentScreen = Screen::DOCTOR_MENU;
            setupDoctorMenu();
        } else if (currentRole == "Admin") {
            currentScreen = Screen::PATIENT_MENU;
            setupPatientMenu();
        }
    } else {
        messageBox->show("Loi", "Ten dang nhap hoac mat khau sai!", false);
    }
}

void HospitalGUI::performRegister() {
    if (inputBoxes.size() < 4) return;
    
    std::string username = inputBoxes[0]->getContent();
    std::string email = inputBoxes[1]->getContent();
    std::string password = inputBoxes[2]->getContent();
    std::string confirm = inputBoxes[3]->getContent();
    
    if (username.empty() || email.empty() || password.empty()) {
        messageBox->show("Loi", "Vui long nhap day du thong tin!", false);
        return;
    }
    
    if (password != confirm) {
        messageBox->show("Loi", "Mat khau xac nhan khong khop!", false);
        return;
    }
    
    if (password.length() < 6) {
        messageBox->show("Loi", "Mat khau phai co it nhat 6 ky tu!", false);
        return;
    }
    
    // Call actual register function
    account.registerAccount();
    messageBox->show("Thanh Cong", "Dang ky tai khoan thanh cong!", true);
    currentScreen = Screen::MAIN_MENU;
    setupMainMenu();
}

void HospitalGUI::bookAppointment() {
    if (inputBoxes.size() < 3) return;
    if (selectedDoctorIndex < 0 || selectedDoctorIndex >= static_cast<int>(doctorIds.size())) {
        messageBox->show("Loi", "Vui long chon bac si!", false);
        return;
    }
    
    std::string date = inputBoxes[0]->getContent();
    std::string time = inputBoxes[1]->getContent();
    std::string reason = inputBoxes[2]->getContent();
    
    if (date.empty() || time.empty() || reason.empty()) {
        messageBox->show("Loi", "Vui long nhap day du thong tin!", false);
        return;
    }
    
    // Basic date validation (YYYY-MM-DD)
    if (date.length() != 10 || date[4] != '-' || date[7] != '-') {
        messageBox->show("Loi", "Ngay khong dung dinh dang! (YYYY-MM-DD)", false);
        return;
    }
    
    // Basic time validation (HH:MM)
    if (time.length() != 5 || time[2] != ':') {
        messageBox->show("Loi", "Gio khong dung dinh dang! (HH:MM)", false);
        return;
    }
    
    std::string apptId;
    std::string selectedDoctorId = doctorIds[selectedDoctorIndex];
    
    if (PatientService::bookAppointment(currentId, selectedDoctorId, date, time, reason, apptId)) {
        messageBox->show("Thanh Cong", "Dat lich thanh cong!\nMa: " + apptId, true);
        currentScreen = Screen::PATIENT_MENU;
        setupPatientMenu();
    } else {
        messageBox->show("Loi", "Khong the dat lich kham!", false);
    }
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
    drawModernBackground();
    
    // Draw logo/title
    sf::Text logo(font, "DAT LICH KHAM", 42);
    logo.setFillColor(sf::Color(74, 144, 226));
    logo.setPosition({420, 50});
    window.draw(logo);
    
    sf::Text tagline(font, "Hospital Management System", 16);
    tagline.setFillColor(sf::Color(107, 114, 128));
    tagline.setPosition({470, 100});
    window.draw(tagline);
    
    // Draw card if exists
    if (loginCard) {
        loginCard->draw(window);
    }
    
    // Draw input boxes
    for (auto& inputBox : inputBoxes) {
        inputBox->draw(window);
    }
    
    // Draw buttons
    for (auto& button : buttons) {
        button->draw(window);
    }
    
    // Draw message box (always on top)
    messageBox->draw(window);
    
    window.display();
}

void HospitalGUI::drawModernBackground() {
    // Gradient background
    sf::RectangleShape bg1({1200, 400});
    bg1.setPosition({0, 0});
    bg1.setFillColor(sf::Color(240, 242, 245));
    window.draw(bg1);
    
    sf::RectangleShape bg2({1200, 400});
    bg2.setPosition({0, 400});
    bg2.setFillColor(sf::Color(249, 250, 251));
    window.draw(bg2);
    
    // Decorative circles
    sf::CircleShape circle1(150);
    circle1.setPosition({-50, -50});
    circle1.setFillColor(sf::Color(74, 144, 226, 30));
    window.draw(circle1);
    
    sf::CircleShape circle2(200);
    circle2.setPosition({1000, 600});
    circle2.setFillColor(sf::Color(52, 211, 153, 30));
    window.draw(circle2);
}

void HospitalGUI::run() {
    while (window.isOpen()) {
        handleEvents();
        update();
        render();
    }
}

