#pragma once
#include <string>
#include <vector>
#include "DataStore.h"

namespace PatientService {
    std::vector<std::string> getPatientIDs();
    bool bookAppointment(const std::string& patientId, const std::string& doctorId, const std::string& date, const std::string& time, const std::string& reason, std::string& outAppointmentId);
    std::vector<DataStore::AppointmentDetails> getAppointments(const std::string& patientId);
}
