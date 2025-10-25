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

void Patient::setID() {
    role = "01";// Assuming role 1 is for Patient
    ID = role + std::to_string(count); // Example ID generation logic
}

// define static member
int Patient::count = 0;