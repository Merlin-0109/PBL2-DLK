#include "DataStore.h"
#include <filesystem>
#include <fstream>
#include <sstream>

namespace fs = std::filesystem;

namespace DataStore {

std::vector<std::string> listIDs(const std::string& role) {
    std::vector<std::string> out;
    std::ifstream list("data/" + role + ".txt");
    if (!list.is_open()) return out;
    std::string id;
    while (list >> id) out.push_back(id);
    return out;
}

std::pair<std::string,std::string> readUser(const std::string& role, const std::string& id) {
    std::ifstream file("data/" + role + "/" + id + ".txt");
    std::string u, p;
    if (!file.is_open()) return {"",""};
    std::getline(file, u);
    std::getline(file, p);
    return {u,p};
}

bool appendAppointment(const std::string& patientId, const std::string& doctorId, const std::string& date, const std::string& time, const std::string& reason, std::string& outAppointmentId) {
    fs::create_directories("data/appointments");
    // generate simple appointment id and sanitize characters not allowed in Windows filenames
    std::ostringstream oss;
    oss << "A" << patientId << "_" << doctorId << "_" << date << "_" << time;
    outAppointmentId = oss.str();
    for (auto &c : outAppointmentId) {
        if (c == ':' || c == '/' || c == '\\' || c == ' ')
            c = '-';
    }
    std::ofstream f("data/appointments/" + outAppointmentId + ".txt");
    if (!f.is_open()) return false;
    f << patientId << "\n" << doctorId << "\n" << date << "\n" << time << "\n" << reason << "\n";
    return true;
}

std::vector<std::string> listAppointmentsForPatient(const std::string& patientId) {
    std::vector<std::string> out;
    if (!fs::exists("data/appointments")) return out;
    for (auto &p : fs::directory_iterator("data/appointments")) {
        if (!p.is_regular_file()) continue;
        std::ifstream f(p.path());
        std::string pid; std::getline(f, pid);
        if (pid == patientId) out.push_back(p.path().filename().string());
    }
    return out;
}

std::optional<AppointmentDetails> readAppointment(const std::string& filename) {
    AppointmentDetails d;
    std::ifstream f("data/appointments/" + filename);
    if (!f.is_open()) return std::nullopt;
    // if file is empty, treat as missing/invalid
    if (f.peek() == std::ifstream::traits_type::eof()) return std::nullopt;
    d.filename = filename;
    std::getline(f, d.patientId);
    std::getline(f, d.doctorId);
    std::getline(f, d.date);
    std::getline(f, d.time);
    std::getline(f, d.reason);
    return d;
}

void ensureAppointmentsDirExists() {
    fs::create_directories("data/appointments");
}

// Helper to read common user info fields
void readUserInfoFields(std::ifstream& f, UserInfo& info) {
    // Skip username/password lines
    std::string skip;
    std::getline(f, skip); // username
    std::getline(f, skip); // password
    
    std::getline(f, info.name);
    std::getline(f, info.phone);
    std::getline(f, info.email);
    std::getline(f, info.dob);
    std::getline(f, info.gender);
    std::getline(f, info.address);
    std::getline(f, info.cccd);
}

// Helper to write common user info fields
void writeUserInfoFields(std::ofstream& f, const UserInfo& info, const std::string& existingUsername, const std::string& existingPassword) {
    f << (existingUsername.empty() ? info.name : existingUsername) << "\n"
      << (existingPassword.empty() ? "default" : existingPassword) << "\n"
      << info.name << "\n"
      << info.phone << "\n"
      << info.email << "\n"
      << info.dob << "\n"
      << info.gender << "\n"
      << info.address << "\n"
      << info.cccd << "\n";
}

DoctorInfo readDoctorInfo(const std::string& doctorId) {
    DoctorInfo info;
    std::ifstream f("data/doctor/" + doctorId + ".txt");
    if (!f.is_open()) return info;

    readUserInfoFields(f, info);
    
    // Read doctor-specific fields
    std::getline(f, info.specialization);
    std::getline(f, info.faculty);
    
    return info;
}

bool writeDoctorInfo(const std::string& doctorId, const DoctorInfo& info) {
    fs::create_directories("data/doctor");
    
    // Read existing username/password
    std::string existingUsername, existingPassword;
    {
        std::ifstream check("data/doctor/" + doctorId + ".txt");
        if (check.is_open()) {
            std::getline(check, existingUsername);
            std::getline(check, existingPassword);
        }
    }
    
    std::ofstream f("data/doctor/" + doctorId + ".txt");
    if (!f.is_open()) return false;
    
    writeUserInfoFields(f, info, existingUsername, existingPassword);
    f << info.specialization << "\n"
      << info.faculty << "\n";
    
    return true;
}

PatientInfo readPatientInfo(const std::string& patientId) {
    PatientInfo info;
    std::ifstream f("data/Patient/" + patientId + ".txt");
    if (!f.is_open()) return info;
    
    readUserInfoFields(f, info);
    
    return info;
}

bool writePatientInfo(const std::string& patientId, const PatientInfo& info) {
    fs::create_directories("data/Patient");
    
    // Read existing username/password
    std::string existingUsername, existingPassword;
    {
        std::ifstream check("data/Patient/" + patientId + ".txt");
        if (check.is_open()) {
            std::getline(check, existingUsername);
            std::getline(check, existingPassword);
        }
    }
    
    std::ofstream f("data/Patient/" + patientId + ".txt");
    if (!f.is_open()) return false;
    
    writeUserInfoFields(f, info, existingUsername, existingPassword);
    
    return true;
}

} // namespace DataStore
