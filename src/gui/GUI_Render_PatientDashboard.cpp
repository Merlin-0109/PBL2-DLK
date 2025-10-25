#include "GUI.h"
#include "DataStore.h"
#include "gui/GuiInternal.h"
#include <algorithm>
#include <ctime>
#include <sstream>
#include <iomanip>

void GUI::renderPatientDashboard() {
    auto ws = window.getSize();
    float W = static_cast<float>(ws.x);
    float H = static_cast<float>(ws.y);
    sf::RectangleShape background({W, H});
    background.setPosition({0, 0});
    background.setFillColor(sf::Color(245, 248, 250));
    window.draw(background);
    sf::RectangleShape headerBar({W, 70});
    headerBar.setPosition({0, 0});
    headerBar.setFillColor(sf::Color(25, 35, 50));
    window.draw(headerBar);
    sf::Text logoIcon = makeText(font, u8"⚕", 32);
    logoIcon.setPosition({40, 18});
    logoIcon.setFillColor(sf::Color(76, 175, 80));
    window.draw(logoIcon);
    sf::Text appTitle = makeText(font, "MediCare", 26);
    appTitle.setPosition({85, 21});
    appTitle.setFillColor(sf::Color::White);
    appTitle.setStyle(sf::Text::Bold);
    window.draw(appTitle);
    float rightPad = 40.f;
    sf::Text userAvatar = makeSymbolText(symbolFont, u8"◉", 26);
    sf::FloatRect avatarBounds = userAvatar.getLocalBounds();
    userAvatar.setPosition({W - rightPad - avatarBounds.size.x, 17});
    userAvatar.setFillColor(sf::Color(76, 175, 80));
    window.draw(userAvatar);
    sf::Text userName = makeText(font, u8"Xin chào, " + inputUsername, 14);
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
    float mainW = 1180.f;
    float mainH = 680.f;
    float mainX = W / 2.f - mainW / 2.f;
    float mainY = 100.f;
    sf::RectangleShape mainPanel({mainW, mainH});
    mainPanel.setPosition({mainX, mainY});
    mainPanel.setFillColor(sf::Color::White);
    window.draw(mainPanel);
    drawNavigationMenu({mainX, mainY}, {250, 680}, window);
    sf::Vector2f contentPos = {mainX + 270.f, 120};
    std::string sectionTitle = u8"Đặt lịch khám";
    if (activePatientMenu == PatientMenuOption::MY_APPOINTMENTS) {
        sectionTitle = u8"Lịch khám của tôi";
    } else if (activePatientMenu == PatientMenuOption::NOTIFICATIONS) {
        sectionTitle = u8"Thông báo";
    } else if (activePatientMenu == PatientMenuOption::UPDATE_INFO) {
        sectionTitle = u8"Cập nhật thông tin";
    }
    sf::Text contentTitle = makeText(font, sectionTitle, 28);
    contentTitle.setPosition(contentPos);
    contentTitle.setFillColor(sf::Color(25, 35, 50));
    contentTitle.setStyle(sf::Text::Bold);
    window.draw(contentTitle);

    if (activePatientMenu == PatientMenuOption::BOOK_APPOINTMENT) {
        float searchY = contentPos.y + 70;
        sf::Text searchLabel = makeText(font, u8"Tìm kiếm bác sĩ:", 16);
        searchLabel.setPosition({contentPos.x, searchY - 28});
        searchLabel.setFillColor(sf::Color(60, 70, 90));
        searchLabel.setStyle(sf::Text::Bold);
        window.draw(searchLabel);
        drawInputField({contentPos.x, searchY}, {380, 48}, u8"Nhập tên bác sĩ...",
                       searchDoctorText, false, false, window);
        drawButton({contentPos.x + 390, searchY}, {140, 48}, u8"Tìm kiếm",
                   sf::Color(76, 175, 80), sf::Color::White, window);
        sf::Text filterLabel = makeText(font, u8"Bộ lọc tìm kiếm:", 16);
        filterLabel.setPosition({contentPos.x, searchY + 70});
        filterLabel.setFillColor(sf::Color(60, 70, 90));
        filterLabel.setStyle(sf::Text::Bold);
        window.draw(filterLabel);
        drawButton({contentPos.x, searchY + 100}, {185, 48}, u8"⚕ Chuyên khoa",
                   sf::Color(30, 136, 229), sf::Color::White, window);
        drawButton({contentPos.x + 195, searchY + 100}, {185, 48}, u8"Chọn ngày",
                   sf::Color(30, 136, 229), sf::Color::White, window);
    float listStartY = searchY + 170;
        auto allDoctorIds = DataStore::listIDs("Doctor");
        std::vector<std::string> filtered;
        auto toLower = [](std::string s){ for(char& c: s) c = static_cast<char>(std::tolower((unsigned char)c)); return s; };
        std::string q = toLower(searchDoctorText);
        for (const auto& did : allDoctorIds) {
            if (q.empty()) { filtered.push_back(did); continue; }
            auto info = DataStore::readDoctorInfo(did);
            std::string hay = toLower(info.name + " " + info.specialization + " " + did);
            if (hay.find(q) != std::string::npos) filtered.push_back(did);
        }
        int showCount = static_cast<int>(std::min<size_t>(6, filtered.size()));
        if (showCount > 0) {
            for (int i = 0; i < showCount; ++i) {
                const std::string& did = filtered[i];
                auto info = DataStore::readDoctorInfo(did);
                std::string name = info.name.empty() ? (u8"Bác sĩ " + did) : info.name;
                std::string spec = info.specialization.empty() ? u8"Chưa cập nhật" : info.specialization;
                // Thu hẹp cột danh sách bác sĩ để tránh đè chữ với cột bên phải
                drawDoctorListItem({contentPos.x, listStartY + i * 90.f}, {520, 80}, name, spec, did, window);
            }
        } else {
            sf::Text empty = makeText(font, u8"Không tìm thấy bác sĩ phù hợp.", 14);
            empty.setPosition({contentPos.x, listStartY});
            empty.setFillColor(sf::Color(120, 140, 160));
            window.draw(empty);
        }
        if (showDatePicker) {
            auto ws2 = window.getSize();
            float W2 = static_cast<float>(ws2.x);
            float H2 = static_cast<float>(ws2.y);
            sf::RectangleShape overlay({W2, H2});
            overlay.setPosition({0,0});
            overlay.setFillColor(sf::Color(0,0,0,120));
            window.draw(overlay);
            float mw = 420.f, mh = 360.f;
            float mx = W2/2.f - mw/2.f;
            float my = H2/2.f - mh/2.f;
            sf::RectangleShape modal({mw, mh});
            modal.setPosition({mx, my});
            modal.setFillColor(sf::Color::White);
            window.draw(modal);
            sf::Text mtitle = makeText(font, u8"Chọn ngày", 20);
            mtitle.setStyle(sf::Text::Bold);
            mtitle.setPosition({mx + 20, my + 16});
            mtitle.setFillColor(sf::Color(25,35,50));
            window.draw(mtitle);
            auto now = std::time(nullptr);
            std::tm base = *std::localtime(&now);
            float ox = mx + 20, oy = my + 60;
            for (int i = 0; i < 14; ++i) {
                std::tm t = base; t.tm_mday += i; std::mktime(&t);
                std::ostringstream oss; oss << std::setfill('0')
                    << std::setw(2) << t.tm_mday << "/" << std::setw(2) << (t.tm_mon+1) << "/" << (t.tm_year+1900);
                std::string dstr = oss.str();
                float col = (i % 2 == 0) ? 0.f : 200.f;
                float row = static_cast<float>(i / 2) * 42.f;
                drawButton({ox + col, oy + row}, {180, 36}, dstr,
                           sf::Color(30,136,229), sf::Color::White, window);
            }
        }
    if (showBookingModal) {
            auto ws2 = window.getSize();
            float W2 = static_cast<float>(ws2.x);
            float H2 = static_cast<float>(ws2.y);
            sf::RectangleShape overlay({W2, H2});
            overlay.setPosition({0,0});
            overlay.setFillColor(sf::Color(0,0,0,120));
            window.draw(overlay);
            float mw = 600.f, mh = 480.f;
            float mx = W2/2.f - mw/2.f;
            float my = H2/2.f - mh/2.f;
            sf::RectangleShape modal({mw, mh});
            modal.setPosition({mx, my});
            modal.setFillColor(sf::Color::White);
            window.draw(modal);
            sf::Text mtitle = makeText(font, u8"Đặt lịch khám", 22);
            mtitle.setStyle(sf::Text::Bold);
            sf::FloatRect titleBounds = mtitle.getLocalBounds();
            mtitle.setPosition({mx + mw/2.f - titleBounds.size.x/2.f, my + 24});
            mtitle.setFillColor(sf::Color(25,35,50));
            window.draw(mtitle);
            float fieldWidth = 540.f;
            float fx = mx + (mw - fieldWidth) / 2.f;
            float startY = my + 80;
            float spacing = 85.f;
            drawInputField({fx, startY}, {fieldWidth, 48}, u8"Ngày (DD/MM/YYYY)", bookingDateText, activeInputField == 100, false, window);
            drawInputField({fx, startY + spacing}, {fieldWidth, 48}, u8"Giờ (HH:MM)", bookingTimeText, activeInputField == 101, false, window);
            drawInputField({fx, startY + spacing * 2}, {fieldWidth, 90}, u8"Lý do khám (tuỳ chọn)", bookingReasonText, activeInputField == 102, false, window);
            bool canBook = false;
            {
                if (!selectedDoctorForBooking.empty()) {
                    int dd,mm,yy,hh,mi; std::time_t tmp;
                    canBook = parseDateDDMMYYYY(bookingDateText, dd, mm, yy) &&
                              parseTimeHHMM(bookingTimeText, hh, mi) &&
                              toTimeT(bookingDateText, bookingTimeText, tmp) && tmp >= std::time(nullptr);
                }
            }
            auto mp2 = sf::Mouse::getPosition(window);
            sf::Vector2f mpos2 = {static_cast<float>(mp2.x), static_cast<float>(mp2.y)};
            float buttonY = my + mh - 60;
            float buttonSpacing = 20.f;
            float totalButtonWidth = 130.f * 2 + buttonSpacing;
            float buttonStartX = mx + (mw - totalButtonWidth) / 2.f;
            bool cancelHover = isMouseOverRect(mpos2, {buttonStartX, buttonY}, {130, 44});
            bool saveHover = isMouseOverRect(mpos2, {buttonStartX + 130 + buttonSpacing, buttonY}, {130, 44});
            drawButton({buttonStartX, buttonY}, {130, 44}, u8"Hủy",
                       sf::Color(189,189,189), sf::Color::White, true, cancelHover, window);
            drawButton({buttonStartX + 130 + buttonSpacing, buttonY}, {130, 44}, u8"Lưu",
                       sf::Color(76,175,80), sf::Color::White, canBook, saveHover, window);
        }
    // Đẩy cột "Lịch khám sắp tới" sang phải 20px và thu hẹp chiều rộng để không chạm cột trái
    float appointmentX = contentPos.x + 540;
        sf::Text calendarIcon = makeSymbolText(symbolFont, u8"📅", 20);
        calendarIcon.setPosition({appointmentX, searchY + 68});
        calendarIcon.setFillColor(sf::Color(30, 136, 229));
        window.draw(calendarIcon);
        sf::Text upcomingTitle = makeText(font, u8"Lịch khám sắp tới", 18);
        upcomingTitle.setPosition({appointmentX + 30, searchY + 70});
        upcomingTitle.setFillColor(sf::Color(25, 35, 50));
        upcomingTitle.setStyle(sf::Text::Bold);
        window.draw(upcomingTitle);
    const float aptWidth = 370.f; // vừa khít: 910 (content width) - 520 (list) - 20 (gap)
    sf::RectangleShape divider({aptWidth, 2});
        divider.setPosition({appointmentX, searchY + 98});
        divider.setFillColor(sf::Color(220, 230, 240));
        window.draw(divider);
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
        if (toShow > 0) {
            for (int i = 0; i < toShow; ++i) {
                const auto& det = upcoming[i].det;
                auto dinfo = DataStore::readDoctorInfo(det.doctorId);
                std::string dname = dinfo.name.empty() ? (u8"BS. " + det.doctorId) : (u8"BS. " + dinfo.name);
                std::string spec = dinfo.specialization.empty() ? u8"Chưa cập nhật" : dinfo.specialization;
                std::string dt = det.date + " " + det.time;
                float y = (i == 0) ? (searchY + 115) : (searchY + 235);
                drawAppointmentCard({appointmentX, y}, {aptWidth, 105}, dname, spec, dt, window);
            }
        } else {
            sf::RectangleShape emptyBox({aptWidth, 220});
            emptyBox.setPosition({appointmentX, searchY + 115});
            emptyBox.setFillColor(sf::Color(250, 252, 255));
            emptyBox.setOutlineThickness(2);
            emptyBox.setOutlineColor(sf::Color(220, 230, 240));
            window.draw(emptyBox);
            sf::Text emptyIcon = makeSymbolText(symbolFont, u8"📭", 52);
            sf::FloatRect iconBounds = emptyIcon.getLocalBounds();
            emptyIcon.setPosition({appointmentX + 230 - iconBounds.size.x / 2, searchY + 150});
            emptyIcon.setFillColor(sf::Color(150, 170, 190));
            window.draw(emptyIcon);
            sf::Text emptyText = makeText(font, u8"Không có lịch khám nào", 16);
            sf::FloatRect textBounds = emptyText.getLocalBounds();
            emptyText.setPosition({appointmentX + 230 - textBounds.size.x / 2, searchY + 230});
            emptyText.setFillColor(sf::Color(120, 140, 160));
            window.draw(emptyText);
            sf::Text emptySubtext = makeText(font, u8"Tìm kiếm bác sĩ để đặt lịch mới", 13);
            sf::FloatRect subtextBounds = emptySubtext.getLocalBounds();
            emptySubtext.setPosition({appointmentX + 230 - subtextBounds.size.x / 2, searchY + 255});
            emptySubtext.setFillColor(sf::Color(150, 160, 170));
            window.draw(emptySubtext);
        }
    }
    else if (activePatientMenu == PatientMenuOption::NOTIFICATIONS) {
        float startY = contentPos.y + 70.f;
        float itemHeight = 64.f;
        float itemSpacing = 10.f;
        float listWidth = 880.f;
        auto notes = DataStore::readNotifications(currentUserId);
        if (notes.empty()) {
            // Empty state
            sf::RectangleShape emptyBox({listWidth, 220});
            emptyBox.setPosition({contentPos.x, startY});
            emptyBox.setFillColor(sf::Color(250, 252, 255));
            emptyBox.setOutlineThickness(2);
            emptyBox.setOutlineColor(sf::Color(220, 230, 240));
            window.draw(emptyBox);

            sf::Text emptyIcon = makeSymbolText(symbolFont, u8"🔔", 52);
            sf::FloatRect iconBounds = emptyIcon.getLocalBounds();
            emptyIcon.setPosition({contentPos.x + listWidth/2.f - iconBounds.size.x/2.f, startY + 40});
            emptyIcon.setFillColor(sf::Color(150, 170, 190));
            window.draw(emptyIcon);

            sf::Text emptyText = makeText(font, u8"Chưa có thông báo", 16);
            sf::FloatRect textBounds = emptyText.getLocalBounds();
            emptyText.setPosition({contentPos.x + listWidth/2.f - textBounds.size.x/2.f, startY + 110});
            emptyText.setFillColor(sf::Color(120, 140, 160));
            window.draw(emptyText);
        } else {
            int maxShow = static_cast<int>(std::min<size_t>(8, notes.size()));
            for (int i = 0; i < maxShow; ++i) {
                float y = startY + i * (itemHeight + itemSpacing);
                sf::RectangleShape row({listWidth, itemHeight});
                row.setPosition({contentPos.x, y});
                row.setFillColor(sf::Color::White);
                row.setOutlineThickness(1);
                row.setOutlineColor(sf::Color(220, 230, 240));
                window.draw(row);

                sf::Text bell = makeSymbolText(symbolFont, u8"🔔", 22);
                bell.setPosition({contentPos.x + 14, y + 18});
                bell.setFillColor(sf::Color(30, 136, 229));
                window.draw(bell);

                sf::Text msg = makeText(font, notes[i], 14);
                msg.setPosition({contentPos.x + 48, y + 20});
                msg.setFillColor(sf::Color(35, 45, 60));
                window.draw(msg);
            }
        }
    }
    else if (activePatientMenu == PatientMenuOption::UPDATE_INFO) {
        drawButton({contentPos.x, contentPos.y + 80}, {350, 50}, 
                  u8"Cập nhật thông tin cá nhân",
                  sf::Color(30, 136, 229), sf::Color::White, window);
    }
}
