#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <memory>
#include <functional>
#include "Account.h"

// Enum for different screens
enum class Screen {
    REGISTER_SELECT_ROLE,
    REGISTER_INFO,
    LOGIN,
    UPDATE_PATIENT_INFO,
    UPDATE_DOCTOR_INFO,
    PATIENT_DASHBOARD,
    DOCTOR_DASHBOARD
};

// Enum for user role selection
enum class UserRole {
    NONE,
    PATIENT,
    DOCTOR
};

class GUI {
private:
    sf::RenderWindow window;
    sf::Font font;
    Screen currentScreen;
    UserRole selectedRole;
    
    // Current user info after login/register
    std::string currentUserId;
    std::string currentUserRole;
    
    // Input fields data for registration (simplified)
    std::string inputUsername;
    std::string inputPassword;
    std::string inputConfirmPassword;
    std::string inputEmail;
    
    // Additional fields for profile update
    std::string inputFullName;
    std::string inputPhone;
    std::string inputAddress;
    std::string inputDOB;
    std::string inputGender;
    std::string inputBloodType;
    std::string inputCCCD;
    std::string inputSpecialization;  // For Doctor
    std::string inputFaculty;         // For Doctor
    
    // Active input field tracking
    int activeInputField;
    bool passwordVisible;
    
    // For text cursor blinking
    sf::Clock cursorClock;
    bool showCursor;

    // Account system
    Account accountSystem;

public:
    GUI();
    ~GUI();
    
    bool initialize();
    void run();
    
private:
    // Event handling
    void handleEvents();
    void handleMouseClick(const sf::Vector2f& mousePos);
    void handleTextInput(char32_t unicode);
    void handleKeyPress(sf::Keyboard::Key key);
    
    // Screen rendering
    void render();
    void renderRegisterSelectRole();
    void renderRegisterInfo();
    void renderLogin();
    void renderUpdatePatientInfo();
    void renderUpdateDoctorInfo();
    void renderPatientDashboard();
    void renderDoctorDashboard();
    
    // UI Helper functions
    void drawRoundedRect(sf::Vector2f position, sf::Vector2f size, float radius, 
                        sf::Color fillColor, sf::RenderWindow& window);
    void drawButton(sf::Vector2f position, sf::Vector2f size, const std::string& text,
                   sf::Color bgColor, sf::Color textColor, sf::RenderWindow& window);
    bool isMouseOverRect(const sf::Vector2f& mousePos, const sf::Vector2f& rectPos, 
                        const sf::Vector2f& rectSize);
    void drawInputField(sf::Vector2f position, sf::Vector2f size, const std::string& label,
                       const std::string& value, bool isActive, bool isPassword,
                       sf::RenderWindow& window);
    void drawRoleCard(sf::Vector2f position, sf::Vector2f size, const std::string& title,
                     const std::string& description, const std::string& icon,
                     bool isSelected, sf::RenderWindow& window);
    
    // Navigation
    void switchToScreen(Screen newScreen);
    void resetInputFields();
    
    // Registration logic
    void completeRegistration();
    bool validateRegistrationInputs();
    
    // Login logic
    void completeLogin();
    
    // Data persistence
    void loadPatientInfo();
    void savePatientInfo();
    void loadDoctorInfo();
    void saveDoctorInfo();
};
