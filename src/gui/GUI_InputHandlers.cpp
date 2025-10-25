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
            if (activePatientMenu == PatientMenuOption::UPDATE_INFO) {
                if (isMouseOverRect(mousePos, {panelX + 270.f, 200.f}, {350, 50})) {
                    loadPatientInfo();
                    switchToScreen(Screen::UPDATE_PATIENT_INFO);
                }
            }
            if (activePatientMenu == PatientMenuOption::MY_APPOINTMENTS) {
                // Handle patient cancel modal first
                if (showPatientCancelModal) {
                    auto ws2 = window.getSize();
                    float W2 = static_cast<float>(ws2.x);
                    float H2 = static_cast<float>(ws2.y);
                    float mw = 600.f, mh = 380.f;
                    float mx = W2/2.f - mw/2.f;
                    float my = H2/2.f - mh/2.f;
                    
                    float fieldWidth = 540.f;
                    float fx = mx + (mw - fieldWidth) / 2.f;
                    float startY2 = my + 100;
                    
                    // Click on text field
                    if (isMouseOverRect(mousePos, {fx, startY2}, {fieldWidth, 120})) {
                        activeInputField = 400;
                        return;
                    }
                    
                    bool canCancel = !patientCancelReason.empty();
                    float buttonY = my + mh - 60;
                    float buttonSpacing2 = 20.f;
                    float totalButtonWidth = 130.f * 2 + buttonSpacing2;
                    float buttonStartX2 = mx + (mw - totalButtonWidth) / 2.f;
                    
                    // Close button
                    if (isMouseOverRect(mousePos, {buttonStartX2, buttonY}, {130, 44})) {
                        showPatientCancelModal = false;
                        patientCancelReason.clear();
                        activeInputField = -1;
                        return;
                    }
                    
                    // Confirm button
                    if (isMouseOverRect(mousePos, {buttonStartX2 + 130 + buttonSpacing2, buttonY}, {130, 44})) {
                        if (canCancel && !selectedAppointmentToCancel.empty()) {
                            DataStore::updateAppointmentWithCancelReason(selectedAppointmentToCancel, patientCancelReason);
                            std::cout << u8"Đã hủy lịch: " << selectedAppointmentToCancel << u8" với lý do: " << patientCancelReason << std::endl;
                            showPatientCancelModal = false;
                            patientCancelReason.clear();
                            selectedAppointmentToCancel.clear();
                            activeInputField = -1;
                        }
                        return;
                    }
                    
                    // Click outside modal
                    if (!isMouseOverRect(mousePos, {mx, my}, {mw, mh})) {
                        showPatientCancelModal = false;
                        patientCancelReason.clear();
                        activeInputField = -1;
                        return;
                    }
                    return;
                }
                
                // Handle appointment detail modal
                if (showAppointmentDetailModal) {
                    auto ws2 = window.getSize();
                    float W2 = static_cast<float>(ws2.x);
                    float H2 = static_cast<float>(ws2.y);
                    float mw = 650.f, mh = 500.f;
                    float mx = W2/2.f - mw/2.f;
                    float my = H2/2.f - mh/2.f;
                    
                    // Close button
                    if (isMouseOverRect(mousePos, {mx + mw/2.f - 65, my + mh - 60}, {130, 44})) {
                        showAppointmentDetailModal = false;
                        selectedAppointmentForDetail.clear();
                        return;
                    }
                    
                    // Click outside modal
                    if (!isMouseOverRect(mousePos, {mx, my}, {mw, mh})) {
                        showAppointmentDetailModal = false;
                        selectedAppointmentForDetail.clear();
                        return;
                    }
                    return;
                }
                
                sf::Vector2f contentPos = {panelX + 270.f, 120};
                float startY = contentPos.y + 70.f;
                float listWidth = 880.f;
                
                // Handle appointment card clicks
                auto ids = DataStore::listAppointmentsForPatient(currentUserId);
                struct AptView { std::time_t when; DataStore::AppointmentDetails det; };
                std::vector<AptView> allApts;
                auto nowT = std::time(nullptr);
                
                for (const auto& aid : ids) {
                    auto det = DataStore::readAppointment(aid);
                    std::time_t t;
                    if (toTimeT(det.date, det.time, t)) {
                        allApts.push_back({t, det});
                    }
                }
                
                std::sort(allApts.begin(), allApts.end(), 
                         [](const AptView& a, const AptView& b){ return a.when > b.when; });
                
                int maxShow = static_cast<int>(std::min<size_t>(6, allApts.size()));
                for (int i = 0; i < maxShow; ++i) {
                    const auto& det = allApts[i].det;
                    bool isCancelled = (!det.status.empty() && 
                                      (det.status == "Cancelled" || det.status == "cancelled"));
                    
                    float y = startY + i * 115.f;
                    
                    // Cancel button hitbox (always shown, but only active if not cancelled)
                    if (isMouseOverRect(mousePos, {contentPos.x + listWidth - 120, y + 105 - 42}, {105, 32})) {
                        if (!isCancelled) {
                            selectedAppointmentToCancel = det.appointmentId;
                            patientCancelReason.clear();
                            showPatientCancelModal = true;
                            activeInputField = 400;
                        }
                        return;
                    }
                    
                    // Appointment card body hitbox (excluding cancel button area)
                    if (isMouseOverRect(mousePos, {contentPos.x, y}, {listWidth - 130, 105})) {
                        selectedAppointmentForDetail = det.appointmentId;
                        showAppointmentDetailModal = true;
                        return;
                    }
                }
            }
            if (activePatientMenu == PatientMenuOption::BOOK_APPOINTMENT) {
                sf::Vector2f contentPos = {panelX + 270.f, 120};
                float searchY = contentPos.y + 70;
                // Make search input clickable
                if (isMouseOverRect(mousePos, {contentPos.x, searchY}, {380, 48})) {
                    activeInputField = 200; // Dedicated field ID for search
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
                const float aptWidth = 370.f;
                
                // Handle clicks on "Đặt lịch" buttons in available doctors sidebar
                auto allDoctorIds2 = DataStore::listIDs("Doctor");
                int toShow2 = static_cast<int>(std::min<size_t>(4, allDoctorIds2.size()));
                for (int i = 0; i < toShow2; ++i) {
                    float y = searchY + 115 + i * 95.f;
                    if (isMouseOverRect(mousePos, {appointmentX + aptWidth - 110, y + 50}, {100, 28})) {
                        selectedDoctorForBooking = allDoctorIds2[i];
                        bookingDateText.clear();
                        bookingTimeText.clear();
                        bookingReasonText.clear();
                        showBookingModal = true;
                        activeInputField = 100;
                        return;
                    }
                }
            }
            break;
        }
        case Screen::DOCTOR_DASHBOARD: {
            auto ws = window.getSize();
            float W = static_cast<float>(ws.x);
            float mainX = W / 2.f - 1180.f / 2.f;
            sf::Vector2f contentPos = {mainX + 40.f, 140};
            
            // Logout
            if (isMouseOverRect(mousePos, {W - 170.f, 35.f}, {150.f, 30.f})) {
                resetInputFields();
                switchToScreen(Screen::LOGIN);
            }
            
            // Handle cancel modal
            if (showDoctorCancelModal) {
                auto ws2 = window.getSize();
                float W2 = static_cast<float>(ws2.x);
                float H2 = static_cast<float>(ws2.y);
                float mw = 600.f, mh = 380.f;
                float mx = W2/2.f - mw/2.f;
                float my = H2/2.f - mh/2.f;
                
                float fieldWidth = 540.f;
                float fx = mx + (mw - fieldWidth) / 2.f;
                float startY = my + 100;
                
                // Click on text field
                if (isMouseOverRect(mousePos, {fx, startY}, {fieldWidth, 120})) {
                    activeInputField = 300;
                    return;
                }
                
                bool canCancel = !cancelReasonText.empty();
                float buttonY = my + mh - 60;
                float buttonSpacing = 20.f;
                float totalButtonWidth = 130.f * 2 + buttonSpacing;
                float buttonStartX = mx + (mw - totalButtonWidth) / 2.f;
                
                // Close button
                if (isMouseOverRect(mousePos, {buttonStartX, buttonY}, {130, 44})) {
                    showDoctorCancelModal = false;
                    cancelReasonText.clear();
                    activeInputField = -1;
                    return;
                }
                
                // Confirm button
                if (isMouseOverRect(mousePos, {buttonStartX + 130 + buttonSpacing, buttonY}, {130, 44})) {
                    if (canCancel && !selectedAppointmentToCancel.empty()) {
                        auto det = DataStore::readAppointment(selectedAppointmentToCancel);
                        DataStore::updateAppointmentWithCancelReason(selectedAppointmentToCancel, cancelReasonText);
                        // Send notification to patient with format: "Lịch khám <ID> đã bị hủy với lí do: <reason>"
                        std::string msg = u8"Lịch khám " + selectedAppointmentToCancel + 
                                        u8" đã bị hủy với lí do: " + cancelReasonText;
                        DataStore::appendNotification(det.patientId, msg);
                        std::cout << u8"Đã hủy lịch và gửi thông báo cho bệnh nhân" << std::endl;
                        showDoctorCancelModal = false;
                        cancelReasonText.clear();
                        selectedAppointmentToCancel.clear();
                        activeInputField = -1;
                    }
                    return;
                }
                
                // Click outside modal
                if (!isMouseOverRect(mousePos, {mx, my}, {mw, mh})) {
                    showDoctorCancelModal = false;
                    cancelReasonText.clear();
                    activeInputField = -1;
                    return;
                }
                return;
            }
            
            // Handle doctor appointment detail modal
            if (showDoctorAppointmentDetailModal) {
                auto ws2 = window.getSize();
                float W2 = static_cast<float>(ws2.x);
                float H2 = static_cast<float>(ws2.y);
                float mw = 650.f, mh = 550.f;
                float mx = W2/2.f - mw/2.f;
                float my = H2/2.f - mh/2.f;
                
                // Close button
                if (isMouseOverRect(mousePos, {mx + mw/2.f - 65, my + mh - 60}, {130, 44})) {
                    showDoctorAppointmentDetailModal = false;
                    selectedDoctorAppointmentForDetail.clear();
                    return;
                }
                
                // Click outside modal
                if (!isMouseOverRect(mousePos, {mx, my}, {mw, mh})) {
                    showDoctorAppointmentDetailModal = false;
                    selectedDoctorAppointmentForDetail.clear();
                    return;
                }
                return;
            }
            
            // Update info button
            if (isMouseOverRect(mousePos, {contentPos.x, contentPos.y + 50}, {350, 50})) {
                loadDoctorInfo();
                switchToScreen(Screen::UPDATE_DOCTOR_INFO);
                return;
            }
            
            // Cancel appointment buttons
            float startY = contentPos.y + 120.f;
            float listWidth = 1100.f;
            auto ids = DataStore::listAppointmentsForDoctor(currentUserId);
            
            struct AptView { std::time_t when; DataStore::AppointmentDetails det; };
            std::vector<AptView> allApts;
            
            for (const auto& aid : ids) {
                auto det = DataStore::readAppointment(aid);
                std::time_t t;
                if (toTimeT(det.date, det.time, t)) {
                    allApts.push_back({t, det});
                }
            }
            
            std::sort(allApts.begin(), allApts.end(), 
                     [](const AptView& a, const AptView& b){ return a.when > b.when; });
            
            int maxShow = static_cast<int>(std::min<size_t>(5, allApts.size()));
            for (int i = 0; i < maxShow; ++i) {
                const auto& det = allApts[i].det;
                bool isCancelled = (!det.status.empty() && 
                                  (det.status == "Cancelled" || det.status == "cancelled"));
                
                float y = startY + i * 120.f;
                
                // Cancel button hitbox (only for non-cancelled appointments)
                if (!isCancelled) {
                    if (isMouseOverRect(mousePos, {contentPos.x + listWidth - 140, y + 75}, {130, 28})) {
                        selectedAppointmentToCancel = det.appointmentId;
                        cancelReasonText.clear();
                        showDoctorCancelModal = true;
                        activeInputField = 300;
                        return;
                    }
                }
                
                // Appointment card body hitbox (excluding cancel button area)
                if (isMouseOverRect(mousePos, {contentPos.x, y}, {listWidth - 150, 110})) {
                    selectedDoctorAppointmentForDetail = det.appointmentId;
                    showDoctorAppointmentDetailModal = true;
                    return;
                }
            }
            
            break;
        }
    }
}

void GUI::handleTextInput(char32_t unicode) {
    // Handle cancel reason in doctor dashboard
    if (currentScreen == Screen::DOCTOR_DASHBOARD && activeInputField == 300) {
        if (unicode == 8) {
            if (!cancelReasonText.empty()) {
                while (!cancelReasonText.empty()) {
                    cancelReasonText.pop_back();
                    if (cancelReasonText.empty() || (static_cast<unsigned char>(cancelReasonText.back()) & 0xC0) != 0x80) break;
                }
            }
            return;
        }
        if (unicode >= 32 && unicode < 0x10000) {
            if (unicode < 0x80) {
                cancelReasonText.push_back(static_cast<char>(unicode));
            } else if (unicode < 0x800) {
                cancelReasonText.push_back(static_cast<char>(0xC0 | (unicode >> 6)));
                cancelReasonText.push_back(static_cast<char>(0x80 | (unicode & 0x3F)));
            } else {
                cancelReasonText.push_back(static_cast<char>(0xE0 | (unicode >> 12)));
                cancelReasonText.push_back(static_cast<char>(0x80 | ((unicode >> 6) & 0x3F)));
                cancelReasonText.push_back(static_cast<char>(0x80 | (unicode & 0x3F)));
            }
        }
        return;
    }
    // Handle patient cancel reason in patient dashboard
    if (currentScreen == Screen::PATIENT_DASHBOARD && activeInputField == 400) {
        if (unicode == 8) {
            if (!patientCancelReason.empty()) {
                while (!patientCancelReason.empty()) {
                    patientCancelReason.pop_back();
                    if (patientCancelReason.empty() || (static_cast<unsigned char>(patientCancelReason.back()) & 0xC0) != 0x80) break;
                }
            }
            return;
        }
        if (unicode >= 32 && unicode < 0x10000) {
            if (unicode < 0x80) {
                patientCancelReason.push_back(static_cast<char>(unicode));
            } else if (unicode < 0x800) {
                patientCancelReason.push_back(static_cast<char>(0xC0 | (unicode >> 6)));
                patientCancelReason.push_back(static_cast<char>(0x80 | (unicode & 0x3F)));
            } else {
                patientCancelReason.push_back(static_cast<char>(0xE0 | (unicode >> 12)));
                patientCancelReason.push_back(static_cast<char>(0x80 | ((unicode >> 6) & 0x3F)));
                patientCancelReason.push_back(static_cast<char>(0x80 | (unicode & 0x3F)));
            }
        }
        return;
    }
    // Handle search field in patient dashboard
    if (currentScreen == Screen::PATIENT_DASHBOARD && activeInputField == 200) {
        if (unicode == 8) {
            if (!searchDoctorText.empty()) {
                while (!searchDoctorText.empty()) {
                    searchDoctorText.pop_back();
                    if (searchDoctorText.empty() || (static_cast<unsigned char>(searchDoctorText.back()) & 0xC0) != 0x80) break;
                }
            }
            return;
        }
        if (unicode >= 32 && unicode < 0x10000) {
            if (unicode < 0x80) {
                searchDoctorText.push_back(static_cast<char>(unicode));
            } else if (unicode < 0x800) {
                searchDoctorText.push_back(static_cast<char>(0xC0 | (unicode >> 6)));
                searchDoctorText.push_back(static_cast<char>(0x80 | (unicode & 0x3F)));
            } else {
                searchDoctorText.push_back(static_cast<char>(0xE0 | (unicode >> 12)));
                searchDoctorText.push_back(static_cast<char>(0x80 | ((unicode >> 6) & 0x3F)));
                searchDoctorText.push_back(static_cast<char>(0x80 | (unicode & 0x3F)));
            }
        }
        return;
    }
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
