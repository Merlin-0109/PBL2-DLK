#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <iomanip>

class User;

namespace fs = std::filesystem;

class Account {
private:
    std::string role;   // "Admin", "Doctor", "Patient"
    std::string id;     // Ví dụ: 01001, 020101, 001
    std::string username;
    std::string password;

public:
    Account();
    ~Account();

    // Prompts for username/password, returns true on success and fills outRole/outId
    bool login(std::string& outRole, std::string& outId);
    void logout();
    void registerAccount();

    Account& operator =(const Account& other);

    // simple accessors for logged-in session
    std::string getUsername() const;
    std::string getID() const;

    // Ensure a default Admin account exists (creates data/Admin if missing)
    static void ensureDefaultAdminExists();

private:
    std::string generateID(const std::string& role);
    void saveUserData(const std::string& role, const std::string& id);
    void appendIDToList(const std::string& role, const std::string& id);
};
