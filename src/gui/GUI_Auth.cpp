#include "GUI.h"
#include "DataStore.h"
#include <iostream>
#include <fstream>

void GUI::completeRegistration() {
    if (inputUsername.empty() || inputPassword.empty() || inputEmail.empty()) {
        std::cout << u8"❌ Lỗi: Vui lòng điền đầy đủ thông tin!" << std::endl;
        return;
    }
    if (inputPassword != inputConfirmPassword) {
        std::cout << u8"❌ Lỗi: Mật khẩu không khớp!" << std::endl;
        return;
    }
    if (inputUsername.length() < 3) {
        std::cout << u8"❌ Lỗi: Tên đăng nhập phải có ít nhất 3 ký tự!" << std::endl;
        return;
    }
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
        std::ofstream emailFile("data/" + role + "/" + generatedId + "_email.txt");
        if (emailFile.is_open()) {
            emailFile << inputEmail;
        }
        std::cout << u8"✅ Đăng ký thành công! ID: " << generatedId << std::endl;
        inputPassword.clear();
        inputConfirmPassword.clear();
    // Gửi thông báo ngay khi đăng ký thành công
    DataStore::appendNotification(currentUserId, u8"Đăng ký thành công. Vui lòng đăng nhập và vào mục 'Cập nhật thông tin' để hoàn thiện hồ sơ.");

    // Sau khi đăng ký, chuyển về màn hình đăng nhập.
    // Bệnh nhân/Bác sĩ có thể vào mục "Cập nhật thông tin" sau khi đăng nhập để bổ sung hồ sơ.
        switchToScreen(Screen::LOGIN);
    } else {
        std::cout << u8"❌ Đăng ký thất bại! Tên đăng nhập đã tồn tại." << std::endl;
    }
}

bool GUI::validateRegistrationInputs() {
    return !inputUsername.empty() && !inputPassword.empty() && 
           !inputEmail.empty() && (inputPassword == inputConfirmPassword);
}

void GUI::completeLogin() {
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
