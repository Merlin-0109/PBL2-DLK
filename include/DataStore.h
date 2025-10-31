// DataStore.h - Fixed version with all required functions
#pragma once
#include <string>
#include <vector>
#include <map>
#include <set>

class DataStore {
public:
    // Struct for Patient Information
    struct PatientInfo {
        std::string name;
        std::string phone;
        std::string email;
        std::string address;
        std::string dateOfBirth;
        std::string gender;
    };

    // Struct for Doctor Information
    struct DoctorInfo {
        std::string name;
        std::string phone;
        std::string email;
        std::string specialization;
        std::string experience;
    };

    // Struct for Appointment Details - COMPLETE VERSION
    struct AppointmentDetails {
        std::string appointmentId;
        std::string patientId;
        std::string doctorId;
        std::string date;
        std::string time;
        std::string reason;
        std::string status;
        std::string cancelReason;  // Reason for cancellation
    };

    // Struct for Medical Record (Lịch sử khám bệnh)
    struct MedicalRecord {
        std::string recordId;           // ID của bản ghi
        std::string patientId;          // ID bệnh nhân
        std::string doctorId;           // ID bác sĩ khám
        std::string visitDate;          // Ngày khám (DD/MM/YYYY)
        std::string diagnosis;          // Chẩn đoán
        std::string symptoms;           // Triệu chứng
        std::string prescription;       // Đơn thuốc
        std::string notes;              // Ghi chú thêm
        std::string followUpDate;       // Ngày tái khám (nếu có)
    };

    // Patient functions
    static bool writePatientInfo(const std::string& patientId, const PatientInfo& info);
    static PatientInfo readPatientInfo(const std::string& patientId);
    static bool patientExists(const std::string& patientId);

    // Doctor functions
    static bool writeDoctorInfo(const std::string& doctorId, const DoctorInfo& info);
    static DoctorInfo readDoctorInfo(const std::string& doctorId);
    static bool doctorExists(const std::string& doctorId);

    // Appointment functions
    static bool writeAppointment(const std::string& appointmentId, const AppointmentDetails& details);
    static AppointmentDetails readAppointment(const std::string& appointmentId);
    static std::vector<std::string> getPatientAppointments(const std::string& patientId);
    static std::vector<std::string> getDoctorAppointments(const std::string& doctorId);
    static bool updateAppointmentStatus(const std::string& appointmentId, const std::string& newStatus);
    static bool updateAppointmentWithCancelReason(const std::string& appointmentId, const std::string& cancelReason);
    static bool deleteAppointment(const std::string& appointmentId);
    
    // New functions for Services
    static std::vector<std::string> listIDs(const std::string& role);
    static bool appendAppointment(const std::string& patientId, const std::string& doctorId, 
                                  const std::string& date, const std::string& time, 
                                  const std::string& reason, std::string& outAppointmentId);
    static std::vector<std::string> listAppointmentsForPatient(const std::string& patientId);
    static std::vector<std::string> listAppointmentsForDoctor(const std::string& doctorId);

    // Utility functions
    static void ensureAppointmentsDirExists();
    static std::string generateAppointmentId();

    // Notifications
    static void appendNotification(const std::string& userId, const std::string& message);
    static std::vector<std::string> readNotifications(const std::string& userId);
    
    // Medical Records (Lịch sử khám bệnh)
    static bool addMedicalRecord(const std::string& patientId, const MedicalRecord& record);
    static std::vector<MedicalRecord> getMedicalHistory(const std::string& patientId);
    static MedicalRecord readMedicalRecord(const std::string& patientId, const std::string& recordId);
    static bool updateMedicalRecord(const std::string& patientId, const MedicalRecord& record);
    static std::string generateMedicalRecordId();
    
    // Inverted Index for Doctor Search
    static std::vector<std::string> searchDoctorsByInvertedIndex(const std::string& query);
    static void buildDoctorInvertedIndex();
    
private:
    // Inverted index: word -> set of doctor IDs
    static std::map<std::string, std::set<std::string>> doctorInvertedIndex;
    static bool indexBuilt;
    
    // Helper to tokenize and normalize text
    static std::vector<std::string> tokenize(const std::string& text);
    static std::string normalize(const std::string& word);
};