// ============================================
// File: include/GUI.h - Modern Hospital Management GUI
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

class ModernButton {
private:
    sf::RectangleShape shape;
    sf::Text text;
    sf::Color normalColor;
    sf::Color hoverColor;
    sf::Color pressColor;
    bool isHovered;
    bool isPressed;
    std::function<void()> onClick;
    float cornerRadius;

public:
    ModernButton(float x, float y, float width, float height, 
                 const std::string& label, const sf::Font& font,
                 sf::Color color = sf::Color(74, 144, 226));
    void setOnClick(std::function<void()> callback);
    void handleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void update();
    void draw(sf::RenderWindow& window);
};

class ModernInputBox {
private:
    sf::RectangleShape box;
    sf::Text text;
    sf::Text label;
    sf::Text placeholder;
    std::string content;
    bool isFocused;
    bool isPassword;
    bool showPassword;
    sf::Clock blinkClock;
    bool showCursor;
    const sf::Font* fontPtr;
    sf::RectangleShape eyeIcon;

public:
    ModernInputBox(float x, float y, float width, float height, 
                   const std::string& labelText, const std::string& placeholderText,
                   const sf::Font& font, bool password = false);
    void handleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void update();
    void draw(sf::RenderWindow& window);
    std::string getContent() const { return content; }
    void clear();
    void setContent(const std::string& str);
};

class Card {
private:
    sf::RectangleShape shape;
    sf::RectangleShape shadow;
    sf::Text title;
    sf::Text subtitle;
    const sf::Font* fontPtr;

public:
    Card(float x, float y, float width, float height, 
         const std::string& titleText, const std::string& subtitleText,
         const sf::Font& font);
    void draw(sf::RenderWindow& window);
    sf::Vector2f getPosition() const { return shape.getPosition(); }
    sf::Vector2f getSize() const { return shape.getSize(); }
};

class MessageBox {
private:
    sf::RectangleShape background;
    sf::RectangleShape box;
    sf::Text message;
    sf::Text title;
    ModernButton* okButton;
    bool isVisible;
    bool isSuccess;
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

class AppointmentCard {
private:
    sf::RectangleShape shape;
    sf::Text doctorName;
    sf::Text dateTime;
    sf::Text reason;
    sf::Text status;
    const sf::Font* fontPtr;

public:
    AppointmentCard(float x, float y, float width,
                    const std::string& doctor, const std::string& dt,
                    const std::string& reasonText, const std::string& statusText,
                    const sf::Font& font);
    void draw(sf::RenderWindow& window);
};

enum class Screen {
    MAIN_MENU,
    LOGIN,
    REGISTER,
    PATIENT_MENU,
    DOCTOR_MENU,
    BOOK_APPOINTMENT,
    VIEW_APPOINTMENTS,
    UPDATE_PROFILE,
    DOCTOR_LIST
};

class HospitalGUI {
private:
    sf::RenderWindow window;
    sf::Font font;
    Screen currentScreen;
    
    std::vector<std::unique_ptr<ModernButton>> buttons;
    std::vector<std::unique_ptr<ModernInputBox>> inputBoxes;
    std::unique_ptr<MessageBox> messageBox;
    std::unique_ptr<Card> loginCard;
    
    Account account;
    std::string currentRole;
    std::string currentId;
    bool isLoggedIn;
    
    std::vector<std::string> doctorIds;
    int selectedDoctorIndex;
    std::string selectedRegisterRole; // NEW: Store selected role for registration

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
    void setupDoctorList();
    void setupViewAppointments();
    void setupUpdateProfile();
    
    void performLogin();
    void performRegister();
    void bookAppointment();
    
    void handleEvents();
    void update();
    void render();
    void drawModernBackground();
};

