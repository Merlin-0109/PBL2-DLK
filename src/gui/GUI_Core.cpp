#include "GUI.h"
#include "DataStore.h"
#include "gui/GuiInternal.h"
#include <iostream>
#include <vector>

GUI::GUI() 
    : window(sf::VideoMode({1366, 900}), 
             makeUtf8String(u8"MediCare - Hệ Thống Đặt Lịch Khám Bệnh")),
      currentScreen(Screen::REGISTER_SELECT_ROLE),
      selectedRole(UserRole::NONE),
      activeInputField(-1),
      passwordVisible(false),
      showCursor(true),
      activePatientMenu(PatientMenuOption::BOOK_APPOINTMENT) {
    window.setFramerateLimit(60);
}

GUI::~GUI() {}

bool GUI::initialize() {
    // Use Roboto font for best Vietnamese support
    std::vector<std::string> fontPaths = {
        "assets/Roboto.ttf",
        "assets/fonts/arial.ttf",
        "C:/Windows/Fonts/segoeui.ttf",
        "C:/Windows/Fonts/arial.ttf"
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

    // Load Segoe UI Symbol for icons/symbols
    std::vector<std::string> symbolFontPaths = {
        "C:/Windows/Fonts/seguisym.ttf",
        "C:/Windows/Fonts/segoeui.ttf",
        "assets/Roboto.ttf"
    };
    bool symbolFontLoaded = false;
    for (const auto& path : symbolFontPaths) {
        if (symbolFont.openFromFile(path)) {
            std::cout << "Symbol font loaded successfully: " << path << std::endl;
            symbolFontLoaded = true;
            break;
        }
    }
    if (!symbolFontLoaded) {
        std::cout << "Warning: Could not load symbol font, using main font for symbols" << std::endl;
        symbolFont = font; // Use main font as fallback
    }

    Account::ensureDefaultAdminExists();
    DataStore::ensureAppointmentsDirExists();
    return true;
}

void GUI::run() {
    std::cout << "Running main loop..." << std::endl;
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(false);
    while (window.isOpen()) {
        handleEvents();
        render();
    }
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
