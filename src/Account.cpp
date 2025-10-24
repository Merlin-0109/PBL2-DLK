// Account.cpp - Extended with GUI support
#include "Account.h"
#include "User.h"

Account::Account() {}
Account::~Account() {}

// Create a default admin account if none exists
void Account::ensureDefaultAdminExists() {
    fs::create_directories("data/Admin");
    std::ifstream list("data/Admin.txt");
    bool has = false;
    std::string id;
    while (list >> id) { has = true; break; }
    list.close();
    if (!has) {
        std::string adminId = "00";
        std::ofstream file("data/Admin/" + adminId + ".txt");
        file << "admin" << "\n" << "admin" << "\n";
        file.close();
        std::ofstream listw("data/Admin.txt", std::ios::app);
        listw << adminId << "\n";
        listw.close();
    }
}

// Original console-based login (DISABLED for GUI-only app)
bool Account::login(std::string& outRole, std::string& outId) {
    // This function is deprecated - use loginWithCredentials() instead
    std::cerr << "Error: Console login is disabled. Use GUI login." << std::endl;
    return false;
}

// NEW: GUI-compatible login
bool Account::loginWithCredentials(const std::string& inputUser, const std::string& inputPass, 
                                   std::string& outRole, std::string& outId) {
    bool success = false;
    for (auto roleName : {"Admin", "Doctor", "Patient"}) {
        std::ifstream idList("data/" + std::string(roleName) + ".txt");
        if (!idList.is_open()) continue;
        std::string id;
        while (idList >> id) {
            std::ifstream file("data/" + std::string(roleName) + "/" + id + ".txt");
            if (!file.is_open()) continue;
            std::string fileUser, filePass;
            std::getline(file, fileUser);
            std::getline(file, filePass);
            if (inputUser == fileUser && inputPass == filePass) {
                // set current account state
                role = roleName;
                this->id = id;
                username = inputUser;
                password = inputPass;
                outRole = roleName;
                outId = id;
                success = true;
                break;
            }
        }
        if (success) break;
    }
    return success;
}

void Account::logout() {
    std::cout << "Da dang xuat.\n";
}

// Original console-based register (DISABLED for GUI-only app)
void Account::registerAccount() {
    // This function is deprecated - use registerWithCredentials() instead
    std::cerr << "Error: Console registration is disabled. Use GUI registration." << std::endl;
    return;
}

// NEW: GUI-compatible register
bool Account::registerWithCredentials(const std::string& inputUser, const std::string& inputPass,
                                     const std::string& inputRole, std::string& outId) {
    // Validate role
    std::string normalizedRole = inputRole;
    for (auto &c : normalizedRole) c = std::tolower((unsigned char)c);
    
    if (normalizedRole == "doctor") {
        role = "Doctor";
    } else if (normalizedRole == "patient") {
        role = "Patient";
    } else {
        return false; // Invalid role
    }

    // Check if username already exists
    for (auto roleName : {"Admin", "Doctor", "Patient"}) {
        std::ifstream idList("data/" + std::string(roleName) + ".txt");
        if (!idList.is_open()) continue;
        std::string existingId;
        while (idList >> existingId) {
            std::ifstream file("data/" + std::string(roleName) + "/" + existingId + ".txt");
            if (!file.is_open()) continue;
            std::string fileUser;
            std::getline(file, fileUser);
            if (inputUser == fileUser) {
                return false; // Username already exists
            }
        }
    }

    username = inputUser;
    password = inputPass;

    id = generateID(role);
    saveUserData(role, id);
    appendIDToList(role, id);
    
    outId = id;
    return true;
}

std::string Account::generateID(const std::string& role) {
    std::string prefix;
    int count = 0;

    if (role == "Patient") {
        prefix = "01";
    } else if (role == "Doctor") {
        // For GUI/automatic registration, use default department code "01"
        prefix = "0201";  // 02 = Doctor, 01 = Default department
    } else if (role == "Admin") {
        prefix = "00";
    }

    std::ifstream idList("data/" + role + ".txt");
    std::string lastID;
    while (idList >> lastID) count++;
    idList.close();

    std::ostringstream oss;
    oss << prefix << std::setw(2) << std::setfill('0') << (count + 1);
    return oss.str();
}

void Account::saveUserData(const std::string& role, const std::string& id) {
    fs::create_directories("data/" + role);
    std::ofstream file("data/" + role + "/" + id + ".txt");
    file << username << "\n" << password << "\n";
    file.close();
}

void Account::appendIDToList(const std::string& role, const std::string& id) {
    std::ofstream list("data/" + role + ".txt", std::ios::app);
    list << id << "\n";
    list.close();
}

Account& Account::operator=(const Account& other) {
    if (this != &other) {
        role = other.role;
        id = other.id;
        username = other.username;
        password = other.password;
    }
    return *this;
}

std::string Account::getUsername() const {
    return username;
}

std::string Account::getID() const {
    return id;
}