#include "PatientService.h"
#include "DataStore.h"

namespace PatientService {

std::vector<std::string> getPatientIDs() {
    return DataStore::listIDs("Patient");
}

bool bookAppointment(const std::string& patientId, const std::string& doctorId, const std::string& date, const std::string& time, const std::string& reason, std::string& outAppointmentId) {
    return DataStore::appendAppointment(patientId, doctorId, date, time, reason, outAppointmentId);
}

std::vector<DataStore::AppointmentDetails> getAppointments(const std::string& patientId) {
    std::vector<DataStore::AppointmentDetails> out;
    auto files = DataStore::listAppointmentsForPatient(patientId);
    for (auto &f : files) {
        auto d = DataStore::readAppointment(f);
        if (d) out.push_back(*d);
    }
    return out;
}

} // namespace PatientService
