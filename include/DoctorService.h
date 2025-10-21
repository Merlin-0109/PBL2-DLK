#pragma once
#include <string>
#include <vector>
#include "DataStore.h"

namespace DoctorService {
    std::vector<std::string> getDoctorIDs();
    std::vector<DataStore::AppointmentDetails> getAppointments(const std::string& doctorId);
}
