#include "GUI.h"
#include "DataStore.h"
#include "gui/GuiInternal.h"
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <iostream>

void GUI::handleMouseClick(const sf::Vector2f& mousePos) {
    switch (currentScreen) {
        case Screen::REGISTER_SELECT_ROLE: {
            auto ws = window.getSize();
            float W = static_cast<float>(ws.x);
            float panelW = 760.f;
            float panelX = W / 2.f - panelW / 2.f;
            float cardWidth = 280.f, cardHeight = 130.f, cardSpacing = 20.f;
            float cardsStartX = panelX + (panelW - (2 * cardWidth + cardSpacing)) / 2.f;
            float cardY = 360.f;
            if (isMouseOverRect(mousePos, {cardsStartX, cardY}, {cardWidth, cardHeight})) {
                selectedRole = UserRole::PATIENT;
            }
            else if (isMouseOverRect(mousePos, {cardsStartX + cardWidth + cardSpacing, cardY}, {cardWidth, cardHeight})) {
                selectedRole = UserRole::DOCTOR;
            }
            else if (isMouseOverRect(mousePos, {W / 2.f - 150.f, 520}, {300, 55})) {
                if (selectedRole != UserRole::NONE) {
                    switchToScreen(Screen::REGISTER_INFO);
                }
            }
            else if (isMouseOverRect(mousePos, {W / 2.f - 240.f, 590}, {480, 40})) {
                switchToScreen(Screen::LOGIN);
            }
            break;
        }
        case Screen::REGISTER_INFO: {
            auto ws = window.getSize();
            float W = static_cast<float>(ws.x);
            float panelW = 760.f;
            float panelX = W / 2.f - panelW / 2.f;
            float baseX = panelX + 80.f;
            for (int i = 0; i < 4; i++) {
                float yPos = 265 + i * 80;
                if (isMouseOverRect(mousePos, {baseX, yPos}, {600, 50})) {
                    activeInputField = i;
                    return;
                }
            }
            if (isMouseOverRect(mousePos, {W / 2.f - 150.f, 600}, {300, 55})) {
                completeRegistration();
            }
            else if (isMouseOverRect(mousePos, {W / 2.f - 140.f, 700}, {280, 35})) {
                switchToScreen(Screen::REGISTER_SELECT_ROLE);
            }
            activeInputField = -1;
            break;
        }
        case Screen::LOGIN: {
            auto ws = window.getSize();
            float W = static_cast<float>(ws.x);
            float panelW = 700.f;
            float panelX = W / 2.f - panelW / 2.f;
            float baseX = panelX + 110.f;
            if (isMouseOverRect(mousePos, {baseX, 300}, {480, 50})) {
                activeInputField = 0;
            }
            else if (isMouseOverRect(mousePos, {baseX, 390}, {480, 50})) {
                activeInputField = 1;
            }
            else if (isMouseOverRect(mousePos, {W / 2.f - 100.f, 480}, {200, 55})) {
                completeLogin();
            }
            else if (isMouseOverRect(mousePos, {W / 2.f - 200.f, 560}, {400, 40})) {
                switchToScreen(Screen::REGISTER_SELECT_ROLE);
            }
            else {
                activeInputField = -1;
            }
            break;
        }
        case Screen::UPDATE_PATIENT_INFO: {
            auto ws = window.getSize();
            float W = static_cast<float>(ws.x);
            float panelX = W / 2.f - 800.f / 2.f;
            float baseX = panelX + 80.f;
            {
                bool canSave = !inputFullName.empty() && !inputPhone.empty() && !inputEmail.empty();
                if (canSave && isMouseOverRect(mousePos, {W / 2.f - 150.f, 800}, {300, 50})) {
                    savePatientInfo();
                    switchToScreen(Screen::PATIENT_DASHBOARD);
                    break;
                }
            }
            for (int i = 0; i < 8; i++) {
                float yPos = 200 + i * 75;
                if (isMouseOverRect(mousePos, {baseX, yPos}, {640, 50})) {
                    activeInputField = i;
                    return;
                }
            }
            activeInputField = -1;
            break;
        }
        case Screen::UPDATE_DOCTOR_INFO: {
            auto ws = window.getSize();
            float W = static_cast<float>(ws.x);
            float panelX = W / 2.f - 800.f / 2.f;
            float baseX = panelX + 80.f;
            {
                bool canSave = !inputFullName.empty() && !inputPhone.empty() && !inputEmail.empty()
                               && !inputSpecialization.empty() && !inputFaculty.empty();
                if (canSave && isMouseOverRect(mousePos, {W / 2.f - 150.f, 820}, {300, 50})) {
                    saveDoctorInfo();
                    switchToScreen(Screen::DOCTOR_DASHBOARD);
                    break;
                }
            }
            for (int i = 0; i < 9; i++) {
                float yPos = 185 + i * 70;
                if (isMouseOverRect(mousePos, {baseX, yPos}, {640, 50})) {
                    activeInputField = i;
                    return;
                }
            }
            activeInputField = -1;
            break;
        }
        case Screen::PATIENT_DASHBOARD: {
            auto ws = window.getSize();
            float W = static_cast<float>(ws.x);
            float panelX = W / 2.f - 1180.f / 2.f;
            float menuStartY = 140;
            float menuItemHeight = 70;
            for (int i = 0; i < 4; i++) {
                float itemY = menuStartY + i * menuItemHeight;
                if (isMouseOverRect(mousePos, {panelX, itemY}, {250, 60})) {
                    activePatientMenu = static_cast<PatientMenuOption>(i);
                    break;
                }
            }
            if (isMouseOverRect(mousePos, {W - 170.f, 35.f}, {150.f, 30.f})) {
                resetInputFields();
                switchToScreen(Screen::LOGIN);
            }
            if (activePatientMenu == PatientMenuOption::BOOK_APPOINTMENT && showBookingModal) {
                auto ws2 = window.getSize();
                float W2 = static_cast<float>(ws2.x);
                float H2 = static_cast<float>(ws2.y);
                float mw = 600.f, mh = 480.f;
                float mx = W2/2.f - mw/2.f;
                float my = H2/2.f - mh/2.f;
                float fieldWidth = 540.f;
                float fx = mx + (mw - fieldWidth) / 2.f;
                float startY = my + 80;
                float spacing = 85.f;
                if (isMouseOverRect(mousePos, {fx, startY}, {fieldWidth, 48})) { activeInputField = 100; return; }
                if (isMouseOverRect(mousePos, {fx, startY + spacing}, {fieldWidth, 48})) { activeInputField = 101; return; }
                if (isMouseOverRect(mousePos, {fx, startY + spacing * 2}, {fieldWidth, 90})) { activeInputField = 102; return; }
                bool canBook = false;
                {
                    if (!selectedDoctorForBooking.empty()) {
                        int dd,mm,yy,hh,mi; std::time_t tmp;
                        canBook = parseDateDDMMYYYY(bookingDateText, dd, mm, yy) &&
                                  parseTimeHHMM(bookingTimeText, hh, mi) &&
                                  toTimeT(bookingDateText, bookingTimeText, tmp) && tmp >= std::time(nullptr);
                    }
                }
                float buttonY = my + mh - 60;
                float buttonSpacing = 20.f;
                float totalButtonWidth = 130.f * 2 + buttonSpacing;
                float buttonStartX = mx + (mw - totalButtonWidth) / 2.f;
                if (isMouseOverRect(mousePos, {buttonStartX, buttonY}, {130, 44})) {
                    showBookingModal = false;
                    activeInputField = -1;
                    return;
                }
                if (isMouseOverRect(mousePos, {buttonStartX + 130 + buttonSpacing, buttonY}, {130, 44})) {
                    if (!canBook) {
                        std::cout << u8"❌ Ngày/giờ không hợp lệ hoặc đã qua. Vui lòng kiểm tra lại (DD/MM/YYYY, HH:MM)." << std::endl;
                        return;
                    }
                    std::string aptId;
                    bool ok = DataStore::appendAppointment(currentUserId, selectedDoctorForBooking,
                                                           bookingDateText, bookingTimeText, bookingReasonText, aptId);
                    if (ok) {
                        std::cout << u8"✅ Đặt lịch thành công! Mã lịch: " << aptId << std::endl;
                    } else {
                        std::cout << u8"❌ Không thể lưu lịch khám." << std::endl;
                    }
                    showBookingModal = false;
                    activeInputField = -1;
                    return;
                }
                if (!isMouseOverRect(mousePos, {mx, my}, {mw, mh})) {
                    showBookingModal = false;
                    activeInputField = -1;
                    return;
                }
                return;
            }
            if (activePatientMenu == PatientMenuOption::BOOK_APPOINTMENT && showDatePicker) {
                auto ws2 = window.getSize();
                float W2 = static_cast<float>(ws2.x);
                float H2 = static_cast<float>(ws2.y);
                float mw = 420.f, mh = 360.f;
                float mx = W2/2.f - mw/2.f;
                float my = H2/2.f - mh/2.f;
                auto now = std::time(nullptr);
                std::tm base = *std::localtime(&now);
                float ox = mx + 20, oy = my + 60;
                for (int i = 0; i < 14; ++i) {
                    float col = (i % 2 == 0) ? 0.f : 200.f;
                    float row = static_cast<float>(i / 2) * 42.f;
                    if (isMouseOverRect(mousePos, {ox + col, oy + row}, {180, 36})) {
                        std::tm t = base; t.tm_mday += i; std::mktime(&t);
                        std::ostringstream oss; oss << std::setfill('0')
                            << std::setw(2) << t.tm_mday << "/" << std::setw(2) << (t.tm_mon+1) << "/" << (t.tm_year+1900);
                        bookingDateText = oss.str();
                        showDatePicker = false;
                        activeInputField = 101;
                        return;
                    }
                }
                if (!isMouseOverRect(mousePos, {mx, my}, {mw, mh})) {
                    showDatePicker = false;
                    return;
                }
                return;
            }
            if (activePatientMenu == PatientMenuOption::UPDATE_INFO) {
                if (isMouseOverRect(mousePos, {panelX + 270.f, 200.f}, {350, 50})) {
                    loadPatientInfo();
                    switchToScreen(Screen::UPDATE_PATIENT_INFO);
                }
            }
            if (activePatientMenu == PatientMenuOption::BOOK_APPOINTMENT) {
                sf::Vector2f contentPos = {panelX + 270.f, 120};
                float searchY = contentPos.y + 70;
                if (isMouseOverRect(mousePos, {contentPos.x + 195, searchY + 100}, {185, 48})) {
                    showDatePicker = true;
                    return;
                }
                auto allDoctorIds = DataStore::listIDs("Doctor");
                auto toLower = [](std::string s){ for(char& c: s) c = static_cast<char>(std::tolower((unsigned char)c)); return s; };
                std::string q = toLower(searchDoctorText);
                std::vector<std::string> filtered;
                for (const auto& did : allDoctorIds) {
                    if (q.empty()) { filtered.push_back(did); continue; }
                    auto info = DataStore::readDoctorInfo(did);
                    std::string hay = toLower(info.name + " " + info.specialization + " " + did);
                    if (hay.find(q) != std::string::npos) filtered.push_back(did);
                }
                int showCount = static_cast<int>(std::min<size_t>(6, filtered.size()));
                float listStartY = searchY + 170;
                for (int i = 0; i < showCount; ++i) {
                    float y = listStartY + i * 90.f;
                    // Phù hợp với bề rộng mới 520 của item
                    if (isMouseOverRect(mousePos, {contentPos.x + 520 - 120, y + 80 - 40}, {110, 32})) {
                        selectedDoctorForBooking = filtered[i];
                        bookingDateText.clear();
                        bookingTimeText.clear();
                        bookingReasonText.clear();
                        showBookingModal = true;
                        activeInputField = 100;
                        return;
                    }
                }
                // Đồng bộ với render: cột phải bắt đầu tại contentPos.x + 540
                float appointmentX = contentPos.x + 540;
                struct AptView { std::time_t when; DataStore::AppointmentDetails det; };
                std::vector<AptView> upcoming;
                auto ids = DataStore::listAppointmentsForPatient(currentUserId);
                auto nowT = std::time(nullptr);
                for (const auto& aid : ids) {
                    auto det = DataStore::readAppointment(aid);
                    if (!det.status.empty() && (det.status == "Cancelled" || det.status == "cancelled")) continue;
                    std::time_t t;
                    if (toTimeT(det.date, det.time, t) && t >= nowT) {
                        upcoming.push_back({t, det});
                    }
                }
                std::sort(upcoming.begin(), upcoming.end(), [](const AptView& a, const AptView& b){ return a.when < b.when; });
                int toShow = static_cast<int>(std::min<size_t>(2, upcoming.size()));
                for (int i = 0; i < toShow; ++i) {
                    float y = (i == 0) ? (searchY + 115) : (searchY + 235);
                    // aptWidth mới là 370 => nút hủy nằm ở x + 370 - 120
                    if (isMouseOverRect(mousePos, {appointmentX + 370 - 120, y + 105 - 42}, {105, 32})) {
                        DataStore::updateAppointmentStatus(upcoming[i].det.appointmentId, "Cancelled");
                        std::cout << u8"Đã hủy lịch: " << upcoming[i].det.appointmentId << std::endl;
                        return;
                    }
                }
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

void GUI::handleTextInput(char32_t unicode) {
    if (currentScreen == Screen::PATIENT_DASHBOARD && showBookingModal) {
        if (unicode == 8) {
            std::string* target = nullptr;
            if (activeInputField == 100) target = &bookingDateText;
            else if (activeInputField == 101) target = &bookingTimeText;
            else if (activeInputField == 102) target = &bookingReasonText;
            if (target && !target->empty()) {
                while (!target->empty()) {
                    target->pop_back();
                    if (target->empty() || (static_cast<unsigned char>(target->back()) & 0xC0) != 0x80) break;
                }
            }
            return;
        }
        if (unicode >= 32) {
            std::string* target = nullptr;
            if (activeInputField == 100) target = &bookingDateText;
            else if (activeInputField == 101) target = &bookingTimeText;
            else if (activeInputField == 102) target = &bookingReasonText;
            if (target) {
                if (activeInputField == 100) {
                    if (unicode >= '0' && unicode <= '9') {
                        if (target->size() < 10) {
                            target->push_back(static_cast<char>(unicode));
                            if (target->size() == 2 || target->size() == 5) target->push_back('/');
                        }
                    }
                } else if (activeInputField == 101) {
                    if (unicode >= '0' && unicode <= '9') {
                        if (target->size() < 5) {
                            target->push_back(static_cast<char>(unicode));
                            if (target->size() == 2) target->push_back(':');
                        }
                    }
                } else {
                    if (unicode < 0x10000) {
                        if (unicode < 0x80) {
                            target->push_back(static_cast<char>(unicode));
                        } else if (unicode < 0x800) {
                            target->push_back(static_cast<char>(0xC0 | (unicode >> 6)));
                            target->push_back(static_cast<char>(0x80 | (unicode & 0x3F)));
                        } else {
                            target->push_back(static_cast<char>(0xE0 | (unicode >> 12)));
                            target->push_back(static_cast<char>(0x80 | ((unicode >> 6) & 0x3F)));
                            target->push_back(static_cast<char>(0x80 | (unicode & 0x3F)));
                        }
                    }
                }
            }
        }
        return;
    }
    if (activeInputField < 0) return;
    if (unicode == 8) {
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
    if (unicode >= 32 && unicode < 127) {
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
