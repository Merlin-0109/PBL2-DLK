#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <memory>
#include <functional>
#include "Account.h"
#include "gui/Screen.h"
#include "gui/UserRole.h"

// Screen and user role enums are now declared in gui/Screen.h and gui/UserRole.h

class GUI {
private:
    sf::RenderWindow window;
    sf::Font font;
    sf::Font symbolFont; // Segoe UI Symbol for icons
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
    
    // Patient Dashboard state
    enum class PatientMenuOption {
        BOOK_APPOINTMENT,
        MY_APPOINTMENTS,
        NOTIFICATIONS,
        UPDATE_INFO
    };
    PatientMenuOption activePatientMenu;
    
    // Search and filter inputs
    std::string searchDoctorText;
    std::string selectedSpecialty;
    std::string selectedDate;

    // Booking modal state (Patient Dashboard)
    bool showBookingModal = false;
    std::string selectedDoctorForBooking;
    std::string bookingDateText;   // e.g. DD/MM/YYYY
    std::string bookingTimeText;   // e.g. HH:MM
    std::string bookingReasonText; // free text

    // Doctor cancel modal state
    bool showDoctorCancelModal = false;
    std::string selectedAppointmentToCancel;
    std::string cancelReasonText;
    
    // Doctor appointment detail modal state
    bool showDoctorAppointmentDetailModal = false;
    std::string selectedDoctorAppointmentForDetail;
    
    // Patient cancel modal state
    bool showPatientCancelModal = false;
    std::string patientCancelReason;
    
    // Appointment detail view modal state
    bool showAppointmentDetailModal = false;
    std::string selectedAppointmentForDetail;

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
    void drawButton(sf::Vector2f position, sf::Vector2f size, const std::string& text,
                   sf::Color bgColor, sf::Color textColor, bool enabled, bool hovered,
                   sf::RenderWindow& window);
    bool isMouseOverRect(const sf::Vector2f& mousePos, const sf::Vector2f& rectPos, 
                        const sf::Vector2f& rectSize);
    void drawInputField(sf::Vector2f position, sf::Vector2f size, const std::string& label,
                       const std::string& value, bool isActive, bool isPassword,
                       sf::RenderWindow& window);
    void drawInputFieldWithPlaceholder(sf::Vector2f position, sf::Vector2f size, const std::string& placeholder,
                        const std::string& value, bool isActive, bool isPassword,
                        sf::RenderWindow& window);
    void drawRoleCard(sf::Vector2f position, sf::Vector2f size, const std::string& title,
                     const std::string& description, const std::string& icon,
                     bool isSelected, sf::RenderWindow& window);
    void drawNavigationMenu(sf::Vector2f position, sf::Vector2f size, sf::RenderWindow& window);
    void drawAppointmentCard(sf::Vector2f position, sf::Vector2f size, 
                            const std::string& doctorName, const std::string& specialty,
                            const std::string& dateTime, bool isCancelled, sf::RenderWindow& window);
    void drawDoctorListItem(sf::Vector2f position, sf::Vector2f size,
                            const std::string& doctorName, const std::string& specialty,
                            const std::string& doctorId, sf::RenderWindow& window);
    
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
