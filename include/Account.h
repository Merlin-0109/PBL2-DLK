// Account.h - System authentication and account management
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

    // GUI-compatible authentication methods
    bool loginWithCredentials(const std::string& inputUser, const std::string& inputPass, 
                             std::string& outRole, std::string& outId);
    bool registerWithCredentials(const std::string& inputUser, const std::string& inputPass,
                                const std::string& inputRole, std::string& outId);
    
    // Account management
    void logout();
    bool changePassword(const std::string& oldPass, const std::string& newPass);
    bool usernameExists(const std::string& username) const;
    
    // Static utilities
    static void ensureDefaultAdminExists();
    static bool validateUsername(const std::string& username);
    static bool validatePassword(const std::string& password);

    // Operators
    Account& operator=(const Account& other);

    // Getters
    std::string getUsername() const;
    std::string getID() const;
    std::string getRole() const;
};