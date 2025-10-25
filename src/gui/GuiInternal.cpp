#include "gui/GuiInternal.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

sf::Text makeText(const sf::Font& font, const std::string& str, unsigned int size) {
    sf::Text text(font);
    text.setString(sf::String::fromUtf8(str.begin(), str.end()));
    text.setCharacterSize(size);
    return text;
}

sf::Text makeSymbolText(const sf::Font& symbolFont, const std::string& str, unsigned int size) {
    sf::Text text(symbolFont);
    text.setString(sf::String::fromUtf8(str.begin(), str.end()));
    text.setCharacterSize(size);
    return text;
}

sf::String makeUtf8String(const char* u8str) {
    std::string str(u8str);
    return sf::String::fromUtf8(str.begin(), str.end());
}

bool parseDateDDMMYYYY(const std::string& s, int& d, int& m, int& y) {
    if (s.size() != 10 || s[2] != '/' || s[5] != '/') return false;
    try {
        d = std::stoi(s.substr(0,2));
        m = std::stoi(s.substr(3,2));
        y = std::stoi(s.substr(6,4));
    } catch (...) { return false; }
    if (y < 1900 || y > 2100 || m < 1 || m > 12 || d < 1 || d > 31) return false;
    static const int mdays[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
    int maxd = mdays[m-1];
    if (m == 2) {
        bool leap = (y%4==0 && (y%100!=0 || y%400==0));
        if (leap) maxd = 29;
    }
    return d <= maxd;
}

bool parseTimeHHMM(const std::string& s, int& hh, int& mm) {
    if (s.size() != 5 || s[2] != ':') return false;
    try {
        hh = std::stoi(s.substr(0,2));
        mm = std::stoi(s.substr(3,2));
    } catch (...) { return false; }
    return hh >= 0 && hh <= 23 && mm >= 0 && mm <= 59;
}

bool toTimeT(const std::string& date, const std::string& time, std::time_t& out) {
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
