// DataStore.cpp - Complete implementation
#include "DataStore.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <cctype>

namespace fs = std::filesystem;

// Initialize static members for inverted index
std::map<std::string, std::set<std::string>> DataStore::doctorInvertedIndex;
bool DataStore::indexBuilt = false;


// Ensure appointments directory exists
void DataStore::ensureAppointmentsDirExists() {
    fs::create_directories("data/appointments");
    fs::create_directories("data/Patient");
    fs::create_directories("data/Doctor");
    fs::create_directories("data/Admin");
    fs::create_directories("data/notifications");
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
    file << "cancelReason:" << details.cancelReason << "\n";
    
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
            else if (key == "cancelReason") details.cancelReason = value;
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
    // Overwrite appointment file without appending to patient/doctor lists
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
    file << "cancelReason:" << details.cancelReason << "\n";
    file.close();
    return true;
}

bool DataStore::updateAppointmentWithCancelReason(const std::string& appointmentId, const std::string& cancelReason) {
    auto details = readAppointment(appointmentId);
    if (details.appointmentId.empty()) {
        return false;
    }
    details.status = "Cancelled";
    details.cancelReason = cancelReason;
    
    // Update appointment file directly without appending to lists
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
    file << "cancelReason:" << details.cancelReason << "\n";
    file.close();
    
    return true;
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

// ============================================
// Notifications
// ============================================
void DataStore::appendNotification(const std::string& userId, const std::string& message) {
    fs::create_directories("data/notifications");
    std::string filepath = "data/notifications/" + userId + ".txt";
    std::ofstream file(filepath, std::ios::app);
    if (!file.is_open()) return;
    auto now = std::time(nullptr);
    std::tm tm = *std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M");
    file << oss.str() << " - " << message << "\n";
}

std::vector<std::string> DataStore::readNotifications(const std::string& userId) {
    std::vector<std::string> lines;
    std::string filepath = "data/notifications/" + userId + ".txt";
    std::ifstream file(filepath);
    if (!file.is_open()) return lines;
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) lines.push_back(line);
    }
    return lines;
}
// ============================================
// Inverted Index for Doctor Search
// ============================================

// Normalize word: convert to lowercase
std::string DataStore::normalize(const std::string& word) {
    std::string normalized = word;
    std::transform(normalized.begin(), normalized.end(), normalized.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return normalized;
}

// Tokenize text into words (split by spaces and special characters)
std::vector<std::string> DataStore::tokenize(const std::string& text) {
    std::vector<std::string> tokens;
    std::string current;
    
    for (char c : text) {
        if (std::isalnum(static_cast<unsigned char>(c)) || c < 0) { // Allow UTF-8 characters
            current += c;
        } else {
            if (!current.empty()) {
                tokens.push_back(normalize(current));
                current.clear();
            }
        }
    }
    
    if (!current.empty()) {
        tokens.push_back(normalize(current));
    }
    
    return tokens;
}

// Build inverted index from all doctors
void DataStore::buildDoctorInvertedIndex() {
    doctorInvertedIndex.clear();
    
    auto allDoctorIds = listIDs("Doctor");
    
    for (const auto& doctorId : allDoctorIds) {
        auto info = readDoctorInfo(doctorId);
        
        // Tokenize doctor name and specialization
        std::string searchableText = info.name + " " + info.specialization + " " + doctorId;
        auto tokens = tokenize(searchableText);
        
        // Add each token to inverted index
        for (const auto& token : tokens) {
            doctorInvertedIndex[token].insert(doctorId);
        }
    }
    
    indexBuilt = true;
}

// Search doctors using inverted index
std::vector<std::string> DataStore::searchDoctorsByInvertedIndex(const std::string& query) {
    // Build index if not already built
    if (!indexBuilt) {
        buildDoctorInvertedIndex();
    }
    
    // If query is empty, return all doctors
    if (query.empty()) {
        return listIDs("Doctor");
    }
    
    // Tokenize query
    auto queryTokens = tokenize(query);
    
    if (queryTokens.empty()) {
        return listIDs("Doctor");
    }
    
    // Find doctors that match ALL query tokens (AND search)
    std::set<std::string> result;
    bool firstToken = true;
    
    for (const auto& token : queryTokens) {
        auto it = doctorInvertedIndex.find(token);
        
        if (it == doctorInvertedIndex.end()) {
            // Token not found, no results
            result.clear();
            break;
        }
        
        if (firstToken) {
            result = it->second;
            firstToken = false;
        } else {
            // Intersection with previous results
            std::set<std::string> intersection;
            std::set_intersection(result.begin(), result.end(),
                                it->second.begin(), it->second.end(),
                                std::inserter(intersection, intersection.begin()));
            result = intersection;
        }
        
        if (result.empty()) {
            break;
        }
    }
    
    // Convert set to vector
    return std::vector<std::string>(result.begin(), result.end());
}

// ============================================
// Medical Records (Lịch sử khám bệnh)
// ============================================

// Generate unique medical record ID
std::string DataStore::generateMedicalRecordId() {
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    std::ostringstream oss;
    oss << "MR" << std::put_time(&tm, "%Y%m%d%H%M%S");
    return oss.str();
}

// Add a medical record to patient's history
bool DataStore::addMedicalRecord(const std::string& patientId, const MedicalRecord& record) {
    std::string filepath = "data/Patient/" + patientId + "_medical_history.txt";
    std::ofstream file(filepath, std::ios::app);
    
    if (!file.is_open()) {
        return false;
    }
    
    file << "---RECORD_START---\n";
    file << "recordId:" << record.recordId << "\n";
    file << "patientId:" << record.patientId << "\n";
    file << "doctorId:" << record.doctorId << "\n";
    file << "visitDate:" << record.visitDate << "\n";
    file << "diagnosis:" << record.diagnosis << "\n";
    file << "symptoms:" << record.symptoms << "\n";
    file << "prescription:" << record.prescription << "\n";
    file << "notes:" << record.notes << "\n";
    file << "followUpDate:" << record.followUpDate << "\n";
    file << "---RECORD_END---\n";
    
    file.close();
    return true;
}

// Get all medical records for a patient
std::vector<DataStore::MedicalRecord> DataStore::getMedicalHistory(const std::string& patientId) {
    std::vector<MedicalRecord> records;
    std::string filepath = "data/Patient/" + patientId + "_medical_history.txt";
    std::ifstream file(filepath);
    
    if (!file.is_open()) {
        return records; // Return empty vector if file doesn't exist
    }
    
    std::string line;
    MedicalRecord currentRecord;
    bool inRecord = false;
    
    while (std::getline(file, line)) {
        if (line == "---RECORD_START---") {
            inRecord = true;
            currentRecord = MedicalRecord(); // Reset
        } else if (line == "---RECORD_END---") {
            if (inRecord) {
                records.push_back(currentRecord);
            }
            inRecord = false;
        } else if (inRecord) {
            size_t pos = line.find(':');
            if (pos != std::string::npos) {
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 1);
                
                if (key == "recordId") currentRecord.recordId = value;
                else if (key == "patientId") currentRecord.patientId = value;
                else if (key == "doctorId") currentRecord.doctorId = value;
                else if (key == "visitDate") currentRecord.visitDate = value;
                else if (key == "diagnosis") currentRecord.diagnosis = value;
                else if (key == "symptoms") currentRecord.symptoms = value;
                else if (key == "prescription") currentRecord.prescription = value;
                else if (key == "notes") currentRecord.notes = value;
                else if (key == "followUpDate") currentRecord.followUpDate = value;
            }
        }
    }
    
    file.close();
    return records;
}

// Read a specific medical record
DataStore::MedicalRecord DataStore::readMedicalRecord(const std::string& patientId, const std::string& recordId) {
    auto records = getMedicalHistory(patientId);
    
    for (const auto& record : records) {
        if (record.recordId == recordId) {
            return record;
        }
    }
    
    return MedicalRecord(); // Return empty record if not found
}

// Update a medical record
bool DataStore::updateMedicalRecord(const std::string& patientId, const MedicalRecord& updatedRecord) {
    auto records = getMedicalHistory(patientId);
    
    // Find and update the record
    bool found = false;
    for (auto& record : records) {
        if (record.recordId == updatedRecord.recordId) {
            record = updatedRecord;
            found = true;
            break;
        }
    }
    
    if (!found) {
        return false;
    }
    
    // Rewrite the entire file with updated records
    std::string filepath = "data/Patient/" + patientId + "_medical_history.txt";
    std::ofstream file(filepath);
    
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& record : records) {
        file << "---RECORD_START---\n";
        file << "recordId:" << record.recordId << "\n";
        file << "patientId:" << record.patientId << "\n";
        file << "doctorId:" << record.doctorId << "\n";
        file << "visitDate:" << record.visitDate << "\n";
        file << "diagnosis:" << record.diagnosis << "\n";
        file << "symptoms:" << record.symptoms << "\n";
        file << "prescription:" << record.prescription << "\n";
        file << "notes:" << record.notes << "\n";
        file << "followUpDate:" << record.followUpDate << "\n";
        file << "---RECORD_END---\n";
    }
    
    file.close();
    return true;
}
