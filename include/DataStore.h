#pragma once
#include <string>
#include <vector>
#include <optional>
#include <utility>

namespace DataStore {
    struct UserInfo {
        std::string name;
        std::string phone;
        std::string email;
        std::string dob;
        std::string gender;
        std::string address;
        std::string cccd;
    };
    
    struct DoctorInfo : public UserInfo {
        std::string specialization;
        std::string faculty;
    };
    
    struct PatientInfo : public UserInfo {
        // Add any patient-specific fields here if needed
    };

    struct AppointmentDetails {
        std::string filename; // filename in data/appointments
        std::string patientId;
        std::string doctorId;
        std::string date;
        std::string time;
        std::string reason;
    };

    std::vector<std::string> listIDs(const std::string& role);
    // returns pair<username,password>
    std::pair<std::string,std::string> readUser(const std::string& role, const std::string& id);
    bool appendAppointment(const std::string& patientId, const std::string& doctorId, const std::string& date, const std::string& time, const std::string& reason, std::string& outAppointmentId);
    std::vector<std::string> listAppointmentsForPatient(const std::string& patientId);
    // read details for appointment file name (filename with extension)
    std::optional<AppointmentDetails> readAppointment(const std::string& filename);
    void ensureAppointmentsDirExists();
    
    // Doctor information methods
    DoctorInfo readDoctorInfo(const std::string& doctorId);
    bool writeDoctorInfo(const std::string& doctorId, const DoctorInfo& info);
    
    // Patient information methods
    PatientInfo readPatientInfo(const std::string& patientId);
    bool writePatientInfo(const std::string& patientId, const PatientInfo& info);
}
