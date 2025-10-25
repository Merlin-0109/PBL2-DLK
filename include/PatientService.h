#pragma once
#include <string>
#include <vector>
#include "DataStore.h"

namespace PatientService {
    
    // Get list of all patient IDs in the system
    std::vector<std::string> getPatientIDs();

    bool bookAppointment(const std::string& patientId, 
                        const std::string& doctorId,
                        const std::string& date, 
                        const std::string& time,
                        const std::string& reason, 
                        std::string& outAppointmentId);
    
    // Get all appointments for a specific patient
    std::vector<DataStore::AppointmentDetails> getAppointments(const std::string& patientId);
    
    // Cancel an appointment
    bool cancelAppointment(const std::string& appointmentId);
    
    // Get patient information by ID
    DataStore::PatientInfo getPatientInfo(const std::string& patientId);
    
    // Update patient information
    bool updatePatientInfo(const std::string& patientId, const DataStore::PatientInfo& info);
    
    // Check if patient exists in system
    bool patientExists(const std::string& patientId);
    
    // Reschedule an appointment
    bool rescheduleAppointment(const std::string& appointmentId, 
                              const std::string& newDate, 
                              const std::string& newTime);
}