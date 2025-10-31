#include "GUI.h"
#include "DataStore.h"
#include "gui/GuiInternal.h"
#include <algorithm>
#include <ctime>

void GUI::renderDoctorDashboard() {
    auto ws = window.getSize();
    float W = static_cast<float>(ws.x);
    float H = static_cast<float>(ws.y);
    
    // Header bar
    sf::RectangleShape headerBar({W, 70});
    headerBar.setPosition({0, 0});
    headerBar.setFillColor(sf::Color(25, 35, 50));
    window.draw(headerBar);
    
    // Logo
    sf::Text logoIcon = makeSymbolText(symbolFont, u8"⚕", 32);
    logoIcon.setPosition({40, 18});
    logoIcon.setFillColor(sf::Color(76, 175, 80));
    window.draw(logoIcon);
    
    // Get doctor name
    auto dinfo = DataStore::readDoctorInfo(currentUserId);
    std::string doctorName = dinfo.name.empty() ? inputUsername : dinfo.name;
    
    // Title - just "Bác sĩ"
    sf::Text appTitle = makeText(font, u8"Bác sĩ", 26);
    appTitle.setPosition({85, 21});
    appTitle.setFillColor(sf::Color::White);
    appTitle.setStyle(sf::Text::Bold);
    window.draw(appTitle);
    
    // Welcome message with doctor name
    float rightPad = 40.f;
    sf::Text userAvatar = makeSymbolText(symbolFont, u8"◉", 26);
    sf::FloatRect avatarBounds = userAvatar.getLocalBounds();
    userAvatar.setPosition({W - rightPad - avatarBounds.size.x, 17});
    userAvatar.setFillColor(sf::Color(76, 175, 80));
    window.draw(userAvatar);
    
    sf::Text userName = makeText(font, u8"Chào mừng bác sĩ, " + doctorName, 14);
    sf::FloatRect userBounds = userName.getLocalBounds();
    userName.setPosition({W - rightPad - 35.f - userBounds.size.x, 20});
    userName.setFillColor(sf::Color::White);
    window.draw(userName);
    
    sf::Text logoutIcon = makeSymbolText(symbolFont, u8"⊗", 16);
    sf::FloatRect logoutIconBounds = logoutIcon.getLocalBounds();
    logoutIcon.setPosition({W - rightPad - logoutIconBounds.size.x, 44});
    logoutIcon.setFillColor(sf::Color(220, 100, 100));
    window.draw(logoutIcon);
    
    sf::Text logoutLink = makeText(font, u8"Đăng xuất", 13);
    sf::FloatRect logoutBounds = logoutLink.getLocalBounds();
    logoutLink.setPosition({W - rightPad - 35.f - logoutBounds.size.x, 46});
    logoutLink.setFillColor(sf::Color(150, 180, 220));
    window.draw(logoutLink);
    
    // Main panel
    float mainW = 1180.f;
    float mainH = 680.f;
    float mainX = W / 2.f - mainW / 2.f;
    float mainY = 100.f;
    sf::RectangleShape mainPanel({mainW, mainH});
    mainPanel.setPosition({mainX, mainY});
    mainPanel.setFillColor(sf::Color::White);
    window.draw(mainPanel);
    
    sf::Vector2f contentPos = {mainX + 40.f, 140};
    sf::Text contentTitle = makeText(font, u8"Lịch khám của bác sĩ", 28);
    contentTitle.setPosition(contentPos);
    contentTitle.setFillColor(sf::Color(25, 35, 50));
    contentTitle.setStyle(sf::Text::Bold);
    window.draw(contentTitle);
    
    // Button to update info
    drawButton({contentPos.x, contentPos.y + 50}, {350, 50}, 
              u8"Cập nhật thông tin cá nhân",
              sf::Color(30, 136, 229), sf::Color::White, window);
    
    // Display all appointments (including cancelled)
    float startY = contentPos.y + 120.f;
    float listWidth = 1100.f;
    
    auto ids = DataStore::listAppointmentsForDoctor(currentUserId);
    
    if (ids.empty()) {
        // Empty state
        sf::RectangleShape emptyBox({listWidth, 220});
        emptyBox.setPosition({contentPos.x, startY});
        emptyBox.setFillColor(sf::Color(250, 252, 255));
        emptyBox.setOutlineThickness(2);
        emptyBox.setOutlineColor(sf::Color(220, 230, 240));
        window.draw(emptyBox);

        sf::Text emptyIcon = makeSymbolText(symbolFont, u8"📅", 52);
        sf::FloatRect iconBounds = emptyIcon.getLocalBounds();
        emptyIcon.setPosition({contentPos.x + listWidth/2.f - iconBounds.size.x/2.f, startY + 40});
        emptyIcon.setFillColor(sf::Color(150, 170, 190));
        window.draw(emptyIcon);

        sf::Text emptyText = makeText(font, u8"Chưa có lịch khám nào", 16);
        sf::FloatRect textBounds = emptyText.getLocalBounds();
        emptyText.setPosition({contentPos.x + listWidth/2.f - textBounds.size.x/2.f, startY + 110});
        emptyText.setFillColor(sf::Color(120, 140, 160));
        window.draw(emptyText);
    } else {
        // Display all appointments
        struct AptView { std::time_t when; DataStore::AppointmentDetails det; };
        std::vector<AptView> allApts;
        
        for (const auto& aid : ids) {
            auto det = DataStore::readAppointment(aid);
            std::time_t t;
            if (toTimeT(det.date, det.time, t)) {
                allApts.push_back({t, det});
            }
        }
        
        // Sort by date/time (most recent first)
        std::sort(allApts.begin(), allApts.end(), 
                 [](const AptView& a, const AptView& b){ return a.when > b.when; });
        
        int maxShow = static_cast<int>(std::min<size_t>(5, allApts.size()));
        for (int i = 0; i < maxShow; ++i) {
            const auto& det = allApts[i].det;
            auto pinfo = DataStore::readPatientInfo(det.patientId);
            std::string pname = pinfo.name.empty() ? (u8"BN. " + det.patientId) : pinfo.name;
            std::string reason = det.reason.empty() ? u8"Không ghi chú" : det.reason;
            std::string dt = det.date + " " + det.time;
            bool isCancelled = (!det.status.empty() && 
                              (det.status == "Cancelled" || det.status == "cancelled"));
            
            float y = startY + i * 120.f;
            
            // Appointment card
            sf::RectangleShape card({listWidth, 110});
            card.setPosition({contentPos.x, y});
            card.setFillColor(isCancelled ? sf::Color(245, 245, 245) : sf::Color::White);
            card.setOutlineThickness(1);
            card.setOutlineColor(sf::Color(220, 230, 240));
            window.draw(card);
            
            sf::Text avatar = makeSymbolText(symbolFont, u8"👤", 28);
            avatar.setPosition({contentPos.x + 14, y + 12});
            avatar.setFillColor(isCancelled ? sf::Color(150, 150, 150) : sf::Color(30, 136, 229));
            window.draw(avatar);
            
            sf::Text name = makeText(font, pname, 16);
            name.setPosition({contentPos.x + 54, y + 12});
            name.setFillColor(sf::Color(25, 35, 50));
            name.setStyle(sf::Text::Bold);
            window.draw(name);
            
            sf::Text dateText = makeText(font, dt, 14);
            dateText.setPosition({contentPos.x + 54, y + 38});
            dateText.setFillColor(sf::Color(80, 100, 120));
            window.draw(dateText);
            
            sf::Text reasonText = makeText(font, u8"Lý do: " + reason, 13);
            reasonText.setPosition({contentPos.x + 54, y + 62});
            reasonText.setFillColor(sf::Color(100, 120, 140));
            window.draw(reasonText);
            
            if (isCancelled) {
                // Show cancelled badge
                sf::Text cancelledBadge = makeText(font, u8"ĐÃ HỦY", 14);
                cancelledBadge.setPosition({contentPos.x + listWidth - 180, y + 15});
                cancelledBadge.setFillColor(sf::Color(200, 50, 50));
                cancelledBadge.setStyle(sf::Text::Bold);
                window.draw(cancelledBadge);
                
                // Show cancel reason if available
                if (!det.cancelReason.empty()) {
                    sf::Text cancelReasonText = makeText(font, u8"Lý do hủy: " + det.cancelReason, 12);
                    cancelReasonText.setPosition({contentPos.x + 54, y + 84});
                    cancelReasonText.setFillColor(sf::Color(200, 50, 50));
                    window.draw(cancelReasonText);
                }
            } else {
                // Button for adding medical record
                drawButton({contentPos.x + listWidth - 290, y + 75}, {140, 28}, u8"+ Lịch sử khám",
                          sf::Color(76, 175, 80), sf::Color::White, window);
                
                drawButton({contentPos.x + listWidth - 140, y + 75}, {130, 28}, u8"Hủy lịch",
                          sf::Color(244, 67, 54), sf::Color::White, window);
            }
        }
    }
    
    // Cancel modal for doctor
    if (showDoctorCancelModal) {
        auto ws2 = window.getSize();
        float W2 = static_cast<float>(ws2.x);
        float H2 = static_cast<float>(ws2.y);
        sf::RectangleShape overlay({W2, H2});
        overlay.setPosition({0,0});
        overlay.setFillColor(sf::Color(0,0,0,120));
        window.draw(overlay);
        
        float mw = 600.f, mh = 380.f;
        float mx = W2/2.f - mw/2.f;
        float my = H2/2.f - mh/2.f;
        sf::RectangleShape modal({mw, mh});
        modal.setPosition({mx, my});
        modal.setFillColor(sf::Color::White);
        window.draw(modal);
        
        sf::Text mtitle = makeText(font, u8"Hủy lịch khám", 22);
        mtitle.setStyle(sf::Text::Bold);
        sf::FloatRect titleBounds = mtitle.getLocalBounds();
        mtitle.setPosition({mx + mw/2.f - titleBounds.size.x/2.f, my + 24});
        mtitle.setFillColor(sf::Color(25,35,50));
        window.draw(mtitle);
        
        float fieldWidth = 540.f;
        float fx = mx + (mw - fieldWidth) / 2.f;
        float startY = my + 80;
        
        drawInputField({fx, startY}, {fieldWidth, 120}, u8"Lý do hủy lịch *", 
                      cancelReasonText, activeInputField == 300, false, window);
        
        bool canCancel = !cancelReasonText.empty();
        auto mp2 = sf::Mouse::getPosition(window);
        sf::Vector2f mpos2 = {static_cast<float>(mp2.x), static_cast<float>(mp2.y)};
        
        float buttonY = my + mh - 60;
        float buttonSpacing = 20.f;
        float totalButtonWidth = 130.f * 2 + buttonSpacing;
        float buttonStartX = mx + (mw - totalButtonWidth) / 2.f;
        
        bool cancelHover = isMouseOverRect(mpos2, {buttonStartX, buttonY}, {130, 44});
        bool confirmHover = isMouseOverRect(mpos2, {buttonStartX + 130 + buttonSpacing, buttonY}, {130, 44});
        
        drawButton({buttonStartX, buttonY}, {130, 44}, u8"Đóng",
                   sf::Color(189,189,189), sf::Color::White, true, cancelHover, window);
        drawButton({buttonStartX + 130 + buttonSpacing, buttonY}, {130, 44}, u8"Xác nhận",
                   sf::Color(244,67,54), sf::Color::White, canCancel, confirmHover, window);
    }
    
    // Doctor appointment detail modal
    if (showDoctorAppointmentDetailModal && !selectedDoctorAppointmentForDetail.empty()) {
        auto ws2 = window.getSize();
        float W2 = static_cast<float>(ws2.x);
        float H2 = static_cast<float>(ws2.y);
        sf::RectangleShape overlay({W2, H2});
        overlay.setPosition({0,0});
        overlay.setFillColor(sf::Color(0,0,0,120));
        window.draw(overlay);
        
        auto det = DataStore::readAppointment(selectedDoctorAppointmentForDetail);
        auto pinfo = DataStore::readPatientInfo(det.patientId);
        
        float mw = 650.f, mh = 550.f;
        float mx = W2/2.f - mw/2.f;
        float my = H2/2.f - mh/2.f;
        sf::RectangleShape modal({mw, mh});
        modal.setPosition({mx, my});
        modal.setFillColor(sf::Color::White);
        window.draw(modal);
        
        sf::Text mtitle = makeText(font, u8"Chi tiết lịch khám", 24);
        mtitle.setStyle(sf::Text::Bold);
        sf::FloatRect titleBounds = mtitle.getLocalBounds();
        mtitle.setPosition({mx + mw/2.f - titleBounds.size.x/2.f, my + 24});
        mtitle.setFillColor(sf::Color(25,35,50));
        window.draw(mtitle);
        
        float detailY = my + 80;
        float lineHeight = 45.f;
        
        // Patient name
        sf::Text patientLabel = makeText(font, u8"Bệnh nhân:", 16);
        patientLabel.setPosition({mx + 40, detailY});
        patientLabel.setFillColor(sf::Color(80, 100, 120));
        patientLabel.setStyle(sf::Text::Bold);
        window.draw(patientLabel);
        
        std::string pname = pinfo.name.empty() ? (u8"BN. " + det.patientId) : pinfo.name;
        sf::Text patientValue = makeText(font, pname, 16);
        patientValue.setPosition({mx + 200, detailY});
        patientValue.setFillColor(sf::Color(25, 35, 50));
        window.draw(patientValue);
        
        // Patient phone
        if (!pinfo.phone.empty()) {
            sf::Text phoneLabel = makeText(font, u8"Số điện thoại:", 16);
            phoneLabel.setPosition({mx + 40, detailY + lineHeight});
            phoneLabel.setFillColor(sf::Color(80, 100, 120));
            phoneLabel.setStyle(sf::Text::Bold);
            window.draw(phoneLabel);
            
            sf::Text phoneValue = makeText(font, pinfo.phone, 16);
            phoneValue.setPosition({mx + 200, detailY + lineHeight});
            phoneValue.setFillColor(sf::Color(25, 35, 50));
            window.draw(phoneValue);
        }
        
        // Date
        sf::Text dateLabel = makeText(font, u8"Ngày khám:", 16);
        dateLabel.setPosition({mx + 40, detailY + lineHeight * 2});
        dateLabel.setFillColor(sf::Color(80, 100, 120));
        dateLabel.setStyle(sf::Text::Bold);
        window.draw(dateLabel);
        
        sf::Text dateValue = makeText(font, det.date, 16);
        dateValue.setPosition({mx + 200, detailY + lineHeight * 2});
        dateValue.setFillColor(sf::Color(25, 35, 50));
        window.draw(dateValue);
        
        // Time
        sf::Text timeLabel = makeText(font, u8"Giờ khám:", 16);
        timeLabel.setPosition({mx + 40, detailY + lineHeight * 3});
        timeLabel.setFillColor(sf::Color(80, 100, 120));
        timeLabel.setStyle(sf::Text::Bold);
        window.draw(timeLabel);
        
        sf::Text timeValue = makeText(font, det.time, 16);
        timeValue.setPosition({mx + 200, detailY + lineHeight * 3});
        timeValue.setFillColor(sf::Color(25, 35, 50));
        window.draw(timeValue);
        
        // Reason
        sf::Text reasonLabel = makeText(font, u8"Lý do khám:", 16);
        reasonLabel.setPosition({mx + 40, detailY + lineHeight * 4});
        reasonLabel.setFillColor(sf::Color(80, 100, 120));
        reasonLabel.setStyle(sf::Text::Bold);
        window.draw(reasonLabel);
        
        std::string reason = det.reason.empty() ? u8"Không ghi chú" : det.reason;
        sf::Text reasonValue = makeText(font, reason, 16);
        reasonValue.setPosition({mx + 200, detailY + lineHeight * 4});
        reasonValue.setFillColor(sf::Color(25, 35, 50));
        window.draw(reasonValue);
        
        // Status
        sf::Text statusLabel = makeText(font, u8"Trạng thái:", 16);
        statusLabel.setPosition({mx + 40, detailY + lineHeight * 5});
        statusLabel.setFillColor(sf::Color(80, 100, 120));
        statusLabel.setStyle(sf::Text::Bold);
        window.draw(statusLabel);
        
        bool isCancelled = (!det.status.empty() && 
                           (det.status == "Cancelled" || det.status == "cancelled"));
        std::string statusText = isCancelled ? u8"Đã hủy" : u8"Đã đặt";
        sf::Text statusValue = makeText(font, statusText, 16);
        statusValue.setPosition({mx + 200, detailY + lineHeight * 5});
        statusValue.setFillColor(isCancelled ? sf::Color(200, 50, 50) : sf::Color(76, 175, 80));
        statusValue.setStyle(sf::Text::Bold);
        window.draw(statusValue);
        
        // Cancel reason (if cancelled and reason available)
        if (isCancelled && !det.cancelReason.empty()) {
            sf::Text cancelReasonLabel = makeText(font, u8"Lý do hủy:", 16);
            cancelReasonLabel.setPosition({mx + 40, detailY + lineHeight * 6});
            cancelReasonLabel.setFillColor(sf::Color(80, 100, 120));
            cancelReasonLabel.setStyle(sf::Text::Bold);
            window.draw(cancelReasonLabel);
            
            sf::Text cancelReasonValue = makeText(font, det.cancelReason, 16);
            cancelReasonValue.setPosition({mx + 200, detailY + lineHeight * 6});
            cancelReasonValue.setFillColor(sf::Color(200, 50, 50));
            window.draw(cancelReasonValue);
        }
        
        // Close button
        auto mp3 = sf::Mouse::getPosition(window);
        sf::Vector2f mpos3 = {static_cast<float>(mp3.x), static_cast<float>(mp3.y)};
        bool closeHover = isMouseOverRect(mpos3, {mx + mw/2.f - 65, my + mh - 60}, {130, 44});
        
        drawButton({mx + mw/2.f - 65, my + mh - 60}, {130, 44}, u8"Đóng",
                   sf::Color(30, 136, 229), sf::Color::White, true, closeHover, window);
    }

    // Add Medical Record Modal
    if (showAddMedicalRecordModal) {
        std::cout << "Rendering medical record modal..." << std::endl;
        // Overlay
        auto ws3 = window.getSize();
        float W3 = static_cast<float>(ws3.x);
        float H3 = static_cast<float>(ws3.y);
        sf::RectangleShape overlay({W3, H3});
        overlay.setPosition({0, 0});
        overlay.setFillColor(sf::Color(0, 0, 0, 150));
        window.draw(overlay);

        // Modal box
        float mw = 600.f;
        float mh = 620.f;
        float mx = W3/2.f - mw/2.f;
        float my = H3/2.f - mh/2.f;

        sf::RectangleShape modalBg({mw, mh});
        modalBg.setPosition({mx, my});
        modalBg.setFillColor(sf::Color::White);
        modalBg.setOutlineThickness(1);
        modalBg.setOutlineColor(sf::Color(200, 200, 200));
        window.draw(modalBg);

        // Title
        sf::Text title = makeText(font, u8"Thêm Hồ Sơ Khám Bệnh", 24);
        title.setPosition({mx + 30, my + 20});
        title.setFillColor(sf::Color(30, 136, 229));
        title.setStyle(sf::Text::Bold);
        window.draw(title);

        float fieldX = mx + 30;
        float fieldY = my + 80;
        float fieldWidth = mw - 60;
        float spacing = 90;

        // Field 1: Diagnosis
        {
            sf::Text label = makeText(font, u8"Chẩn đoán:", 16);
            label.setPosition({fieldX, fieldY});
            label.setFillColor(sf::Color(60, 60, 60));
            window.draw(label);

            bool isActive = (activeInputField == 100);
            drawInputField({fieldX, fieldY + 25}, {fieldWidth, 40}, "", medicalRecordDiagnosis,
                          isActive, false, window);
        }

        // Field 2: Symptoms
        {
            sf::Text label = makeText(font, u8"Triệu chứng:", 16);
            label.setPosition({fieldX, fieldY + spacing});
            label.setFillColor(sf::Color(60, 60, 60));
            window.draw(label);

            bool isActive = (activeInputField == 101);
            drawInputField({fieldX, fieldY + spacing + 25}, {fieldWidth, 40}, "", medicalRecordSymptoms,
                          isActive, false, window);
        }

        // Field 3: Prescription
        {
            sf::Text label = makeText(font, u8"Đơn thuốc:", 16);
            label.setPosition({fieldX, fieldY + spacing * 2});
            label.setFillColor(sf::Color(60, 60, 60));
            window.draw(label);

            bool isActive = (activeInputField == 102);
            drawInputField({fieldX, fieldY + spacing * 2 + 25}, {fieldWidth, 40}, "", medicalRecordPrescription,
                          isActive, false, window);
        }

        // Field 4: Notes
        {
            sf::Text label = makeText(font, u8"Ghi chú:", 16);
            label.setPosition({fieldX, fieldY + spacing * 3});
            label.setFillColor(sf::Color(60, 60, 60));
            window.draw(label);

            bool isActive = (activeInputField == 103);
            drawInputField({fieldX, fieldY + spacing * 3 + 25}, {fieldWidth, 40}, "", medicalRecordNotes,
                          isActive, false, window);
        }

        // Field 5: Follow-up Date
        {
            sf::Text label = makeText(font, u8"Ngày tái khám (DD/MM/YYYY):", 16);
            label.setPosition({fieldX, fieldY + spacing * 4});
            label.setFillColor(sf::Color(60, 60, 60));
            window.draw(label);

            bool isActive = (activeInputField == 104);
            drawInputField({fieldX, fieldY + spacing * 4 + 25}, {fieldWidth, 40}, "", medicalRecordFollowUpDate,
                          isActive, false, window);
        }

        // Buttons
        auto mp4 = sf::Mouse::getPosition(window);
        sf::Vector2f mpos4 = {static_cast<float>(mp4.x), static_cast<float>(mp4.y)};
        
        bool saveHover = isMouseOverRect(mpos4, {mx + mw/2.f - 210, my + mh - 60}, {180, 44});
        bool cancelHover = isMouseOverRect(mpos4, {mx + mw/2.f + 30, my + mh - 60}, {180, 44});

        drawButton({mx + mw/2.f - 210, my + mh - 60}, {180, 44}, u8"Lưu hồ sơ",
                   sf::Color(76, 175, 80), sf::Color::White, true, saveHover, window);

        drawButton({mx + mw/2.f + 30, my + mh - 60}, {180, 44}, u8"Hủy",
                   sf::Color(158, 158, 158), sf::Color::White, true, cancelHover, window);
    }
}

