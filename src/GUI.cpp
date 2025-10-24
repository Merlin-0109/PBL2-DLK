#include "GUI.h"
#include "DataStore.h"
#include <iostream>
#include <cstring>
#include <ctime>
#include <sstream>
#include <algorithm>
#include <iomanip>

// Helper function for SFML 3.x with proper UTF-8 support
static sf::Text makeText(const sf::Font& font, const std::string& str, unsigned int size) {
    sf::Text text(font);
    
    // Convert UTF-8 string to sf::String properly
    text.setString(sf::String::fromUtf8(str.begin(), str.end()));
    text.setCharacterSize(size);
    
    return text;
}

// Helper to create symbol text with dedicated symbol font
static sf::Text makeSymbolText(const sf::Font& symbolFont, const std::string& str, unsigned int size) {
    sf::Text text(symbolFont);
    text.setString(sf::String::fromUtf8(str.begin(), str.end()));
    text.setCharacterSize(size);
    return text;
}

// Helper to create UTF-8 sf::String from u8 literal
static sf::String makeUtf8String(const char* u8str) {
    std::string str(u8str);
    return sf::String::fromUtf8(str.begin(), str.end());
}

// Helpers: parse and validate date/time
static bool parseDateDDMMYYYY(const std::string& s, int& d, int& m, int& y) {
    if (s.size() != 10 || s[2] != '/' || s[5] != '/') return false;
    try {
        d = std::stoi(s.substr(0,2));
        m = std::stoi(s.substr(3,2));
        y = std::stoi(s.substr(6,4));
    } catch (...) { return false; }
    if (y < 1900 || y > 2100 || m < 1 || m > 12 || d < 1 || d > 31) return false;
    static const int mdays[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
    int maxd = mdays[m-1];
    // leap year check
    if (m == 2) {
        bool leap = (y%4==0 && (y%100!=0 || y%400==0));
        if (leap) maxd = 29;
    }
    return d <= maxd;
}

static bool parseTimeHHMM(const std::string& s, int& hh, int& mm) {
    if (s.size() != 5 || s[2] != ':') return false;
    try {
        hh = std::stoi(s.substr(0,2));
        mm = std::stoi(s.substr(3,2));
    } catch (...) { return false; }
    return hh >= 0 && hh <= 23 && mm >= 0 && mm <= 59;
}

static bool toTimeT(const std::string& date, const std::string& time, std::time_t& out) {
    int d,m,y,hh,mm;
    if (!parseDateDDMMYYYY(date, d, m, y)) return false;
    if (!parseTimeHHMM(time, hh, mm)) return false;
    std::tm tm{};
    tm.tm_mday = d;
    tm.tm_mon = m - 1;
    tm.tm_year = y - 1900;
    tm.tm_hour = hh;
    tm.tm_min = mm;
    tm.tm_sec = 0;
    tm.tm_isdst = -1;
    out = std::mktime(&tm);
    return out != -1;
}

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
        "assets/Roboto.ttf",                 // Roboto - excellent Vietnamese support
        "assets/fonts/arial.ttf",            // Local Arial
        "C:/Windows/Fonts/segoeui.ttf",      // Segoe UI
        "C:/Windows/Fonts/arial.ttf"         // System Arial fallback
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
        "C:/Windows/Fonts/seguisym.ttf",     // Segoe UI Symbol
        "C:/Windows/Fonts/segoeui.ttf",      // Fallback to regular Segoe UI
        "assets/Roboto.ttf"                  // Final fallback
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
    
    // Configure window for smooth performance
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(false);
    
    sf::Clock deltaClock;
    
    while (window.isOpen()) {
        handleEvents();
        render();
    }
}

void GUI::handleEvents() {
    try {
        while (std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            else if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseButtonPressed->button == sf::Mouse::Button::Left) {
                    sf::Vector2f mousePos(static_cast<float>(mouseButtonPressed->position.x), 
                                         static_cast<float>(mouseButtonPressed->position.y));
                    handleMouseClick(mousePos);
                }
            }
            else if (const auto* textEntered = event->getIf<sf::Event::TextEntered>()) {
                handleTextInput(textEntered->unicode);
            }
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                handleKeyPress(keyPressed->code);
            }
        }
        
        if (cursorClock.getElapsedTime().asSeconds() > 0.5f) {
            showCursor = !showCursor;
            cursorClock.restart();
        }
    } catch (const std::exception& e) {
        std::cerr << "Error in handleEvents: " << e.what() << std::endl;
    }
}

void GUI::handleMouseClick(const sf::Vector2f& mousePos) {
    switch (currentScreen) {
        case Screen::REGISTER_SELECT_ROLE: {
            // Responsive click regions: compute centered positions
            auto ws = window.getSize();
            float W = static_cast<float>(ws.x);
            float panelW = 760.f;
            float panelX = W / 2.f - panelW / 2.f;
            // Role cards
            float cardWidth = 280.f, cardHeight = 130.f, cardSpacing = 20.f;
            float cardsStartX = panelX + (panelW - (2 * cardWidth + cardSpacing)) / 2.f;
            float cardY = 360.f;
            if (isMouseOverRect(mousePos, {cardsStartX, cardY}, {cardWidth, cardHeight})) {
                selectedRole = UserRole::PATIENT;
            }
            else if (isMouseOverRect(mousePos, {cardsStartX + cardWidth + cardSpacing, cardY}, {cardWidth, cardHeight})) {
                selectedRole = UserRole::DOCTOR;
            }
            // Continue button centered
            else if (isMouseOverRect(mousePos, {W / 2.f - 150.f, 520}, {300, 55})) {
                if (selectedRole != UserRole::NONE) {
                    switchToScreen(Screen::REGISTER_INFO);
                }
            }
            // Login link: wider area centered
            else if (isMouseOverRect(mousePos, {W / 2.f - 240.f, 590}, {480, 40})) {
                switchToScreen(Screen::LOGIN);
            }
            break;
        }
        
        case Screen::REGISTER_INFO: {
            // Responsive positions based on centered panel
            {
                auto ws = window.getSize();
                float W = static_cast<float>(ws.x);
                float panelW = 760.f;
                float panelX = W / 2.f - panelW / 2.f;
                float baseX = panelX + 80.f;
                // Input fields: startY=265, spacing=80, size={600, 50}
                for (int i = 0; i < 4; i++) {
                    float yPos = 265 + i * 80;
                    if (isMouseOverRect(mousePos, {baseX, yPos}, {600, 50})) {
                        activeInputField = i;
                        return;
                    }
                }
                // Register button centered
                if (isMouseOverRect(mousePos, {W / 2.f - 150.f, 600}, {300, 55})) {
                    completeRegistration();
                }
                // Back link centered below panel
                else if (isMouseOverRect(mousePos, {W / 2.f - 140.f, 700}, {280, 35})) {
                    switchToScreen(Screen::REGISTER_SELECT_ROLE);
                }
            }
            activeInputField = -1;
            break;
        }
        
        case Screen::LOGIN: {
            // Responsive click regions based on centered panel
            auto ws = window.getSize();
            float W = static_cast<float>(ws.x);
            float panelW = 700.f;
            float panelX = W / 2.f - panelW / 2.f;
            float baseX = panelX + 110.f;
            // Input fields
            if (isMouseOverRect(mousePos, {baseX, 300}, {480, 50})) {
                activeInputField = 0;
            }
            else if (isMouseOverRect(mousePos, {baseX, 390}, {480, 50})) {
                activeInputField = 1;
            }
            // Login button centered
            else if (isMouseOverRect(mousePos, {W / 2.f - 100.f, 480}, {200, 55})) {
                completeLogin();
            }
            // Register link centered
            else if (isMouseOverRect(mousePos, {W / 2.f - 200.f, 560}, {400, 40})) {
                switchToScreen(Screen::REGISTER_SELECT_ROLE);
            }
            else {
                activeInputField = -1;
            }
            break;
        }
        
        case Screen::UPDATE_PATIENT_INFO: {
            // Responsive click regions based on centered panel
            auto ws = window.getSize();
            float W = static_cast<float>(ws.x);
            float panelX = W / 2.f - 800.f / 2.f;
            float baseX = panelX + 80.f;
            // Save button centered at W/2 (check first to avoid overlap with last input field)
            {
                bool canSave = !inputFullName.empty() && !inputPhone.empty() && !inputEmail.empty();
                if (canSave && isMouseOverRect(mousePos, {W / 2.f - 150.f, 800}, {300, 50})) {
                    savePatientInfo();
                    switchToScreen(Screen::PATIENT_DASHBOARD);
                    break;
                }
            }
            // Input fields: startY=200, spacing=75, width=640
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
            // Responsive click regions based on centered panel
            auto ws = window.getSize();
            float W = static_cast<float>(ws.x);
            float panelX = W / 2.f - 800.f / 2.f;
            float baseX = panelX + 80.f;
            // Save button centered at W/2 (check first to avoid overlap with last input field)
            {
                bool canSave = !inputFullName.empty() && !inputPhone.empty() && !inputEmail.empty()
                               && !inputSpecialization.empty() && !inputFaculty.empty();
                if (canSave && isMouseOverRect(mousePos, {W / 2.f - 150.f, 820}, {300, 50})) {
                    saveDoctorInfo();
                    switchToScreen(Screen::DOCTOR_DASHBOARD);
                    break;
                }
            }
            // Input fields: startY=185, spacing=70, width=640
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
            // Navigation menu clicks (relative to centered main panel)
            auto ws = window.getSize();
            float W = static_cast<float>(ws.x);
            float panelX = W / 2.f - 1180.f / 2.f;
            float menuStartY = 140;
            float menuItemHeight = 70;
            // Check menu items
            for (int i = 0; i < 4; i++) {
                float itemY = menuStartY + i * menuItemHeight;
                if (isMouseOverRect(mousePos, {panelX, itemY}, {250, 60})) {
                    activePatientMenu = static_cast<PatientMenuOption>(i);
                    break;
                }
            }
            
            // Logout button area near top-right
            if (isMouseOverRect(mousePos, {W - 170.f, 35.f}, {150.f, 30.f})) {
                resetInputFields();
                switchToScreen(Screen::LOGIN);
            }
            
            // If booking modal is open, handle its clicks first
            if (activePatientMenu == PatientMenuOption::BOOK_APPOINTMENT && showBookingModal) {
                auto ws2 = window.getSize();
                float W2 = static_cast<float>(ws2.x);
                float H2 = static_cast<float>(ws2.y);
                float mw = 600.f, mh = 480.f;
                float mx = W2/2.f - mw/2.f;
                float my = H2/2.f - mh/2.f;
                // Fields (match render positions)
                float fieldWidth = 540.f;
                float fx = mx + (mw - fieldWidth) / 2.f;
                float startY = my + 80;
                float spacing = 85.f;
                if (isMouseOverRect(mousePos, {fx, startY}, {fieldWidth, 48})) { activeInputField = 100; return; }
                if (isMouseOverRect(mousePos, {fx, startY + spacing}, {fieldWidth, 48})) { activeInputField = 101; return; }
                if (isMouseOverRect(mousePos, {fx, startY + spacing * 2}, {fieldWidth, 90})) { activeInputField = 102; return; }
                // Buttons (match render positions)
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
                    // Cancel
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
                // Click outside modal closes it
                if (!isMouseOverRect(mousePos, {mx, my}, {mw, mh})) {
                    showBookingModal = false;
                    activeInputField = -1;
                    return;
                }
                return; // Don't fall through when modal is open
            }

            // If date picker is open
            if (activePatientMenu == PatientMenuOption::BOOK_APPOINTMENT && showDatePicker) {
                auto ws2 = window.getSize();
                float W2 = static_cast<float>(ws2.x);
                float H2 = static_cast<float>(ws2.y);
                float mw = 420.f, mh = 360.f;
                float mx = W2/2.f - mw/2.f;
                float my = H2/2.f - mh/2.f;
                // Generate next 14 days and check clicks on them
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
                        activeInputField = 101; // move focus to time
                        return;
                    }
                }
                // Click outside closes
                if (!isMouseOverRect(mousePos, {mx, my}, {mw, mh})) {
                    showDatePicker = false;
                    return;
                }
                return;
            }

            // Update info button (when on UPDATE_INFO menu)
            if (activePatientMenu == PatientMenuOption::UPDATE_INFO) {
                if (isMouseOverRect(mousePos, {panelX + 270.f, 200.f}, {350, 50})) {
                    loadPatientInfo();
                    switchToScreen(Screen::UPDATE_PATIENT_INFO);
                }
            }

            // Handle clicks on doctor list "Đặt lịch" buttons in booking page
            if (activePatientMenu == PatientMenuOption::BOOK_APPOINTMENT) {
                sf::Vector2f contentPos = {panelX + 270.f, 120};
                float searchY = contentPos.y + 70;
                // Open date picker when clicking "Chọn ngày"
                if (isMouseOverRect(mousePos, {contentPos.x + 195, searchY + 100}, {185, 48})) {
                    showDatePicker = true;
                    return;
                }
                // Build filtered list (same as render)
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
                    // Button rect as in drawDoctorListItem: at right bottom of the item
                    if (isMouseOverRect(mousePos, {contentPos.x + 530 - 120, y + 80 - 40}, {110, 32})) {
                        selectedDoctorForBooking = filtered[i];
                        bookingDateText.clear();
                        bookingTimeText.clear();
                        bookingReasonText.clear();
                        showBookingModal = true;
                        activeInputField = 100; // focus date by default
                        return;
                    }
                }

                // Handle cancel click on upcoming appointment cards (up to 2 shown)
                float appointmentX = contentPos.x + 520;
                // Build upcoming like render
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
                    // Cancel button rect in drawAppointmentCard: {x + size.x - 120, y + size.y - 42} with size {105,32}
                    if (isMouseOverRect(mousePos, {appointmentX + 460 - 120, y + 105 - 42}, {105, 32})) {
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

// [CONTINUED IN NEXT PART - handleTextInput]
void GUI::handleTextInput(char32_t unicode) {
    // Special handling for booking modal on Patient Dashboard
    if (currentScreen == Screen::PATIENT_DASHBOARD && showBookingModal) {
        if (unicode == 8) { // backspace
            std::string* target = nullptr;
            if (activeInputField == 100) target = &bookingDateText;
            else if (activeInputField == 101) target = &bookingTimeText;
            else if (activeInputField == 102) target = &bookingReasonText;
            if (target && !target->empty()) {
                // Handle UTF-8 multibyte: remove last full character
                while (!target->empty()) {
                    target->pop_back();
                    if (target->empty() || (static_cast<unsigned char>(target->back()) & 0xC0) != 0x80) break;
                }
            }
            return;
        }
        // Handle all printable characters including Vietnamese (UTF-8)
        if (unicode >= 32) {
            std::string* target = nullptr;
            if (activeInputField == 100) target = &bookingDateText;
            else if (activeInputField == 101) target = &bookingTimeText;
            else if (activeInputField == 102) target = &bookingReasonText;
            if (target) {
                if (activeInputField == 100) {
                    // Date input mask DD/MM/YYYY: accept digits only, auto insert '/'
                    if (unicode >= '0' && unicode <= '9') {
                        if (target->size() < 10) {
                            target->push_back(static_cast<char>(unicode));
                            if (target->size() == 2 || target->size() == 5) target->push_back('/');
                        }
                    }
                } else if (activeInputField == 101) {
                    // Time input mask HH:MM: digits only
                    if (unicode >= '0' && unicode <= '9') {
                        if (target->size() < 5) {
                            target->push_back(static_cast<char>(unicode));
                            if (target->size() == 2) target->push_back(':');
                        }
                    }
                } else {
                    // Reason: free text with full UTF-8 support (Vietnamese)
                    if (unicode < 0x10000) {
                        // Convert Unicode codepoint to UTF-8
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
    
    if (unicode == 8) {  // Backspace
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
    
    if (unicode >= 32 && unicode < 127) {  // Printable chars
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

void GUI::render() {
    window.clear(sf::Color(240, 248, 255));  // Light blue background
    
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

// [RENDER FUNCTIONS - PART 1]
void GUI::renderRegisterSelectRole() {
    // Responsive width/centering
    auto ws = window.getSize();
    float W = static_cast<float>(ws.x);
    float H = static_cast<float>(ws.y);

    // Title bar - gradient effect with darker blue
    sf::RectangleShape titleBar({W, 70});
    titleBar.setPosition({0, 0});
    titleBar.setFillColor(sf::Color(30, 136, 229));  // Material Blue
    window.draw(titleBar);
    
    sf::Text appTitle = makeText(font, u8"Đặt LỊCH KHÁM", 32);
    appTitle.setPosition({40, 18});
    appTitle.setFillColor(sf::Color::White);
    appTitle.setStyle(sf::Text::Bold);
    window.draw(appTitle);
    
    // Central panel with shadow effect - centered
    float panelW = 760.f;
    float panelH = 490.f;
    float panelX = W / 2.f - panelW / 2.f;
    float panelY = 150.f;

    sf::RectangleShape shadow({764, 494});
    shadow.setPosition({panelX + 3.f, panelY + 3.f});
    shadow.setFillColor(sf::Color(200, 200, 200, 80));
    window.draw(shadow);
    
    sf::RectangleShape panel({panelW, panelH});
    panel.setPosition({panelX, panelY});
    panel.setFillColor(sf::Color::White);
    window.draw(panel);
    
    // Medical cross icon - centered
    sf::Text logo = makeText(font, "+", 80);
    sf::FloatRect logoBounds = logo.getLocalBounds();
    logo.setPosition({W / 2.f - logoBounds.size.x / 2.f, 170});
    logo.setFillColor(sf::Color(30, 136, 229));
    logo.setStyle(sf::Text::Bold);
    window.draw(logo);
    
    // Title with better spacing
    sf::Text title = makeText(font, u8"Đăng Ký Tài Khoản Mới", 32);
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setPosition({W / 2.f - titleBounds.size.x / 2.f, 270});
    title.setFillColor(sf::Color(33, 33, 33));
    title.setStyle(sf::Text::Bold);
    window.draw(title);
    
    // Subtitle
    sf::Text subtitle = makeText(font, u8"Vui lòng chọn loại tài khoản", 16);
    sf::FloatRect subtitleBounds = subtitle.getLocalBounds();
    subtitle.setPosition({W / 2.f - subtitleBounds.size.x / 2.f, 315});
    subtitle.setFillColor(sf::Color(100, 100, 100));
    window.draw(subtitle);
    
    // Role cards - centered within panel
    float cardsStartX = panelX + (panelW - (2 * 280.f + 20.f)) / 2.f;
    float cardY = 360;
    float cardWidth = 280;
    float cardHeight = 130;
    float cardSpacing = 20;
    
    drawRoleCard({cardsStartX, cardY}, {cardWidth, cardHeight}, u8"Bệnh Nhân", 
                 u8"Đặt lịch khám bệnh", "P", 
                 selectedRole == UserRole::PATIENT, window);
    drawRoleCard({cardsStartX + cardWidth + cardSpacing, cardY}, {cardWidth, cardHeight}, u8"Bác Sĩ",
                 u8"Quản lý lịch khám", "D",
                 selectedRole == UserRole::DOCTOR, window);
    
    // Continue button - centered, wider
    sf::Color buttonColor = (selectedRole != UserRole::NONE) ? 
                           sf::Color(76, 175, 80) : sf::Color(189, 189, 189);
    drawButton({W / 2.f - 150.f, 520}, {300, 55}, u8"Tiếp Tục", buttonColor, sf::Color::White, window);
    
    // Login link - centered
    sf::Text loginLink = makeText(font, u8"Đã có tài khoản? Đăng Nhập", 16);
    sf::FloatRect linkBounds = loginLink.getLocalBounds();
    loginLink.setPosition({W / 2.f - linkBounds.size.x / 2.f, 595});
    loginLink.setFillColor(sf::Color(30, 136, 229));
    window.draw(loginLink);
}

void GUI::renderRegisterInfo() {
    // Responsive width/centering
    auto ws = window.getSize();
    float W = static_cast<float>(ws.x);

    // Title bar
    sf::RectangleShape titleBar({W, 70});
    titleBar.setPosition({0, 0});
    titleBar.setFillColor(sf::Color(30, 136, 229));
    window.draw(titleBar);
    
    std::string roleText = (selectedRole == UserRole::PATIENT) ? u8"Bệnh Nhân" : u8"Bác Sĩ";
    sf::Text appTitle = makeText(font, u8"Đăng Ký - " + roleText, 28);
    appTitle.setPosition({40, 20});
    appTitle.setFillColor(sf::Color::White);
    appTitle.setStyle(sf::Text::Bold);
    window.draw(appTitle);
    
    // Enlarged white panel for better visibility (centered)
    float panelW = 760.f;
    float panelX = W / 2.f - panelW / 2.f;
    sf::RectangleShape panel({panelW, 580});
    panel.setPosition({panelX, 110});
    panel.setFillColor(sf::Color::White);
    window.draw(panel);
    
    sf::Text title = makeText(font, u8"Thông Tin Đăng Ký", 32);
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setPosition({W / 2.f - titleBounds.size.x / 2.f, 145});
    title.setFillColor(sf::Color(33, 33, 33));
    title.setStyle(sf::Text::Bold);
    window.draw(title);
    
    sf::Text subtitle = makeText(font, u8"Vui lòng điền đầy đủ thông tin", 16);
    sf::FloatRect subtitleBounds = subtitle.getLocalBounds();
    subtitle.setPosition({W / 2.f - subtitleBounds.size.x / 2.f, 190});
    subtitle.setFillColor(sf::Color(120, 120, 120));
    window.draw(subtitle);
    
    // Input fields - simplified to 4 fields
    float startY = 265;
    float spacing = 80;
    float baseX = panelX + 80.f;
    
    drawInputField({baseX, startY}, {600, 50}, u8"Tên Đăng Nhập *", inputUsername,
                   activeInputField == 0, false, window);
    drawInputField({baseX, startY + spacing}, {600, 50}, u8"Mật Khẩu *", inputPassword,
                   activeInputField == 1, true, window);
    drawInputField({baseX, startY + spacing * 2}, {600, 50}, u8"Xác Nhận Mật Khẩu *", 
                   inputConfirmPassword, activeInputField == 2, true, window);
    drawInputField({baseX, startY + spacing * 3}, {600, 50}, "Email *", inputEmail,
                   activeInputField == 3, false, window);
    
    // Register button - centered
    drawButton({W / 2.f - 150.f, 600}, {300, 55}, u8"Đăng Ký", 
               sf::Color(76, 175, 80), sf::Color::White, window);
    
    // Back link - moved below white panel
    sf::Text backText = makeText(font, "< Quay Lại", 16);
    sf::FloatRect backBounds = backText.getLocalBounds();
    backText.setPosition({W / 2.f - backBounds.size.x / 2.f, 710});
    backText.setFillColor(sf::Color(30, 136, 229));
    window.draw(backText);
}

// [CONTINUED - PART 2]
void GUI::renderLogin() {
    auto ws = window.getSize();
    float W = static_cast<float>(ws.x);

    sf::RectangleShape titleBar({W, 70});
    titleBar.setPosition({0, 0});
    titleBar.setFillColor(sf::Color(30, 136, 229));
    window.draw(titleBar);
    
    sf::Text appTitle = makeText(font, u8"Đăng Nhập", 28);
    appTitle.setPosition({40, 20});
    appTitle.setFillColor(sf::Color::White);
    appTitle.setStyle(sf::Text::Bold);
    window.draw(appTitle);
    
    // Enlarged white panel for better visibility (centered)
    float panelW = 700.f;
    float panelX = W / 2.f - panelW / 2.f;
    sf::RectangleShape panel({panelW, 450});
    panel.setPosition({panelX, 180});
    panel.setFillColor(sf::Color::White);
    window.draw(panel);
    
    sf::Text title = makeText(font, u8"Chào Mừng Trở Lại", 32);
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setPosition({W / 2.f - titleBounds.size.x / 2.f, 215});
    title.setFillColor(sf::Color(33, 33, 33));
    title.setStyle(sf::Text::Bold);
    window.draw(title);
    
    float baseX = panelX + 110.f;
    drawInputField({baseX, 300}, {480, 50}, u8"Tên Đăng Nhập", inputUsername,
                   activeInputField == 0, false, window);
    drawInputField({baseX, 390}, {480, 50}, u8"Mật Khẩu", inputPassword,
                   activeInputField == 1, true, window);
    
    drawButton({W / 2.f - 100.f, 480}, {200, 55}, u8"Đăng Nhập",
               sf::Color(30, 136, 229), sf::Color::White, window);
    
    sf::Text registerLink = makeText(font, u8"Chưa có tài khoản? Đăng Ký", 16);
    sf::FloatRect linkBounds = registerLink.getLocalBounds();
    registerLink.setPosition({W / 2.f - linkBounds.size.x / 2.f, 570});
    registerLink.setFillColor(sf::Color(30, 136, 229));
    window.draw(registerLink);
}

void GUI::renderUpdatePatientInfo() {
    // Background sized to current window
    auto ws = window.getSize();
    float W = static_cast<float>(ws.x);
    float H = static_cast<float>(ws.y);
    sf::RectangleShape background({W, H});
    background.setPosition({0, 0});
    background.setFillColor(sf::Color(245, 248, 250));
    window.draw(background);
    
    // Title bar spans the width
    sf::RectangleShape titleBar({W, 70});
    titleBar.setPosition({0, 0});
    titleBar.setFillColor(sf::Color(30, 136, 229));
    window.draw(titleBar);
    
    // App title with icon
    sf::Text titleIcon = makeText(font, u8"⚕", 28);
    titleIcon.setPosition({40, 20});
    titleIcon.setFillColor(sf::Color(76, 175, 80));
    window.draw(titleIcon);
    
    sf::Text appTitle = makeText(font, u8"Cập Nhật Thông Tin Bệnh Nhân", 24);
    appTitle.setPosition({80, 23});
    appTitle.setFillColor(sf::Color::White);
    appTitle.setStyle(sf::Text::Bold);
    window.draw(appTitle);
    
    // Wider white panel with more spacing (centered)
    float panelW = 800.f;
    float panelH = 740.f; // Increased to keep save button inside panel
    float panelX = W / 2.f - panelW / 2.f;
    float panelY = 90.f;
    sf::RectangleShape panel({panelW, panelH});
    panel.setPosition({panelX, panelY});
    panel.setFillColor(sf::Color::White);
    window.draw(panel);
    
    // Centered title
    sf::Text title = makeText(font, u8"◈ Thông Tin Cá Nhân", 26);
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setPosition({W / 2.f - titleBounds.size.x / 2.f, 120});
    title.setFillColor(sf::Color(33, 33, 33));
    title.setStyle(sf::Text::Bold);
    window.draw(title);
    
    // Subtitle
    sf::Text subtitle = makeText(font, u8"Vui lòng cập nhật đầy đủ thông tin của bạn", 14);
    sf::FloatRect subtitleBounds = subtitle.getLocalBounds();
    subtitle.setPosition({W / 2.f - subtitleBounds.size.x / 2.f, 155});
    subtitle.setFillColor(sf::Color(120, 120, 120));
    window.draw(subtitle);
    
    // Input fields with better spacing
    float startY = 200;
    float spacing = 75; // Increased from 65 to 75
    float baseX = panelX + 80; // Align inputs nicely inside panel
    
    drawInputField({baseX, startY}, {640, 50}, u8"Họ và tên *", inputFullName,
                   activeInputField == 0, false, window);
    drawInputField({baseX, startY + spacing}, {640, 50}, u8"Số điện thoại *", inputPhone,
                   activeInputField == 1, false, window);
    drawInputField({baseX, startY + spacing * 2}, {640, 50}, "Email *", inputEmail,
                   activeInputField == 2, false, window);
    drawInputField({baseX, startY + spacing * 3}, {640, 50}, u8"Địa chỉ", inputAddress,
                   activeInputField == 3, false, window);
    drawInputField({baseX, startY + spacing * 4}, {640, 50}, u8"Ngày sinh (DD/MM/YYYY)", inputDOB,
                   activeInputField == 4, false, window);
    drawInputField({baseX, startY + spacing * 5}, {640, 50}, u8"Giới tính", inputGender,
                   activeInputField == 5, false, window);
    drawInputField({baseX, startY + spacing * 6}, {640, 50}, u8"Nhóm máu", inputBloodType,
                   activeInputField == 6, false, window);
    drawInputField({baseX, startY + spacing * 7}, {640, 50}, "CCCD", inputCCCD,
                   activeInputField == 7, false, window);
    
    // Determine save state and hover
    bool canSave = !inputFullName.empty() && !inputPhone.empty() && !inputEmail.empty();
    auto mp = sf::Mouse::getPosition(window);
    sf::Vector2f mpos = {static_cast<float>(mp.x), static_cast<float>(mp.y)};
    bool hovered = isMouseOverRect(mpos, {W / 2.f - 150.f, 800}, {300, 50});
    // Centered save button (moved down to avoid overlap with last input)
    drawButton({W / 2.f - 150.f, 800}, {300, 50}, u8"Lưu Thông Tin",
               sf::Color(76, 175, 80), sf::Color::White, canSave, hovered, window);
}

void GUI::renderUpdateDoctorInfo() {
    // Background sized to current window
    auto ws = window.getSize();
    float W = static_cast<float>(ws.x);
    float H = static_cast<float>(ws.y);
    sf::RectangleShape background({W, H});
    background.setPosition({0, 0});
    background.setFillColor(sf::Color(245, 248, 250));
    window.draw(background);
    
    // Title bar spans the width
    sf::RectangleShape titleBar({W, 70});
    titleBar.setPosition({0, 0});
    titleBar.setFillColor(sf::Color(30, 136, 229));
    window.draw(titleBar);
    
    // App title with icon
    sf::Text titleIcon = makeText(font, u8"⚕", 28);
    titleIcon.setPosition({40, 20});
    titleIcon.setFillColor(sf::Color(76, 175, 80));
    window.draw(titleIcon);
    
    sf::Text appTitle = makeText(font, u8"Cập Nhật Thông Tin Bác Sĩ", 24);
    appTitle.setPosition({80, 23});
    appTitle.setFillColor(sf::Color::White);
    appTitle.setStyle(sf::Text::Bold);
    window.draw(appTitle);
    
    // Wider white panel (centered)
    float panelW = 800.f;
    float panelH = 780.f; // Increased to keep save button inside panel
    float panelX = W / 2.f - panelW / 2.f;
    float panelY = 85.f;
    sf::RectangleShape panel({panelW, panelH});
    panel.setPosition({panelX, panelY});
    panel.setFillColor(sf::Color::White);
    window.draw(panel);
    
    // Centered title
    sf::Text title = makeText(font, u8"◈ Thông Tin Bác Sĩ", 26);
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setPosition({W / 2.f - titleBounds.size.x / 2.f, 110});
    title.setFillColor(sf::Color(33, 33, 33));
    title.setStyle(sf::Text::Bold);
    window.draw(title);
    
    // Subtitle
    sf::Text subtitle = makeText(font, u8"Cập nhật thông tin hành nghề và chuyên môn", 14);
    sf::FloatRect subtitleBounds = subtitle.getLocalBounds();
    subtitle.setPosition({W / 2.f - subtitleBounds.size.x / 2.f, 145});
    subtitle.setFillColor(sf::Color(120, 120, 120));
    window.draw(subtitle);
    
    // Input fields with better spacing
    float startY = 185;
    float spacing = 70; // Increased from 60 to 70
    float baseX = panelX + 80; // Align inputs inside centered panel
    
    drawInputField({baseX, startY}, {640, 50}, u8"Họ và tên *", inputFullName,
                   activeInputField == 0, false, window);
    drawInputField({baseX, startY + spacing}, {640, 50}, u8"Số điện thoại *", inputPhone,
                   activeInputField == 1, false, window);
    drawInputField({baseX, startY + spacing * 2}, {640, 50}, "Email *", inputEmail,
                   activeInputField == 2, false, window);
    drawInputField({baseX, startY + spacing * 3}, {640, 50}, u8"Địa chỉ", inputAddress,
                   activeInputField == 3, false, window);
    drawInputField({baseX, startY + spacing * 4}, {640, 50}, u8"Ngày sinh (DD/MM/YYYY)", inputDOB,
                   activeInputField == 4, false, window);
    drawInputField({baseX, startY + spacing * 5}, {640, 50}, u8"Giới tính", inputGender,
                   activeInputField == 5, false, window);
    drawInputField({baseX, startY + spacing * 6}, {640, 50}, "CCCD", inputCCCD,
                   activeInputField == 6, false, window);
    drawInputField({baseX, startY + spacing * 7}, {640, 50}, u8"Chuyên môn *", inputSpecialization,
                   activeInputField == 7, false, window);
    drawInputField({baseX, startY + spacing * 8}, {640, 50}, "Khoa *", inputFaculty,
                   activeInputField == 8, false, window);
    
    // Determine save state and hover
    bool canSave = !inputFullName.empty() && !inputPhone.empty() && !inputEmail.empty()
                   && !inputSpecialization.empty() && !inputFaculty.empty();
    auto mp = sf::Mouse::getPosition(window);
    sf::Vector2f mpos = {static_cast<float>(mp.x), static_cast<float>(mp.y)};
    bool hovered = isMouseOverRect(mpos, {W / 2.f - 150.f, 820}, {300, 50});
    // Centered save button (moved down to avoid overlap with last input)
    drawButton({W / 2.f - 150.f, 820}, {300, 50}, u8"Lưu Thông Tin",
               sf::Color(76, 175, 80), sf::Color::White, canSave, hovered, window);
}

void GUI::renderPatientDashboard() {
    // Subtle background pattern/gradient (responsive to window size)
    auto ws = window.getSize();
    float W = static_cast<float>(ws.x);
    float H = static_cast<float>(ws.y);
    sf::RectangleShape background({W, H});
    background.setPosition({0, 0});
    background.setFillColor(sf::Color(245, 248, 250));
    window.draw(background);
    
    // Header bar - dark blue (spans width)
    sf::RectangleShape headerBar({W, 70});
    headerBar.setPosition({0, 0});
    headerBar.setFillColor(sf::Color(25, 35, 50));
    window.draw(headerBar);
    
    // MediCare logo icon (medical cross)
    sf::Text logoIcon = makeText(font, u8"⚕", 32);
    logoIcon.setPosition({40, 18});
    logoIcon.setFillColor(sf::Color(76, 175, 80));
    window.draw(logoIcon);
    
    // MediCare title
    sf::Text appTitle = makeText(font, "MediCare", 26);
    appTitle.setPosition({85, 21});
    appTitle.setFillColor(sf::Color::White);
    appTitle.setStyle(sf::Text::Bold);
    window.draw(appTitle);
    
    // User info in top-right with avatar using symbol font (aligned to right edge)
    float rightPad = 40.f;
    sf::Text userAvatar = makeSymbolText(symbolFont, u8"◉", 26);
    sf::FloatRect avatarBounds = userAvatar.getLocalBounds();
    userAvatar.setPosition({W - rightPad - avatarBounds.size.x, 17});
    userAvatar.setFillColor(sf::Color(76, 175, 80));
    window.draw(userAvatar);
    
    // User name with better spacing
    sf::Text userName = makeText(font, u8"Xin chào, " + inputUsername, 14);
    sf::FloatRect userBounds = userName.getLocalBounds();
    userName.setPosition({W - rightPad - 35.f - userBounds.size.x, 20});
    userName.setFillColor(sf::Color::White);
    window.draw(userName);
    
    // Logout link with icon using symbol font
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
    
    // Main white panel with rounded effect (centered horizontally)
    float mainW = 1180.f;
    float mainH = 680.f;
    float mainX = W / 2.f - mainW / 2.f;
    float mainY = 100.f;
    sf::RectangleShape mainPanel({mainW, mainH});
    mainPanel.setPosition({mainX, mainY});
    mainPanel.setFillColor(sf::Color::White);
    window.draw(mainPanel);
    
    // Left navigation menu
    drawNavigationMenu({mainX, mainY}, {250, 680}, window);
    
    // Right content area
    sf::Vector2f contentPos = {mainX + 270.f, 120};
    
    // Title for current section
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
    
    // Content based on active menu
    if (activePatientMenu == PatientMenuOption::BOOK_APPOINTMENT) {
        // Search and filter section (reflow to avoid overlap)
        float searchY = contentPos.y + 70;

        // Label above input
        sf::Text searchLabel = makeText(font, u8"Tìm kiếm bác sĩ:", 16);
        searchLabel.setPosition({contentPos.x, searchY - 28});
        searchLabel.setFillColor(sf::Color(60, 70, 90));
        searchLabel.setStyle(sf::Text::Bold);
        window.draw(searchLabel);

        // Search input
        drawInputField({contentPos.x, searchY}, {380, 48}, u8"Nhập tên bác sĩ...",
                       searchDoctorText, false, false, window);

        // Search button inline on the right of input to prevent vertical crowding
        drawButton({contentPos.x + 390, searchY}, {140, 48}, u8"Tìm kiếm",
                   sf::Color(76, 175, 80), sf::Color::White, window);

        // Filter section label
        sf::Text filterLabel = makeText(font, u8"Bộ lọc tìm kiếm:", 16);
        filterLabel.setPosition({contentPos.x, searchY + 70});
        filterLabel.setFillColor(sf::Color(60, 70, 90));
        filterLabel.setStyle(sf::Text::Bold);
        window.draw(filterLabel);

        // Specialty and Date buttons in one row
        drawButton({contentPos.x, searchY + 100}, {185, 48}, u8"⚕ Chuyên khoa",
                   sf::Color(30, 136, 229), sf::Color::White, window);

    drawButton({contentPos.x + 195, searchY + 100}, {185, 48}, u8"Chọn ngày",
           sf::Color(30, 136, 229), sf::Color::White, window);

    // Doctor list (show up to 6, filtered by search text)
        float listStartY = searchY + 170;
        auto allDoctorIds = DataStore::listIDs("Doctor");
        std::vector<std::string> filtered;
        // simple lowercase contains
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
                drawDoctorListItem({contentPos.x, listStartY + i * 90.f}, {530, 80},
                                   name, spec, did, window);
            }
        } else {
            sf::Text empty = makeText(font, u8"Không tìm thấy bác sĩ phù hợp.", 14);
            empty.setPosition({contentPos.x, listStartY});
            empty.setFillColor(sf::Color(120, 140, 160));
            window.draw(empty);
        }

        // Date picker modal overlay
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

            // Generate next 14 days
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

        // Booking modal overlay
        if (showBookingModal) {
            auto ws2 = window.getSize();
            float W2 = static_cast<float>(ws2.x);
            float H2 = static_cast<float>(ws2.y);
            // dim background
            sf::RectangleShape overlay({W2, H2});
            overlay.setPosition({0,0});
            overlay.setFillColor(sf::Color(0,0,0,120));
            window.draw(overlay);

            // modal panel centered and taller for bigger reason field
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

            // Centered input fields with equal spacing
            float fieldWidth = 540.f;
            float fx = mx + (mw - fieldWidth) / 2.f;
            float startY = my + 80;
            float spacing = 85.f; // equal spacing between fields
            
            drawInputField({fx, startY}, {fieldWidth, 48}, u8"Ngày (DD/MM/YYYY)", bookingDateText, activeInputField == 100, false, window);
            drawInputField({fx, startY + spacing}, {fieldWidth, 48}, u8"Giờ (HH:MM)", bookingTimeText, activeInputField == 101, false, window);
            // Larger reason field for longer text
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
            // Centered buttons at bottom with equal spacing
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
        
        // Upcoming appointments section on the right with better spacing
    float appointmentX = contentPos.x + 520; // keep within window width while giving room for doctor list
        
        // Section title with icon
        sf::Text calendarIcon = makeSymbolText(symbolFont, u8"📅", 20);
        calendarIcon.setPosition({appointmentX, searchY + 68});
        calendarIcon.setFillColor(sf::Color(30, 136, 229));
        window.draw(calendarIcon);
        
        sf::Text upcomingTitle = makeText(font, u8"Lịch khám sắp tới", 18);
        upcomingTitle.setPosition({appointmentX + 30, searchY + 70});
        upcomingTitle.setFillColor(sf::Color(25, 35, 50));
        upcomingTitle.setStyle(sf::Text::Bold);
        window.draw(upcomingTitle);
        
        // Divider line
        sf::RectangleShape divider({460, 2});
        divider.setPosition({appointmentX, searchY + 98});
        divider.setFillColor(sf::Color(220, 230, 240));
        window.draw(divider);
        
        // Load real upcoming appointments for current patient (exclude Cancelled)
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
                drawAppointmentCard({appointmentX, y}, {460, 105}, dname, spec, dt, window);
            }
        } else {
            // No appointments message with better spacing
            sf::RectangleShape emptyBox({460, 220});
            emptyBox.setPosition({appointmentX, searchY + 115});
            emptyBox.setFillColor(sf::Color(250, 252, 255));
            emptyBox.setOutlineThickness(2);
            emptyBox.setOutlineColor(sf::Color(220, 230, 240));
            window.draw(emptyBox);
            
            // Empty state icon using symbol font
            sf::Text emptyIcon = makeSymbolText(symbolFont, u8"📭", 52);
            sf::FloatRect iconBounds = emptyIcon.getLocalBounds();
            emptyIcon.setPosition({appointmentX + 230 - iconBounds.size.x / 2, searchY + 150});
            emptyIcon.setFillColor(sf::Color(150, 170, 190));
            window.draw(emptyIcon);
            
            // Empty state text with better spacing
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
    else if (activePatientMenu == PatientMenuOption::UPDATE_INFO) {
        // Redirect to update info screen
        drawButton({contentPos.x, contentPos.y + 80}, {350, 50}, 
                  u8"Cập nhật thông tin cá nhân",
                  sf::Color(30, 136, 229), sf::Color::White, window);
    }
}

void GUI::renderDoctorDashboard() {
    sf::RectangleShape titleBar({1280, 70});
    titleBar.setPosition({0, 0});
    titleBar.setFillColor(sf::Color(30, 136, 229));
    window.draw(titleBar);
    
    sf::Text appTitle = makeText(font, "Dashboard - Bác Sĩ", 28);
    appTitle.setPosition({40, 20});
    appTitle.setFillColor(sf::Color::White);
    appTitle.setStyle(sf::Text::Bold);
    window.draw(appTitle);
    
    sf::Text welcomeText = makeText(font, u8"Chào Mừng Bác Sĩ, " + inputUsername + "!", 32);
    welcomeText.setPosition({350, 200});
    welcomeText.setFillColor(sf::Color(33, 33, 33));
    welcomeText.setStyle(sf::Text::Bold);
    window.draw(welcomeText);
    
    sf::Text idText = makeText(font, "ID: " + currentUserId, 20);
    idText.setPosition({400, 250});
    idText.setFillColor(sf::Color(100, 100, 100));
    window.draw(idText);
    
    drawButton({440, 350}, {400, 55}, u8"Cập Nhật Thông Tin Cá Nhân",
               sf::Color(30, 136, 229), sf::Color::White, window);
}

// Helper drawing functions
void GUI::drawRoundedRect(sf::Vector2f position, sf::Vector2f size, float radius,
                          sf::Color fillColor, sf::RenderWindow& win) {
    sf::RectangleShape rect(size);
    rect.setPosition(position);
    rect.setFillColor(fillColor);
    win.draw(rect);
}

void GUI::drawButton(sf::Vector2f position, sf::Vector2f size, const std::string& text,
                    sf::Color bgColor, sf::Color textColor, sf::RenderWindow& win) {
    sf::RectangleShape button(size);
    button.setPosition(position);
    button.setFillColor(bgColor);
    win.draw(button);
    
    sf::Text buttonText = makeText(font, text, 20);
    buttonText.setFillColor(textColor);
    buttonText.setStyle(sf::Text::Bold);
    
    sf::FloatRect textBounds = buttonText.getLocalBounds();
    buttonText.setOrigin({textBounds.size.x / 2.0f, textBounds.size.y / 2.0f});
    buttonText.setPosition({position.x + size.x / 2.0f, position.y + size.y / 2.0f});
    win.draw(buttonText);
}

// Overload: Button with enabled/hovered states
void GUI::drawButton(sf::Vector2f position, sf::Vector2f size, const std::string& text,
                    sf::Color baseBg, sf::Color baseText,
                    bool enabled, bool hovered, sf::RenderWindow& win) {
    sf::Color bg = baseBg;
    sf::Color fg = baseText;
    if (!enabled) {
        // Desaturate and lower opacity when disabled
        bg = sf::Color(189, 189, 189);
        fg = sf::Color(255, 255, 255, 200);
    } else if (hovered) {
        // Slightly darken on hover
        int r = std::max(0, baseBg.r - 12);
        int g = std::max(0, baseBg.g - 12);
        int b = std::max(0, baseBg.b - 12);
        bg = sf::Color(r, g, b);
    }
    sf::RectangleShape button(size);
    button.setPosition(position);
    button.setFillColor(bg);
    win.draw(button);

    sf::Text buttonText = makeText(font, text, 20);
    buttonText.setFillColor(fg);
    buttonText.setStyle(sf::Text::Bold);
    sf::FloatRect textBounds = buttonText.getLocalBounds();
    buttonText.setOrigin({textBounds.size.x / 2.0f, textBounds.size.y / 2.0f});
    buttonText.setPosition({position.x + size.x / 2.0f, position.y + size.y / 2.0f});
    win.draw(buttonText);
}

bool GUI::isMouseOverRect(const sf::Vector2f& mousePos, const sf::Vector2f& rectPos,
                          const sf::Vector2f& rectSize) {
    return mousePos.x >= rectPos.x && mousePos.x <= rectPos.x + rectSize.x &&
           mousePos.y >= rectPos.y && mousePos.y <= rectPos.y + rectSize.y;
}

void GUI::drawInputField(sf::Vector2f position, sf::Vector2f size, const std::string& label,
                        const std::string& value, bool isActive, bool isPassword,
                        sf::RenderWindow& win) {
    // Label with more spacing above the input box
    sf::Text labelText = makeText(font, label, 14);
    labelText.setPosition({position.x, position.y - 30}); // Increased from 25 to 30
    labelText.setFillColor(sf::Color(80, 80, 80));
    labelText.setStyle(sf::Text::Bold); // Make label bold for clarity
    win.draw(labelText);
    
    // Input box with subtle shadow effect
    sf::RectangleShape inputBox(size);
    inputBox.setPosition(position);
    inputBox.setFillColor(sf::Color(255, 255, 255)); // Pure white for input
    inputBox.setOutlineThickness(2);
    inputBox.setOutlineColor(isActive ? sf::Color(30, 136, 229) : sf::Color(220, 220, 220));
    win.draw(inputBox);
    
    std::string displayValue = value;
    if (isPassword && !value.empty()) {
        displayValue = std::string(value.length(), '*'); // Use asterisks for password
    }
    
    // Value text with better padding
    sf::Text valueText = makeText(font, displayValue, 16);
    valueText.setPosition({position.x + 18, position.y + 15}); // Slightly more left padding
    valueText.setFillColor(sf::Color(33, 33, 33)); // Darker for better readability
    win.draw(valueText);
    
    // Blinking cursor
    if (isActive && showCursor) {
        sf::FloatRect bounds = valueText.getLocalBounds();
        sf::RectangleShape cursor({2, 28});
        cursor.setPosition({position.x + 18 + bounds.size.x + 3, position.y + 11});
        cursor.setFillColor(sf::Color(30, 136, 229)); // Blue cursor
        win.draw(cursor);
    }
}

void GUI::drawRoleCard(sf::Vector2f position, sf::Vector2f size, const std::string& title,
                      const std::string& description, const std::string& icon,
                      bool isSelected, sf::RenderWindow& win) {
    sf::RectangleShape card(size);
    card.setPosition(position);
    card.setFillColor(sf::Color::White);
    card.setOutlineThickness(3);
    card.setOutlineColor(isSelected ? sf::Color(30, 136, 229) : sf::Color(230, 230, 230));
    win.draw(card);
    
    sf::Text iconText = makeText(font, icon, 48);
    iconText.setPosition({position.x + 25, position.y + 15});
    iconText.setFillColor(sf::Color(30, 136, 229));
    iconText.setStyle(sf::Text::Bold);
    win.draw(iconText);
    
    sf::CircleShape radioOuter(12);
    radioOuter.setPosition({position.x + size.x - 40, position.y + 20});
    radioOuter.setFillColor(sf::Color::White);
    radioOuter.setOutlineThickness(2);
    radioOuter.setOutlineColor(isSelected ? sf::Color(30, 136, 229) : sf::Color(180, 180, 180));
    win.draw(radioOuter);
    
    if (isSelected) {
        sf::CircleShape radioInner(7);
        radioInner.setPosition({position.x + size.x - 35, position.y + 25});
        radioInner.setFillColor(sf::Color(30, 136, 229));
        win.draw(radioInner);
    }
    
    sf::Text titleText = makeText(font, title, 22);
    titleText.setPosition({position.x + 25, position.y + 70});
    titleText.setFillColor(sf::Color(33, 33, 33));
    titleText.setStyle(sf::Text::Bold);
    win.draw(titleText);
    
    sf::Text descText = makeText(font, description, 14);
    descText.setPosition({position.x + 25, position.y + 100});
    descText.setFillColor(sf::Color(120, 120, 120));
    win.draw(descText);
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

void GUI::completeRegistration() {
    // Validate empty fields
    if (inputUsername.empty() || inputPassword.empty() || inputEmail.empty()) {
        std::cout << u8"❌ Lỗi: Vui lòng điền đầy đủ thông tin!" << std::endl;
        return;
    }
    
    // Validate password match
    if (inputPassword != inputConfirmPassword) {
        std::cout << u8"❌ Lỗi: Mật khẩu không khớp!" << std::endl;
        return;
    }
    
    // Validate username length
    if (inputUsername.length() < 3) {
        std::cout << u8"❌ Lỗi: Tên đăng nhập phải có ít nhất 3 ký tự!" << std::endl;
        return;
    }
    
    // Validate password length
    if (inputPassword.length() < 6) {
        std::cout << u8"❌ Lỗi: Mật khẩu phải có ít nhất 6 ký tự!" << std::endl;
        return;
    }
    
    std::cout << u8"⏳ Đang đăng ký..." << std::endl;
    
    std::string role = (selectedRole == UserRole::PATIENT) ? "Patient" : "Doctor";
    std::string generatedId;
    
    bool success = accountSystem.registerWithCredentials(inputUsername, inputPassword, role, generatedId);
    
    if (success) {
        currentUserId = generatedId;
        currentUserRole = role;
        
        // Save email to account file
        std::ofstream emailFile("data/" + role + "/" + generatedId + "_email.txt");
        if (emailFile.is_open()) {
            emailFile << inputEmail;
            emailFile.close();
        }
        
        std::cout << u8"✅ Đăng ký thành công! ID: " << generatedId << std::endl;
        
        // Clear password fields for security
        inputPassword.clear();
        inputConfirmPassword.clear();
        
        // After registration, go to update info screen
        if (selectedRole == UserRole::PATIENT) {
            switchToScreen(Screen::UPDATE_PATIENT_INFO);
        } else {
            switchToScreen(Screen::UPDATE_DOCTOR_INFO);
        }
    } else {
        std::cout << u8"❌ Đăng ký thất bại! Tên đăng nhập đã tồn tại." << std::endl;
    }
}

bool GUI::validateRegistrationInputs() {
    return !inputUsername.empty() && !inputPassword.empty() && 
           !inputEmail.empty() && (inputPassword == inputConfirmPassword);
}

void GUI::completeLogin() {
    // Validate empty fields
    if (inputUsername.empty() || inputPassword.empty()) {
        std::cout << u8"❌ Lỗi: Vui lòng nhập tên đăng nhập và mật khẩu!" << std::endl;
        return;
    }
    
    std::cout << u8"⏳ Đang đăng nhập..." << std::endl;
    
    std::string outRole, outId;
    bool success = accountSystem.loginWithCredentials(inputUsername, inputPassword, outRole, outId);
    
    if (success) {
        currentUserId = outId;
        currentUserRole = outRole;
        
        std::cout << u8"✅ Đăng nhập thành công! Role: " << outRole << ", ID: " << outId << std::endl;
        
        // Clear password for security
        inputPassword.clear();
        
        if (outRole == "Patient") {
            switchToScreen(Screen::PATIENT_DASHBOARD);
        } else if (outRole == "Doctor") {
            switchToScreen(Screen::DOCTOR_DASHBOARD);
        } else {
            switchToScreen(Screen::PATIENT_DASHBOARD);
        }
    } else {
        std::cout << u8"Đăng nhập thất bại!" << std::endl;
    }
}

void GUI::loadPatientInfo() {
    DataStore::PatientInfo info = DataStore::readPatientInfo(currentUserId);
    inputFullName = info.name;
    inputPhone = info.phone;
    inputEmail = info.email;
    inputAddress = info.address;
    inputDOB = info.dateOfBirth;
    inputGender = info.gender;
}

void GUI::savePatientInfo() {
    DataStore::PatientInfo info;
    info.name = inputFullName;
    info.phone = inputPhone;
    info.email = inputEmail;
    info.address = inputAddress;
    info.dateOfBirth = inputDOB;
    info.gender = inputGender;
    
    if (DataStore::writePatientInfo(currentUserId, info)) {
        std::cout << u8"Lưu thông tin thành công!" << std::endl;
    } else {
        std::cout << u8"Lưu thông tin thất bại!" << std::endl;
    }
}

void GUI::loadDoctorInfo() {
    DataStore::DoctorInfo info = DataStore::readDoctorInfo(currentUserId);
    inputFullName = info.name;
    inputPhone = info.phone;
    inputEmail = info.email;
    inputSpecialization = info.specialization;
    inputFaculty = info.experience;
}

void GUI::saveDoctorInfo() {
    DataStore::DoctorInfo info;
    info.name = inputFullName;
    info.phone = inputPhone;
    info.email = inputEmail;
    info.specialization = inputSpecialization;
    info.experience = inputFaculty;
    
    if (DataStore::writeDoctorInfo(currentUserId, info)) {
        std::cout << u8"Lưu thông tin thành công!" << std::endl;
    } else {
        std::cout << u8"Lưu thông tin thất bại!" << std::endl;
    }
}

void GUI::drawNavigationMenu(sf::Vector2f position, sf::Vector2f size, sf::RenderWindow& win) {
    // Navigation background with border
    sf::RectangleShape navBg(size);
    navBg.setPosition(position);
    navBg.setFillColor(sf::Color(248, 250, 252));
    win.draw(navBg);
    
    // Right border
    sf::RectangleShape border({2, size.y});
    border.setPosition({position.x + size.x, position.y});
    border.setFillColor(sf::Color(220, 230, 240));
    win.draw(border);
    
    float menuItemHeight = 62;
    float startY = position.y + 45;
    
    // Menu items with icons
    struct MenuItem {
        PatientMenuOption option;
        std::string icon;
        std::string text;
    };
    
    // Using Unicode symbols with symbol font for better rendering
    MenuItem items[] = {
        {PatientMenuOption::BOOK_APPOINTMENT, u8"⊕", u8"Đặt lịch khám"},
        {PatientMenuOption::MY_APPOINTMENTS, u8"☰", u8"Lịch khám của tôi"},
        {PatientMenuOption::NOTIFICATIONS, u8"◉", u8"Thông báo"},
        {PatientMenuOption::UPDATE_INFO, u8"◈", u8"Cập nhật thông tin"}
    };
    
    for (int i = 0; i < 4; i++) {
        bool isActive = (items[i].option == activePatientMenu);
        float itemY = startY + i * (menuItemHeight + 12);
        
        // Highlight active item with rounded corners effect
        if (isActive) {
            sf::RectangleShape highlight({size.x - 20, menuItemHeight});
            highlight.setPosition({position.x + 10, itemY});
            highlight.setFillColor(sf::Color(30, 136, 229));
            win.draw(highlight);
        }
        
        // Icon using symbol font for better rendering
        sf::Text icon = makeSymbolText(symbolFont, items[i].icon, 30);
        icon.setPosition({position.x + 22, itemY + 14});
        icon.setFillColor(isActive ? sf::Color::White : sf::Color(80, 100, 120));
        win.draw(icon);
        
        // Text with better spacing
        sf::Text text = makeText(font, items[i].text, 14);
        text.setPosition({position.x + 65, itemY + 22});
        text.setFillColor(isActive ? sf::Color::White : sf::Color(50, 60, 80));
        if (isActive) {
            text.setStyle(sf::Text::Bold);
        }
        win.draw(text);
    }
}

void GUI::drawAppointmentCard(sf::Vector2f position, sf::Vector2f size,
                              const std::string& doctorName, const std::string& specialty,
                              const std::string& dateTime, sf::RenderWindow& win) {
    // Card background with shadow effect
    sf::RectangleShape shadow(size);
    shadow.setPosition({position.x + 2, position.y + 2});
    shadow.setFillColor(sf::Color(220, 230, 240, 80));
    win.draw(shadow);
    
    sf::RectangleShape card(size);
    card.setPosition(position);
    card.setFillColor(sf::Color::White);
    card.setOutlineThickness(1);
    card.setOutlineColor(sf::Color(220, 230, 240));
    win.draw(card);
    
    // Doctor icon/avatar using symbol font with better spacing
    sf::Text avatar = makeSymbolText(symbolFont, u8"⚕", 34);
    avatar.setPosition({position.x + 18, position.y + 15});
    avatar.setFillColor(sf::Color(30, 136, 229));
    win.draw(avatar);
    
    // Doctor name with better vertical alignment
    sf::Text doctor = makeText(font, doctorName, 16);
    doctor.setPosition({position.x + 65, position.y + 18});
    doctor.setFillColor(sf::Color(25, 35, 50));
    doctor.setStyle(sf::Text::Bold);
    win.draw(doctor);
    
    // Specialty badge with background
    sf::RectangleShape specBadge({110, 24});
    specBadge.setPosition({position.x + 65, position.y + 48});
    specBadge.setFillColor(sf::Color(230, 245, 255));
    win.draw(specBadge);
    
    sf::Text specIcon = makeSymbolText(symbolFont, u8"●", 10);
    specIcon.setPosition({position.x + 70, position.y + 54});
    specIcon.setFillColor(sf::Color(30, 136, 229));
    win.draw(specIcon);
    
    sf::Text spec = makeText(font, specialty, 12);
    spec.setPosition({position.x + 83, position.y + 52});
    spec.setFillColor(sf::Color(30, 136, 229));
    win.draw(spec);
    
    // Date and time with clock icon and better spacing
    sf::Text clockIcon = makeSymbolText(symbolFont, u8"◷", 18);
    clockIcon.setPosition({position.x + 18, position.y + size.y - 32});
    clockIcon.setFillColor(sf::Color(100, 120, 140));
    win.draw(clockIcon);
    
    sf::Text date = makeText(font, dateTime, 14);
    date.setPosition({position.x + 45, position.y + size.y - 29});
    date.setFillColor(sf::Color(80, 100, 120));
    win.draw(date);
    
    // Cancel button with better sizing
    sf::RectangleShape cancelBtn({105, 32});
    cancelBtn.setPosition({position.x + size.x - 120, position.y + size.y - 42});
    cancelBtn.setFillColor(sf::Color(244, 67, 54));
    win.draw(cancelBtn);
    
    sf::Text cancelText = makeText(font, u8"✕ Hủy", 13);
    sf::FloatRect cancelBounds = cancelText.getLocalBounds();
    cancelText.setPosition({position.x + size.x - 67 - cancelBounds.size.x / 2, 
                           position.y + size.y - 34});
    cancelText.setFillColor(sf::Color::White);
    win.draw(cancelText);
    
    // Arrow indicator with symbol font
    sf::Text arrow = makeSymbolText(symbolFont, u8"›", 28);
    arrow.setPosition({position.x + size.x - 35, position.y + 38});
    arrow.setFillColor(sf::Color(150, 170, 190));
    win.draw(arrow);
}

void GUI::drawDoctorListItem(sf::Vector2f position, sf::Vector2f size,
                             const std::string& doctorName, const std::string& specialty,
                             const std::string& doctorId, sf::RenderWindow& win) {
    // Card background
    sf::RectangleShape card(size);
    card.setPosition(position);
    card.setFillColor(sf::Color::White);
    card.setOutlineThickness(1);
    card.setOutlineColor(sf::Color(220, 230, 240));
    win.draw(card);

    // Avatar/icon
    sf::Text avatar = makeSymbolText(symbolFont, u8"⚕", 28);
    avatar.setPosition({position.x + 14, position.y + 12});
    avatar.setFillColor(sf::Color(30, 136, 229));
    win.draw(avatar);

    // Name
    sf::Text name = makeText(font, doctorName, 16);
    name.setPosition({position.x + 54, position.y + 10});
    name.setFillColor(sf::Color(25, 35, 50));
    name.setStyle(sf::Text::Bold);
    win.draw(name);

    // Specialty (badge style)
    sf::RectangleShape specBadge({150, 22});
    specBadge.setPosition({position.x + 54, position.y + 40});
    specBadge.setFillColor(sf::Color(230, 245, 255));
    win.draw(specBadge);

    sf::Text spec = makeText(font, specialty, 12);
    spec.setPosition({position.x + 64, position.y + 42});
    spec.setFillColor(sf::Color(30, 136, 229));
    win.draw(spec);

    // ID subtle on right
    sf::Text idText = makeText(font, "#" + doctorId, 12);
    sf::FloatRect idBounds = idText.getLocalBounds();
    idText.setPosition({position.x + size.x - 12 - idBounds.size.x, position.y + 10});
    idText.setFillColor(sf::Color(150, 160, 170));
    win.draw(idText);

    // Book button (visual only)
    drawButton({position.x + size.x - 120, position.y + size.y - 40}, {110, 32}, u8"Đặt lịch",
               sf::Color(76, 175, 80), sf::Color::White, win);
}



