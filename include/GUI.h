// ============================================
// File: include/GUI.h - SFML 3.0 Compatible
// ============================================
#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <optional>
#include "Account.h"
#include "DataStore.h"
#include "PatientService.h"
#include "DoctorService.h"

class Button {
private:
    sf::RectangleShape shape;
    sf::Text text;
    sf::Color normalColor;
    sf::Color hoverColor;
    bool isHovered;
    std::function<void()> onClick;

public:
    Button(float x, float y, float width, float height, 
           const std::string& label, const sf::Font& font);
    void setOnClick(std::function<void()> callback);
    void handleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void update();
    void draw(sf::RenderWindow& window);
};

class InputBox {
private:
    sf::RectangleShape box;
    sf::Text text;
    sf::Text label;
    std::string content;
    bool isFocused;
    bool isPassword;
    sf::Clock blinkClock;
    bool showCursor;
    const sf::Font* fontPtr;

public:
    InputBox(float x, float y, float width, float height, 
             const std::string& labelText, const sf::Font& font, bool password = false);
    void handleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void update();
    void draw(sf::RenderWindow& window);
    std::string getContent() const { return content; }
    void clear();
    void setContent(const std::string& str);
};

class MessageBox {
private:
    sf::RectangleShape background;
    sf::RectangleShape box;
    sf::Text message;
    sf::Text title;
    Button* okButton;
    bool isVisible;
    const sf::Font* fontPtr;

public:
    MessageBox(const sf::Font& font);
    ~MessageBox();
    void show(const std::string& titleText, const std::string& msg, bool success = true);
    void hide();
    void handleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void update();
    void draw(sf::RenderWindow& window);
    bool visible() const { return isVisible; }
};

enum class Screen {
    MAIN_MENU,
    LOGIN,
    REGISTER,
    PATIENT_MENU,
    DOCTOR_MENU,
    BOOK_APPOINTMENT,
    VIEW_APPOINTMENTS,
    UPDATE_PROFILE
};

class HospitalGUI {
private:
    sf::RenderWindow window;
    sf::Font font;
    Screen currentScreen;
    
    std::vector<std::unique_ptr<Button>> buttons;
    std::vector<std::unique_ptr<InputBox>> inputBoxes;
    std::unique_ptr<MessageBox> messageBox;
    
    Account account;
    std::string currentRole;
    std::string currentId;
    bool isLoggedIn;

public:
    HospitalGUI();
    void run();

private:
    void setupMainMenu();
    void setupLoginScreen();
    void setupRegisterScreen();
    void setupPatientMenu();
    void setupDoctorMenu();
    void setupBookAppointment();
    void setupViewAppointments();
    void setupUpdateProfile();
    void performLogin();
    void performRegister();
    void handleEvents();
    void update();
    void render();
    void drawHeader();
    void drawScreenTitle(const std::string& title);
};

