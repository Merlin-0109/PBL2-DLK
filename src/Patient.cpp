#include "Patient.h"
#include <iostream>
#include <string>

Patient::Patient() : User() {
    count++;
}

Patient::Patient(std::string id, std::string name, std::string sdt, std::string email, std::string address, std::string dob, std::string gender, std::string nhommau, std::string cccd, Account acc)
    : User(id, name, sdt, email, address, dob, gender, nhommau, cccd, acc) {
    count++;
    
}

Patient::~Patient() {}

void Patient::showInfo() {
    User::showInfo();
    
}
void Patient::chinhsuathongtin() {
    int a;
    std::cout << "Editing patient information" << std::endl;
    std::cout << "Nhap thong tin ban muon chinh sua:"<<std::endl;
    std::cout << "1. Name"<<std::endl;
    std::cout << "2. SDT"<<std::endl;
    std::cout << "3. Email"<<std::endl;
    std::cout << "4. Address"<<std::endl;
    std::cout << "5. DOB"<<std::endl;
    std::cout << "6. Gender"<<std::endl;
    std::cout << "7. Nhommau"<<std::endl;
    std::cout << "8. CCCD"<<std::endl;
    std::cin >> a;
    while (a !=0){
    switch (a) { 
        case 1:
            {
                std::string newName;
                std::cout << "Enter new Name: ";
                std::cin >> newName;
                User::setName(newName);
            }
        case 2:
            {
                std::string newSDT;
                std::cout << "Enter new SDT: ";
                std::cin >> newSDT;
                User::setSDT(newSDT);
            }
        case 3:
            {
                std::string newEmail;
                std::cout << "Enter new Email: ";
                std::cin >> newEmail;
                User::setEmail(newEmail);
            }
        case 4:
            {
                std::string newAddress;
                std::cout << "Enter new Address: ";
                std::cin >> newAddress;
                User::setAddress(newAddress);
            }
        case 5:
            {
                std::string newDOB;
                std::cout << "Enter new DOB: ";
                std::cin >> newDOB;
                User::setDOB(newDOB);
            }
        case 6:
            {
                std::string newGender;
                std::cout << "Enter new Gender: ";
                std::cin >> newGender;
                User::setGender(newGender);
            }
        case 7:
            {
                std::string newNhommau;
                std::cout << "Enter new Nhommau: ";
                std::cin >> newNhommau;
                User::setNhommau(newNhommau);
            }
        case 8:
            {
                std::string newCCCD;
                std::cout << "Enter new CCCD: ";
                std::cin >> newCCCD;
                User::setCCCD(newCCCD);
            }
        case 0:
            std::cout << "No changes made." << std::endl;
            break;
        default:
            std::cout << "Invalid option." << std::endl;
            break;
    }
    }
    std::cout << "Information updated." << std::endl;
}
void Patient::datlichkham() {
    std::cout << "Booking an appointment..." << std::endl;
    std::cin >> myLichKham;
    myLichKham.setTrangThai("Da dat");

}

void Patient::danhgia() {
    std::cout << "Providing feedback..." << std::endl;
    std::string feedback;
    std::cout << "Enter your feedback: ";
    std::cin.ignore(); // To ignore the newline character left in the buffer
    std::getline(std::cin, feedback);
    std::cout << "Thank you for your feedback: " << feedback << std::endl;
}

void Patient::huylichkham() {
    std::cout << "Canceling an appointment..." << std::endl;
    myLichKham.setTrangThai("Da huy");
    
    // Implementation for canceling an appointment
}

void Patient::setID() {
    role = "01";// Assuming role 1 is for Patient
    ID = role + std::to_string(count); // Example ID generation logic
}

// define static member
int Patient::count = 0;