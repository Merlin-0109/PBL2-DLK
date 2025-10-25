// ============================================
// File: include/DoctorService.h
// ============================================
#pragma once
#include <string>
#include <vector>
#include "DataStore.h"

namespace DoctorService {
    
    // Get list of all doctor IDs in the system
    std::vector<std::string> getDoctorIDs();
    
    // Get all appointments for a specific doctor
    // Returns: Vector of appointment details for the specified doctor
    std::vector<DataStore::AppointmentDetails> getAppointments(const std::string& doctorId);
    
    bool updateAppointmentStatus(const std::string& appointmentId, 
                                const std::string& newStatus);
    
    // Get doctor information by ID
    DataStore::DoctorInfo getDoctorInfo(const std::string& doctorId);
    
    // Update doctor information
    bool updateDoctorInfo(const std::string& doctorId, const DataStore::DoctorInfo& info);
    
    // Check if doctor exists in system
    bool doctorExists(const std::string& doctorId);
}