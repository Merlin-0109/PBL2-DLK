#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <ctime>

// Common internal helpers used across GUI implementation files
sf::Text makeText(const sf::Font& font, const std::string& str, unsigned int size);
sf::Text makeSymbolText(const sf::Font& symbolFont, const std::string& str, unsigned int size);
sf::String makeUtf8String(const char* u8str);

bool parseDateDDMMYYYY(const std::string& s, int& d, int& m, int& y);
bool parseTimeHHMM(const std::string& s, int& hh, int& mm);
bool toTimeT(const std::string& date, const std::string& time, std::time_t& out);
