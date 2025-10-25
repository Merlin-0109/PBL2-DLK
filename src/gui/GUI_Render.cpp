#include "GUI.h"
#include <SFML/Graphics.hpp>

void GUI::render() {
    window.clear(sf::Color(240, 248, 255));
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
