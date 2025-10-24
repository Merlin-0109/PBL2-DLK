// Account.h - GUI-only version (no console input)
#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <filesystem>

namespace fs = std::filesystem;

class Account {
private:
    std::string role;
    std::string id;
    std::string username;
    std::string password;

    std::string generateID(const std::string& role);
    void saveUserData(const std::string& role, const std::string& id);
    void appendIDToList(const std::string& role, const std::string& id);

public:
    Account();
    ~Account();

    // DEPRECATED: Console-based methods (disabled for GUI-only app)
    bool login(std::string& outRole, std::string& outId);
    void logout();
    void registerAccount();
    
    // GUI-compatible methods (USE THESE!)
    bool loginWithCredentials(const std::string& inputUser, const std::string& inputPass, 
                             std::string& outRole, std::string& outId);
    bool registerWithCredentials(const std::string& inputUser, const std::string& inputPass,
                                const std::string& inputRole, std::string& outId);

    static void ensureDefaultAdminExists();

    Account& operator=(const Account& other);

    std::string getUsername() const;
    std::string getID() const;
};