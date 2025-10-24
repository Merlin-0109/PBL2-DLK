// Account.cpp - System authentication and account management
#include "Account.h"
#include <algorithm>
#include <cctype>
#include <filesystem>

// Helper: trim leading/trailing whitespace
static inline std::string trim(const std::string& s) {
    size_t start = 0;
    while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start]))) start++;
    size_t end = s.size();
    while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) end--;
    return s.substr(start, end - start);
}

// Helper: strip UTF-8 BOM if present
static inline void stripBOM(std::string& s) {
    if (s.size() >= 3 && static_cast<unsigned char>(s[0]) == 0xEF &&
        static_cast<unsigned char>(s[1]) == 0xBB &&
        static_cast<unsigned char>(s[2]) == 0xBF) {
        s.erase(0, 3);
    }
}

Account::Account() : role(""), id(""), username(""), password("") {}

Account::~Account() {}

// ============================================================================
// STATIC UTILITIES
// ============================================================================

void Account::ensureDefaultAdminExists() {
    fs::create_directories("data/Admin");
    
    std::ifstream list("data/Admin.txt");
    bool hasAdmin = false;
    std::string id;
    while (list >> id) { 
        hasAdmin = true; 
        break; 
    }
    list.close();
    
    if (!hasAdmin) {
        std::string adminId = "00";
        std::ofstream file("data/Admin/" + adminId + ".txt");
        file << "admin" << "\n" << "admin" << "\n";
        file.close();
        
        std::ofstream listWrite("data/Admin.txt", std::ios::app);
        listWrite << adminId << "\n";
        listWrite.close();
        
        std::cout << u8"Đã tạo tài khoản admin mặc định (username: admin, password: admin)\n";
    }
}

bool Account::validateUsername(const std::string& username) {
    if (username.length() < 3 || username.length() > 20) {
        return false;
    }
    
    for (char c : username) {
        if (!std::isalnum(static_cast<unsigned char>(c)) && c != '_' && c != '.') {
            return false;
        }
    }
    
    return true;
}

bool Account::validatePassword(const std::string& password) {
    return password.length() >= 6 && password.length() <= 50;
}

// ============================================================================
// AUTHENTICATION
// ============================================================================

bool Account::loginWithCredentials(const std::string& inputUser, const std::string& inputPass, 
                                   std::string& outRole, std::string& outId) {
    if (inputUser.empty() || inputPass.empty()) {
        std::cout << u8"Lỗi: Tên đăng nhập và mật khẩu không được để trống.\n";
        return false;
    }
    
    // Search through all role types
    for (const char* roleName : {"Admin", "Doctor", "Patient"}) {
        std::string roleStr(roleName);
        std::ifstream idList("data/" + roleStr + ".txt");
        
        if (!idList.is_open()) {
            std::cout << u8"Cảnh báo: Không mở được danh sách ID cho vai trò '" << roleStr
                      << u8"' tại đường dẫn: " << (std::filesystem::current_path() / ("data/" + roleStr + ".txt")) << "\n";
            continue;
        }
        
        std::string currentId;
        while (idList >> currentId) {
            std::ifstream accountFile("data/" + roleStr + "/" + currentId + ".txt");
            
            if (!accountFile.is_open()) {
                continue;
            }
            
            std::string fileUser, filePass;
            std::getline(accountFile, fileUser);
            std::getline(accountFile, filePass);
            accountFile.close();
            stripBOM(fileUser);
            stripBOM(filePass);
            fileUser = trim(fileUser);
            filePass = trim(filePass);
            std::string inUser = trim(inputUser);
            std::string inPass = trim(inputPass);
            
            if (inUser == fileUser && inPass == filePass) {
                // Authentication successful
                this->role = roleStr;
                this->id = currentId;
                this->username = inUser;
                this->password = inPass;
                
                outRole = roleStr;
                outId = currentId;
                
                std::cout << u8"Đăng nhập thành công! Vai trò: " << roleStr 
                         << u8", ID: " << currentId << "\n";
                return true;
            }
        }
        idList.close();
    }
    
    std::cout << u8"Đăng nhập thất bại! Tên đăng nhập hoặc mật khẩu không đúng.\n";
    return false;
}

bool Account::registerWithCredentials(const std::string& inputUser, const std::string& inputPass,
                                     const std::string& inputRole, std::string& outId) {
    // Validate inputs
    if (!validateUsername(inputUser)) {
        std::cout << u8"Lỗi: Tên đăng nhập không hợp lệ (3-20 ký tự, chỉ chữ, số, _, .)\n";
        return false;
    }
    
    if (!validatePassword(inputPass)) {
        std::cout << u8"Lỗi: Mật khẩu phải có từ 6-50 ký tự.\n";
        return false;
    }
    
    // Normalize role
    std::string normalizedRole = inputRole;
    std::transform(normalizedRole.begin(), normalizedRole.end(), 
                  normalizedRole.begin(), ::tolower);
    
    if (normalizedRole == "doctor") {
        this->role = "Doctor";
    } else if (normalizedRole == "patient") {
        this->role = "Patient";
    } else {
        std::cout << u8"Lỗi: Vai trò không hợp lệ. Chỉ chấp nhận 'patient' hoặc 'doctor'.\n";
        return false;
    }
    
    // Check if username already exists in any role
    if (usernameExists(inputUser)) {
        std::cout << u8"Lỗi: Tên đăng nhập đã tồn tại.\n";
        return false;
    }
    
    // Create account
    this->username = inputUser;
    this->password = inputPass;
    this->id = generateID(this->role);
    
    saveUserData(this->role, this->id);
    appendIDToList(this->role, this->id);
    
    outId = this->id;
    
    std::cout << u8"Đăng ký thành công! ID: " << this->id 
             << u8", Vai trò: " << this->role << "\n";
    return true;
}

void Account::logout() {
    std::cout << u8"Đã đăng xuất tài khoản: " << username << "\n";
    
    role.clear();
    id.clear();
    username.clear();
    password.clear();
}

bool Account::changePassword(const std::string& oldPass, const std::string& newPass) {
    if (this->password != oldPass) {
        std::cout << u8"Lỗi: Mật khẩu cũ không đúng.\n";
        return false;
    }
    
    if (!validatePassword(newPass)) {
        std::cout << u8"Lỗi: Mật khẩu mới phải có từ 6-50 ký tự.\n";
        return false;
    }
    
    this->password = newPass;
    saveUserData(this->role, this->id);
    
    std::cout << u8"Đổi mật khẩu thành công!\n";
    return true;
}

bool Account::usernameExists(const std::string& username) const {
    for (const char* roleName : {"Admin", "Doctor", "Patient"}) {
        std::string roleStr(roleName);
        std::ifstream idList("data/" + roleStr + ".txt");
        
        if (!idList.is_open()) {
            continue;
        }
        
        std::string existingId;
        while (idList >> existingId) {
            std::ifstream accountFile("data/" + roleStr + "/" + existingId + ".txt");
            
            if (!accountFile.is_open()) {
                continue;
            }
            
            std::string fileUser;
            std::getline(accountFile, fileUser);
            accountFile.close();
            
            if (username == fileUser) {
                return true;
            }
        }
        idList.close();
    }
    
    return false;
}

// ============================================================================
// PRIVATE HELPERS
// ============================================================================

std::string Account::generateID(const std::string& role) {
    std::string prefix;
    int count = 0;

    if (role == "Patient") {
        prefix = "01";
    } else if (role == "Doctor") {
        prefix = "02";
    } else if (role == "Admin") {
        prefix = "00";
    }

    std::ifstream idList("data/" + role + ".txt");
    std::string lastID;
    while (idList >> lastID) {
        count++;
    }
    idList.close();

    std::ostringstream oss;
    oss << prefix << std::setw(4) << std::setfill('0') << (count + 1);
    return oss.str();
}

void Account::saveUserData(const std::string& role, const std::string& id) {
    fs::create_directories("data/" + role);
    
    std::ofstream file("data/" + role + "/" + id + ".txt");
    if (!file.is_open()) {
        std::cerr << u8"Lỗi: Không thể lưu dữ liệu người dùng.\n";
        return;
    }
    
    file << username << "\n" << password << "\n";
    file.close();
}

void Account::appendIDToList(const std::string& role, const std::string& id) {
    std::ofstream list("data/" + role + ".txt", std::ios::app);
    if (!list.is_open()) {
        std::cerr << u8"Lỗi: Không thể cập nhật danh sách ID.\n";
        return;
    }
    
    list << id << "\n";
    list.close();
}

// ============================================================================
// OPERATORS & GETTERS
// ============================================================================

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

std::string Account::getRole() const {
    return role;
}
