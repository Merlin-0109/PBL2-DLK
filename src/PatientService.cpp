// ============================================
// File: src/PatientService.cpp
// ============================================
#include "PatientService.h"
#include "DataStore.h"

namespace PatientService {

// Get list of all patient IDs in the system
std::vector<std::string> getPatientIDs() {
    return DataStore::listIDs("Patient");
}

// Book a new appointment
bool bookAppointment(const std::string& patientId, 
                     const std::string& doctorId,
                     const std::string& date, 
                     const std::string& time,
                     const std::string& reason, 
                     std::string& outAppointmentId) {
    // Validate inputs
    if (patientId.empty() || doctorId.empty() || date.empty() || time.empty() || reason.empty()) {
        return false;
    }
    
    // Check if patient and doctor exist
    if (!DataStore::patientExists(patientId) || !DataStore::doctorExists(doctorId)) {
        return false;
    }
    
    // Book the appointment using DataStore
    return DataStore::appendAppointment(patientId, doctorId, date, time, reason, outAppointmentId);
}

// Get all appointments for a specific patient
std::vector<DataStore::AppointmentDetails> getAppointments(const std::string& patientId) {
    std::vector<DataStore::AppointmentDetails> appointments;
    
    // Get list of appointment IDs for this patient
    auto appointmentIds = DataStore::listAppointmentsForPatient(patientId);
    
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

// Cancel an appointment
bool cancelAppointment(const std::string& appointmentId) {
    // Update the appointment status to "Cancelled"
    return DataStore::updateAppointmentStatus(appointmentId, "Cancelled");
}

// Get patient information by ID
DataStore::PatientInfo getPatientInfo(const std::string& patientId) {
    return DataStore::readPatientInfo(patientId);
}

// Update patient information
bool updatePatientInfo(const std::string& patientId, const DataStore::PatientInfo& info) {
    return DataStore::writePatientInfo(patientId, info);
}

// Check if patient exists in system
bool patientExists(const std::string& patientId) {
    return DataStore::patientExists(patientId);
}

// Reschedule an appointment
bool rescheduleAppointment(const std::string& appointmentId, 
                          const std::string& newDate, 
                          const std::string& newTime) {
    // Read current appointment details
    auto details = DataStore::readAppointment(appointmentId);
    
    if (details.appointmentId.empty()) {
        return false; // Appointment not found
    }
    
    // Update date and time
    details.date = newDate;
    details.time = newTime;
    details.status = "Rescheduled";
    
    // Write updated appointment
    return DataStore::writeAppointment(appointmentId, details);
}

} // namespace PatientService