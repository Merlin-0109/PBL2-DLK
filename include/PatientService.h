// ============================================
// File: include/PatientService.h
// ============================================
#pragma once
#include <string>
#include <vector>
#include "DataStore.h"

namespace PatientService {
    
    // Get list of all patient IDs in the system
    std::vector<std::string> getPatientIDs();
    
    // Book a new appointment
    // Parameters:
    //   - patientId: ID of the patient booking the appointment
    //   - doctorId: ID of the doctor to see
    //   - date: Date of appointment (format: YYYY-MM-DD)
    //   - time: Time of appointment (format: HH:MM)
    //   - reason: Reason for the appointment
    //   - outAppointmentId: [OUT] Generated appointment ID
    // Returns: true if successful, false otherwise
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