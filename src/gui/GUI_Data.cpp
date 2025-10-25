#include "GUI.h"
#include "DataStore.h"
#include <iostream>

void GUI::loadPatientInfo() {
    DataStore::PatientInfo info = DataStore::readPatientInfo(currentUserId);
    inputFullName = info.name;
    inputPhone = info.phone;
    inputEmail = info.email;
    inputAddress = info.address;
    inputDOB = info.dateOfBirth;
    inputGender = info.gender;
}

void GUI::savePatientInfo() {
    DataStore::PatientInfo info;
    info.name = inputFullName;
    info.phone = inputPhone;
    info.email = inputEmail;
    info.address = inputAddress;
    info.dateOfBirth = inputDOB;
    info.gender = inputGender;
    if (DataStore::writePatientInfo(currentUserId, info)) {
        std::cout << u8"Lưu thông tin thành công!" << std::endl;
    } else {
        std::cout << u8"Lưu thông tin thất bại!" << std::endl;
    }
}

void GUI::loadDoctorInfo() {
    DataStore::DoctorInfo info = DataStore::readDoctorInfo(currentUserId);
    inputFullName = info.name;
    inputPhone = info.phone;
    inputEmail = info.email;
    inputSpecialization = info.specialization;
    inputFaculty = info.experience;
}

void GUI::saveDoctorInfo() {
    DataStore::DoctorInfo info;
    info.name = inputFullName;
    info.phone = inputPhone;
    info.email = inputEmail;
    info.specialization = inputSpecialization;
    info.experience = inputFaculty;
    if (DataStore::writeDoctorInfo(currentUserId, info)) {
        std::cout << u8"Lưu thông tin thành công!" << std::endl;
    } else {
        std::cout << u8"Lưu thông tin thất bại!" << std::endl;
    }
}
