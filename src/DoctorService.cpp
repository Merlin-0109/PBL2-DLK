#include "DoctorService.h"
#include "DataStore.h"
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

namespace DoctorService {

std::vector<std::string> getDoctorIDs() {
    return DataStore::listIDs("Doctor");
}

std::vector<DataStore::AppointmentDetails> getAppointments(const std::string& doctorId) {
    std::vector<DataStore::AppointmentDetails> out;
    for (auto &fn : fs::directory_iterator("data/appointments")) {
        if (!fn.is_regular_file()) continue;
        auto details = DataStore::readAppointment(fn.path().filename().string());
        if (details && details->doctorId == doctorId) out.push_back(*details);
    }
    return out;
}

} // namespace DoctorService
