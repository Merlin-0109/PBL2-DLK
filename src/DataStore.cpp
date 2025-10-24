// DataStore.cpp - Complete implementation
#include "DataStore.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <ctime>
#include <iomanip>

namespace fs = std::filesystem;

// Ensure appointments directory exists
void DataStore::ensureAppointmentsDirExists() {
    fs::create_directories("data/appointments");
    fs::create_directories("data/Patient");
    fs::create_directories("data/Doctor");
    fs::create_directories("data/Admin");
}

// Generate unique appointment ID
std::string DataStore::generateAppointmentId() {
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    std::ostringstream oss;
    oss << "APT" << std::put_time(&tm, "%Y%m%d%H%M%S");
    return oss.str();
}

// ============================================
// Patient Functions
// ============================================
bool DataStore::writePatientInfo(const std::string& patientId, const PatientInfo& info) {
    std::string filepath = "data/Patient/" + patientId + "_info.txt";
    std::ofstream file(filepath);
    
    if (!file.is_open()) {
        return false;
    }
    
    file << "name:" << info.name << "\n";
    file << "phone:" << info.phone << "\n";
    file << "email:" << info.email << "\n";
    file << "address:" << info.address << "\n";
    file << "dateOfBirth:" << info.dateOfBirth << "\n";
    file << "gender:" << info.gender << "\n";
    
    file.close();
    return true;
}

DataStore::PatientInfo DataStore::readPatientInfo(const std::string& patientId) {
    PatientInfo info;
    std::string filepath = "data/Patient/" + patientId + "_info.txt";
    std::ifstream file(filepath);
    
    if (!file.is_open()) {
        return info; // Return empty struct
    }
    
    std::string line;
    while (std::getline(file, line)) {
        size_t pos = line.find(':');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            
            if (key == "name") info.name = value;
            else if (key == "phone") info.phone = value;
            else if (key == "email") info.email = value;
            else if (key == "address") info.address = value;
            else if (key == "dateOfBirth") info.dateOfBirth = value;
            else if (key == "gender") info.gender = value;
        }
    }
    
    file.close();
    return info;
}

bool DataStore::patientExists(const std::string& patientId) {
    std::string filepath = "data/Patient/" + patientId + "_info.txt";
    return fs::exists(filepath);
}

// ============================================
// Doctor Functions
// ============================================
bool DataStore::writeDoctorInfo(const std::string& doctorId, const DoctorInfo& info) {
    std::string filepath = "data/Doctor/" + doctorId + "_info.txt";
    std::ofstream file(filepath);
    
    if (!file.is_open()) {
        return false;
    }
    
    file << "name:" << info.name << "\n";
    file << "phone:" << info.phone << "\n";
    file << "email:" << info.email << "\n";
    file << "specialization:" << info.specialization << "\n";
    file << "experience:" << info.experience << "\n";
    
    file.close();
    return true;
}

DataStore::DoctorInfo DataStore::readDoctorInfo(const std::string& doctorId) {
    DoctorInfo info;
    std::string filepath = "data/Doctor/" + doctorId + "_info.txt";
    std::ifstream file(filepath);
    
    if (!file.is_open()) {
        return info;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        size_t pos = line.find(':');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            
            if (key == "name") info.name = value;
            else if (key == "phone") info.phone = value;
            else if (key == "email") info.email = value;
            else if (key == "specialization") info.specialization = value;
            else if (key == "experience") info.experience = value;
        }
    }
    
    file.close();
    return info;
}

bool DataStore::doctorExists(const std::string& doctorId) {
    std::string filepath = "data/Doctor/" + doctorId + "_info.txt";
    return fs::exists(filepath);
}

// ============================================
// Appointment Functions
// ============================================
bool DataStore::writeAppointment(const std::string& appointmentId, const AppointmentDetails& details) {
    std::string filepath = "data/appointments/" + appointmentId + ".txt";
    std::ofstream file(filepath);
    
    if (!file.is_open()) {
        return false;
    }
    
    file << "appointmentId:" << details.appointmentId << "\n";
    file << "patientId:" << details.patientId << "\n";
    file << "doctorId:" << details.doctorId << "\n";
    file << "date:" << details.date << "\n";
    file << "time:" << details.time << "\n";
    file << "reason:" << details.reason << "\n";
    file << "status:" << details.status << "\n";
    
    file.close();
    
    // Also save to patient's appointments list
    std::string patientListFile = "data/Patient/" + details.patientId + "_appointments.txt";
    std::ofstream patientFile(patientListFile, std::ios::app);
    if (patientFile.is_open()) {
        patientFile << appointmentId << "\n";
        patientFile.close();
    }
    
    // Also save to doctor's appointments list
    std::string doctorListFile = "data/Doctor/" + details.doctorId + "_appointments.txt";
    std::ofstream doctorFile(doctorListFile, std::ios::app);
    if (doctorFile.is_open()) {
        doctorFile << appointmentId << "\n";
        doctorFile.close();
    }
    
    return true;
}

DataStore::AppointmentDetails DataStore::readAppointment(const std::string& appointmentId) {
    AppointmentDetails details;
    std::string filepath = "data/appointments/" + appointmentId + ".txt";
    std::ifstream file(filepath);
    
    if (!file.is_open()) {
        return details;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        size_t pos = line.find(':');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            
            if (key == "appointmentId") details.appointmentId = value;
            else if (key == "patientId") details.patientId = value;
            else if (key == "doctorId") details.doctorId = value;
            else if (key == "date") details.date = value;
            else if (key == "time") details.time = value;
            else if (key == "reason") details.reason = value;
            else if (key == "status") details.status = value;
        }
    }
    
    file.close();
    return details;
}

std::vector<std::string> DataStore::getPatientAppointments(const std::string& patientId) {
    std::vector<std::string> appointments;
    std::string filepath = "data/Patient/" + patientId + "_appointments.txt";
    std::ifstream file(filepath);
    
    if (!file.is_open()) {
        return appointments;
    }
    
    std::string appointmentId;
    while (std::getline(file, appointmentId)) {
        if (!appointmentId.empty()) {
            appointments.push_back(appointmentId);
        }
    }
    
    file.close();
    return appointments;
}

std::vector<std::string> DataStore::getDoctorAppointments(const std::string& doctorId) {
    std::vector<std::string> appointments;
    std::string filepath = "data/Doctor/" + doctorId + "_appointments.txt";
    std::ifstream file(filepath);
    
    if (!file.is_open()) {
        return appointments;
    }
    
    std::string appointmentId;
    while (std::getline(file, appointmentId)) {
        if (!appointmentId.empty()) {
            appointments.push_back(appointmentId);
        }
    }
    
    file.close();
    return appointments;
}

bool DataStore::updateAppointmentStatus(const std::string& appointmentId, const std::string& newStatus) {
    auto details = readAppointment(appointmentId);
    if (details.appointmentId.empty()) {
        return false;
    }
    
    details.status = newStatus;
    return writeAppointment(appointmentId, details);
}

bool DataStore::deleteAppointment(const std::string& appointmentId) {
    std::string filepath = "data/appointments/" + appointmentId + ".txt";
    
    if (!fs::exists(filepath)) {
        return false;
    }
    
    // Read appointment to get patient and doctor IDs
    auto details = readAppointment(appointmentId);
    
    // Remove from appointment file
    fs::remove(filepath);
    
    // Remove from patient's list (would need more complex logic to remove line)
    // For now, just mark as cancelled
    updateAppointmentStatus(appointmentId, "Cancelled");
    
    return true;
}

// ============================================
// New Functions for Services
// ============================================
std::vector<std::string> DataStore::listIDs(const std::string& role) {
    std::vector<std::string> ids;
    std::string filepath = "data/" + role + ".txt";
    std::ifstream file(filepath);
    
    if (!file.is_open()) {
        return ids;
    }
    
    std::string id;
    while (file >> id) {
        if (!id.empty()) {
            ids.push_back(id);
        }
    }
    
    file.close();
    return ids;
}

bool DataStore::appendAppointment(const std::string& patientId, const std::string& doctorId, 
                                  const std::string& date, const std::string& time, 
                                  const std::string& reason, std::string& outAppointmentId) {
    // Generate appointment ID
    outAppointmentId = generateAppointmentId();
    
    // Create appointment details
    AppointmentDetails details;
    details.appointmentId = outAppointmentId;
    details.patientId = patientId;
    details.doctorId = doctorId;
    details.date = date;
    details.time = time;
    details.reason = reason;
    details.status = "Scheduled";
    
    // Write appointment
    return writeAppointment(outAppointmentId, details);
}

std::vector<std::string> DataStore::listAppointmentsForPatient(const std::string& patientId) {
    return getPatientAppointments(patientId);
}

std::vector<std::string> DataStore::listAppointmentsForDoctor(const std::string& doctorId) {
    return getDoctorAppointments(doctorId);
}