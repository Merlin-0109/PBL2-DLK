// main.cpp - Integrated Console and GUI Application
#include <iostream>
#include <limits>
#include "Account.h"
#include "Guest.h"
#include "Patient.h"
#include "Doctor.h"
#include "PatientService.h"
#include "DoctorService.h"
#include "DataStore.h"
#include "GUI.h"

using namespace std;

// Console menu functions
void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void pauseScreen() {
    cout << "\nNhan Enter de tiep tuc...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

void showMainMenu() {
    clearScreen();
    cout << "========================================\n";
    cout << "   DAT LICH KHAM - QUAN LY BENH VIEN   \n";
    cout << "========================================\n";
    cout << "1. Dang nhap\n";
    cout << "2. Dang ky\n";
    cout << "3. Mo giao dien GUI\n";
    cout << "0. Thoat\n";
    cout << "========================================\n";
    cout << "Chon: ";
}

void showPatientMenu() {
    clearScreen();
    cout << "========================================\n";
    cout << "         MENU BENH NHAN                 \n";
    cout << "========================================\n";
    cout << "1. Dat lich kham\n";
    cout << "2. Xem lich kham\n";
    cout << "3. Huy lich kham\n";
    cout << "4. Cap nhat thong tin\n";
    cout << "5. Xem thong tin ca nhan\n";
    cout << "0. Dang xuat\n";
    cout << "========================================\n";
    cout << "Chon: ";
}

void showDoctorMenu() {
    clearScreen();
    cout << "========================================\n";
    cout << "          MENU BAC SI                   \n";
    cout << "========================================\n";
    cout << "1. Xem lich hen\n";
    cout << "2. Cap nhat thong tin\n";
    cout << "3. Xem thong tin ca nhan\n";
    cout << "0. Dang xuat\n";
    cout << "========================================\n";
    cout << "Chon: ";
}

void showAdminMenu() {
    clearScreen();
    cout << "========================================\n";
    cout << "        MENU QUAN TRI VIEN              \n";
    cout << "========================================\n";
    cout << "1. Quan ly benh nhan\n";
    cout << "2. Quan ly bac si\n";
    cout << "3. Thong ke\n";
    cout << "0. Dang xuat\n";
    cout << "========================================\n";
    cout << "Chon: ";
}

void handlePatientBooking(const string& patientId) {
    clearScreen();
    cout << "========================================\n";
    cout << "         DAT LICH KHAM                  \n";
    cout << "========================================\n";
    
    vector<string> doctorIds = DoctorService::getDoctorIDs();
    
    if (doctorIds.empty()) {
        cout << "Khong co bac si nao trong he thong!\n";
        pauseScreen();
        return;
    }
    
    cout << "\nDanh sach bac si:\n";
    cout << "----------------------------------------\n";
    for (size_t i = 0; i < doctorIds.size(); i++) {
        auto info = DataStore::readDoctorInfo(doctorIds[i]);
        cout << (i + 1) << ". Dr. " << info.name;
        if (!info.specialization.empty()) {
            cout << " - " << info.specialization;
        }
        cout << " (ID: " << doctorIds[i] << ")\n";
    }
    cout << "----------------------------------------\n";
    
    int choice;
    cout << "\nChon bac si (1-" << doctorIds.size() << "): ";
    cin >> choice;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    
    if (choice < 1 || choice > static_cast<int>(doctorIds.size())) {
        cout << "Lua chon khong hop le!\n";
        pauseScreen();
        return;
    }
    
    string selectedDoctorId = doctorIds[choice - 1];
    
    string date, time, reason;
    cout << "\nNhap ngay kham (YYYY-MM-DD): ";
    getline(cin, date);
    
    cout << "Nhap gio kham (HH:MM): ";
    getline(cin, time);
    
    cout << "Nhap ly do kham: ";
    getline(cin, reason);
    
    string appointmentId;
    if (PatientService::bookAppointment(patientId, selectedDoctorId, date, time, reason, appointmentId)) {
        cout << "\n========================================\n";
        cout << "Dat lich thanh cong!\n";
        cout << "Ma lich kham: " << appointmentId << "\n";
        cout << "========================================\n";
    } else {
        cout << "\nKhong the dat lich kham!\n";
    }
    
    pauseScreen();
}

void handleViewAppointments(const string& patientId) {
    clearScreen();
    cout << "========================================\n";
    cout << "       LICH KHAM CUA BAN                \n";
    cout << "========================================\n";
    
    auto appointments = PatientService::getAppointments(patientId);
    
    if (appointments.empty()) {
        cout << "Ban chua co lich kham nao!\n";
    } else {
        cout << "\n";
        for (size_t i = 0; i < appointments.size(); i++) {
            const auto& appt = appointments[i];
            cout << "----------------------------------------\n";
            cout << (i + 1) << ". Ma lich: " << appt.appointmentId << "\n";
            cout << "   Bac si: " << appt.doctorId << "\n";
            cout << "   Ngay: " << appt.date << " | Gio: " << appt.time << "\n";
            cout << "   Ly do: " << appt.reason << "\n";
            cout << "   Trang thai: " << appt.status << "\n";
        }
        cout << "----------------------------------------\n";
    }
    
    pauseScreen();
}

void handleCancelAppointment(const string& patientId) {
    clearScreen();
    cout << "========================================\n";
    cout << "        HUY LICH KHAM                   \n";
    cout << "========================================\n";
    
    auto appointments = PatientService::getAppointments(patientId);
    
    if (appointments.empty()) {
        cout << "Ban chua co lich kham nao de huy!\n";
        pauseScreen();
        return;
    }
    
    cout << "\nDanh sach lich kham:\n";
    for (size_t i = 0; i < appointments.size(); i++) {
        const auto& appt = appointments[i];
        cout << (i + 1) << ". " << appt.date << " " << appt.time 
             << " - Dr. " << appt.doctorId << "\n";
    }
    
    int choice;
    cout << "\nChon lich can huy (1-" << appointments.size() << "): ";
    cin >> choice;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    
    if (choice < 1 || choice > static_cast<int>(appointments.size())) {
        cout << "Lua chon khong hop le!\n";
        pauseScreen();
        return;
    }
    
    string appointmentId = appointments[choice - 1].appointmentId;
    if (PatientService::cancelAppointment(appointmentId)) {
        cout << "\nHuy lich thanh cong!\n";
    } else {
        cout << "\nKhong the huy lich kham!\n";
    }
    
    pauseScreen();
}

void handleUpdatePatientProfile(const string& patientId) {
    clearScreen();
    cout << "========================================\n";
    cout << "    CAP NHAT THONG TIN BENH NHAN       \n";
    cout << "========================================\n";
    
    auto info = DataStore::readPatientInfo(patientId);
    
    cout << "\nThong tin hien tai:\n";
    cout << "Ho ten: " << info.name << "\n";
    cout << "So dien thoai: " << info.phone << "\n";
    cout << "Email: " << info.email << "\n";
    cout << "Dia chi: " << info.address << "\n";
    
    cout << "\n--- Nhap thong tin moi (Enter de giu nguyen) ---\n";
    
    string input;
    cout << "Ho ten moi: ";
    getline(cin, input);
    if (!input.empty()) info.name = input;
    
    cout << "So dien thoai moi: ";
    getline(cin, input);
    if (!input.empty()) info.phone = input;
    
    cout << "Email moi: ";
    getline(cin, input);
    if (!input.empty()) info.email = input;
    
    cout << "Dia chi moi: ";
    getline(cin, input);
    if (!input.empty()) info.address = input;
    
    if (DataStore::writePatientInfo(patientId, info)) {
        cout << "\nCap nhat thanh cong!\n";
    } else {
        cout << "\nKhong the cap nhat thong tin!\n";
    }
    
    pauseScreen();
}

void handleViewPatientProfile(const string& patientId) {
    clearScreen();
    cout << "========================================\n";
    cout << "      THONG TIN BENH NHAN              \n";
    cout << "========================================\n";
    
    auto info = DataStore::readPatientInfo(patientId);
    
    cout << "\nMa benh nhan: " << patientId << "\n";
    cout << "Ho va ten: " << info.name << "\n";
    cout << "So dien thoai: " << info.phone << "\n";
    cout << "Email: " << info.email << "\n";
    cout << "Dia chi: " << info.address << "\n";
    
    pauseScreen();
}

void handleDoctorViewAppointments(const string& doctorId) {
    clearScreen();
    cout << "========================================\n";
    cout << "         LICH HEN CUA BAC SI           \n";
    cout << "========================================\n";
    
    auto appointments = DoctorService::getAppointments(doctorId);
    
    if (appointments.empty()) {
        cout << "Chua co lich hen nao!\n";
    } else {
        cout << "\n";
        for (size_t i = 0; i < appointments.size(); i++) {
            const auto& appt = appointments[i];
            cout << "----------------------------------------\n";
            cout << (i + 1) << ". Ma lich: " << appt.appointmentId << "\n";
            cout << "   Benh nhan: " << appt.patientId << "\n";
            cout << "   Ngay: " << appt.date << " | Gio: " << appt.time << "\n";
            cout << "   Ly do: " << appt.reason << "\n";
            cout << "   Trang thai: " << appt.status << "\n";
        }
        cout << "----------------------------------------\n";
    }
    
    pauseScreen();
}

void handleUpdateDoctorProfile(const string& doctorId) {
    clearScreen();
    cout << "========================================\n";
    cout << "      CAP NHAT THONG TIN BAC SI        \n";
    cout << "========================================\n";
    
    auto info = DataStore::readDoctorInfo(doctorId);
    
    cout << "\nThong tin hien tai:\n";
    cout << "Ho ten: " << info.name << "\n";
    cout << "So dien thoai: " << info.phone << "\n";
    cout << "Email: " << info.email << "\n";
    cout << "Chuyen khoa: " << info.specialization << "\n";
    
    cout << "\n--- Nhap thong tin moi (Enter de giu nguyen) ---\n";
    
    string input;
    cout << "Ho ten moi: ";
    getline(cin, input);
    if (!input.empty()) info.name = input;
    
    cout << "So dien thoai moi: ";
    getline(cin, input);
    if (!input.empty()) info.phone = input;
    
    cout << "Email moi: ";
    getline(cin, input);
    if (!input.empty()) info.email = input;
    
    cout << "Chuyen khoa moi: ";
    getline(cin, input);
    if (!input.empty()) info.specialization = input;
    
    if (DataStore::writeDoctorInfo(doctorId, info)) {
        cout << "\nCap nhat thanh cong!\n";
    } else {
        cout << "\nKhong the cap nhat thong tin!\n";
    }
    
    pauseScreen();
}

void handleViewDoctorProfile(const string& doctorId) {
    clearScreen();
    cout << "========================================\n";
    cout << "        THONG TIN BAC SI               \n";
    cout << "========================================\n";
    
    auto info = DataStore::readDoctorInfo(doctorId);
    
    cout << "\nMa bac si: " << doctorId << "\n";
    cout << "Ho va ten: " << info.name << "\n";
    cout << "So dien thoai: " << info.phone << "\n";
    cout << "Email: " << info.email << "\n";
    cout << "Chuyen khoa: " << info.specialization << "\n";
    
    pauseScreen();
}

void runPatientMenu(const string& patientId) {
    int choice;
    do {
        showPatientMenu();
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        switch (choice) {
            case 1:
                handlePatientBooking(patientId);
                break;
            case 2:
                handleViewAppointments(patientId);
                break;
            case 3:
                handleCancelAppointment(patientId);
                break;
            case 4:
                handleUpdatePatientProfile(patientId);
                break;
            case 5:
                handleViewPatientProfile(patientId);
                break;
            case 0:
                cout << "Dang xuat...\n";
                break;
            default:
                cout << "Lua chon khong hop le!\n";
                pauseScreen();
        }
    } while (choice != 0);
}

void runDoctorMenu(const string& doctorId) {
    int choice;
    do {
        showDoctorMenu();
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        switch (choice) {
            case 1:
                handleDoctorViewAppointments(doctorId);
                break;
            case 2:
                handleUpdateDoctorProfile(doctorId);
                break;
            case 3:
                handleViewDoctorProfile(doctorId);
                break;
            case 0:
                cout << "Dang xuat...\n";
                break;
            default:
                cout << "Lua chon khong hop le!\n";
                pauseScreen();
        }
    } while (choice != 0);
}

void runAdminMenu(const string& adminId) {
    int choice;
    do {
        showAdminMenu();
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        switch (choice) {
            case 1:
                cout << "Chuc nang quan ly benh nhan...\n";
                pauseScreen();
                break;
            case 2:
                cout << "Chuc nang quan ly bac si...\n";
                pauseScreen();
                break;
            case 3:
                cout << "Chuc nang thong ke...\n";
                pauseScreen();
                break;
            case 0:
                cout << "Dang xuat...\n";
                break;
            default:
                cout << "Lua chon khong hop le!\n";
                pauseScreen();
        }
    } while (choice != 0);
}

void runConsoleMode() {
    Account account;
    string role, id;
    int choice;
    
    do {
        showMainMenu();
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        switch (choice) {
            case 1: // Login
                clearScreen();
                cout << "========================================\n";
                cout << "           DANG NHAP                    \n";
                cout << "========================================\n";
                
                // Account::login already handles input internally
                if (account.login(role, id)) {
                    cout << "\nDang nhap thanh cong!\n";
                    cout << "Vai tro: " << role << " | ID: " << id << "\n";
                    pauseScreen();
                    
                    if (role == "Patient") {
                        runPatientMenu(id);
                    } else if (role == "Doctor") {
                        runDoctorMenu(id);
                    } else if (role == "Admin") {
                        runAdminMenu(id);
                    }
                } else {
                    cout << "\nDang nhap that bai!\n";
                    pauseScreen();
                }
                break;
                
            case 2: // Register
                clearScreen();
                cout << "========================================\n";
                cout << "            DANG KY                     \n";
                cout << "========================================\n";
                
                // Account::registerAccount already handles input internally
                account.registerAccount();
                pauseScreen();
                break;
                
            case 3: // GUI Mode
                try {
                    HospitalGUI gui;
                    gui.run();
                } catch (const exception& e) {
                    cerr << "Loi GUI: " << e.what() << endl;
                    pauseScreen();
                }
                break;
                
            case 0:
                cout << "\nCam on da su dung he thong!\n";
                break;
                
            default:
                cout << "Lua chon khong hop le!\n";
                pauseScreen();
        }
    } while (choice != 0);
}

int main(int argc, char* argv[]) {
    // Initialize system
    Account::ensureDefaultAdminExists();
    DataStore::ensureAppointmentsDirExists();
    
    try {
        // Always launch GUI mode
        HospitalGUI gui;
        gui.run();
    } catch (const exception& e) {
        cerr << "Loi: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}