#include "User.h"
#include <iostream>
#include <string>

User::User() {
    ID = "";
    Name = "";
    SDT = "";
    Email = "";
    Address = "";
    DOB = "";
    Gender = "";
    Nhommau = "";
    CCCD = "";
}

User::User(std::string id, std::string name, std::string sdt, std::string email, std::string address, std::string dob, std::string gender, std::string nhommau, std::string cccd, Account acc) {
    ID = id;
    Name = name;
    SDT = sdt;
    Email = email;
    Address = address;
    DOB = dob;
    Gender = gender;
    Nhommau = nhommau;
    CCCD = cccd;
    acc = acc;
}

User::~User() {}

void User::showInfo() {
    std::cout << "ID: " << ID << std::endl;
    std::cout << "Name: " << Name << std::endl;
    std::cout << "SDT: " << SDT << std::endl;
    std::cout << "Email: " << Email << std::endl;
    std::cout << "Address: " << Address << std::endl;
    std::cout << "DOB: " << DOB << std::endl;
    std::cout << "Gender: " << Gender << std::endl;
    std::cout << "Nhommau: " << Nhommau << std::endl;
    std::cout << "CCCD: " << CCCD << std::endl;
}

void User::setName(std::string name) {
    Name = name;
}

void User::setSDT(std::string sdt) {
    SDT = sdt;
}

void User::setEmail(std::string email) {
    Email = email;
}

void User::setAddress(std::string address) {
    Address = address;
}

void User::setDOB(std::string dob) {
    DOB = dob;
}

void User::setGender(std::string gender) {
    Gender = gender;
}

void User::setNhommau(std::string nhommau) {
    Nhommau = nhommau;
}

void User::setCCCD(std::string cccd) {
    CCCD = cccd;
}

void User::setID() {
}
const string* User::getName() const {

}
const string* getSDT() const{

}
const string* getEmail() const{

}
const string* getAddress() const{

}
const string* getDOB() const{

}
const string* getGender() const{

}
const string* getNhommau() const{

}
const string* getCCCD() const{

}
// define static member
std::string User::role = "";