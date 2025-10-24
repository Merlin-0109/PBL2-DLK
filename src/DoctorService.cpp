// ============================================
// File: src/DoctorService.cpp
// ============================================
#include "DoctorService.h"
#include "DataStore.h"

namespace DoctorService {

// Get list of all doctor IDs in the system
std::vector<std::string> getDoctorIDs() {
    return DataStore::listIDs("Doctor");
}

// Get all appointments for a specific doctor
std::vector<DataStore::AppointmentDetails> getAppointments(const std::string& doctorId) {
    std::vector<DataStore::AppointmentDetails> appointments;
    
    // Get list of appointment IDs for this doctor
    auto appointmentIds = DataStore::listAppointmentsForDoctor(doctorId);
    
    // Read details for each appointment
    for (const auto& apptId : appointmentIds) {
        auto details = DataStore::readAppointment(apptId);
        
        // Only add if appointment details were successfully read
        if (!details.appointmentId.empty()) {
            appointments.push_back(details);
        }
    }
    
    return appointments;
}

// Update appointment status (e.g., "Completed", "Cancelled", "Rescheduled")
bool updateAppointmentStatus(const std::string& appointmentId, const std::string& newStatus) {
    return DataStore::updateAppointmentStatus(appointmentId, newStatus);
}

// Get doctor information by ID
DataStore::DoctorInfo getDoctorInfo(const std::string& doctorId) {
    return DataStore::readDoctorInfo(doctorId);
}

// Update doctor information
bool updateDoctorInfo(const std::string& doctorId, const DataStore::DoctorInfo& info) {
    return DataStore::writeDoctorInfo(doctorId, info);
}

// Check if doctor exists in system
bool doctorExists(const std::string& doctorId) {
    return DataStore::doctorExists(doctorId);
}

} // namespace DoctorService