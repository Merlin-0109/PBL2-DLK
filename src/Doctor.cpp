#include "Doctor.h"

Doctor::Doctor() {
    doctorCount++;
}

Doctor::Doctor(string id, string name, string sdt, string email, string address, 
        string dob, string gender, string cccd, string specialization, 
        string faculty, Account acc) : User(id, name, sdt, email, address, dob, gender, "", cccd, acc) {
    this->specialization = specialization;
    this->faculty = faculty;
    this->doctorRole = "Doctor"; // Default role
    doctorCount++;
}
Doctor::~Doctor(){}

void Doctor::setSpecialization(string specialization) {
    this->specialization = specialization;
}

void Doctor::setDoctorRole(string doctorRole) {
    this->doctorRole = doctorRole;
}

void Doctor::setFaculty(string faculty) {
    this->faculty = faculty;
}

void Doctor::updateInformation(const string& name, const string& sdt, 
    const string& email, const string& address, const string& dob,
    const string& gender, const string& cccd, const string& specialization,
    const string& faculty) {
    
    // Update base class info
    setName(name);
    setSDT(sdt);
    setEmail(email);
    setAddress(address);
    setDOB(dob);
    setGender(gender);
    setCCCD(cccd);
    
    // Update doctor-specific info
    this->specialization = specialization;
    this->faculty = faculty;
}

ostream& operator<<(ostream& o, const Doctor& doctor) {
    o << "Doctor Information:\n"
      << "ID: " << doctor.ID << "\n"
      << "Name: " << doctor.getName() << "\n"
      << "Specialization: " << doctor.getSpecialization() << "\n"
      << "Faculty: " << doctor.getFaculty() << "\n"
      << "Phone: " << doctor.getSDT() << "\n"
      << "Email: " << doctor.getEmail() << "\n"
      << "Address: " << doctor.getAddress() << "\n"
      << "Date of Birth: " << doctor.getDOB() << "\n"
      << "Gender: " << doctor.getGender() << "\n"
      << "CCCD: " << doctor.getCCCD();
    return o;
}

// define static member
int Doctor::doctorCount = 0;