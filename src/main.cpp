#include <iostream>
#include <string>
#include "Account.h"
#include "DoctorService.h"
#include "PatientService.h"

void showMenu() {
	std::cout << "================================" << std::endl;
	std::cout << "  Datchikham - Demo Menu" << std::endl;
	std::cout << "================================" << std::endl;
	std::cout << "1) Register (demo)" << std::endl;
	std::cout << "2) Login (demo)" << std::endl;
	std::cout << "3) About" << std::endl;
	std::cout << "0) Exit" << std::endl;
	std::cout << "--------------------------------" << std::endl;
	std::cout << "Choose an option: ";
}

int main() {
	std::cout << "Datchikham starting..." << std::endl;
	// ensure default admin exists before any register/login
	Account::ensureDefaultAdminExists();
	DataStore::ensureAppointmentsDirExists();

	while (true) {
		showMenu();
		std::string choice;
		if (!std::getline(std::cin, choice)) break; // EOF or error

		if (choice == "1") {
			Account a;
			a.registerAccount();
			std::cout << "(Account created)" << std::endl;
		} else if (choice == "2") {
			Account a;
			std::string role, id;
			if (a.login(role, id)) {
				// logged in submenu
				bool logged = true;
				while (logged) {
					std::cout << "\nLogged in as " << role << " (ID=" << id << ")" << std::endl;
					if (role == "Doctor") {
						std::cout << "1) View appointments" << std::endl;
						std::cout << "2) Update profile" << std::endl;
						std::cout << "3) Logout" << std::endl;
					} else if (role == "Patient") {
						std::cout << "1) Book appointment" << std::endl;
						std::cout << "2) View my appointments" << std::endl;
						std::cout << "3) Update profile" << std::endl;
						std::cout << "4) Logout" << std::endl;
					} else {
						std::cout << "1) View profile" << std::endl;
						std::cout << "2) Logout" << std::endl;
					}
					std::cout << "Choose: ";
					std::string s; std::getline(std::cin >> std::ws, s);
					if (role == "Doctor") {
						if (s == "1") {
							auto appts = DoctorService::getAppointments(id);
							std::cout << "Appointments for " << id << ":\n";
							for (auto &p : appts) {
								std::cout << " - file=" << p.filename << " patient=" << p.patientId << " date=" << p.date << " time=" << p.time << " reason=" << p.reason << "\n";
							}
						} else if (s == "2") {
							// Update doctor profile
							DataStore::DoctorInfo info = DataStore::readDoctorInfo(id);
							std::cout << "\nUpdate Profile Information\n";
							std::cout << "Current values shown in [brackets]. Press Enter to keep current value.\n\n";
							
							std::string input;
							std::cout << "Name [" << info.name << "]: ";
							std::getline(std::cin >> std::ws, input);
							if (!input.empty()) info.name = input;
							
							std::cout << "Specialization [" << info.specialization << "]: ";
							std::getline(std::cin >> std::ws, input);
							if (!input.empty()) info.specialization = input;
							
							std::cout << "Faculty [" << info.faculty << "]: ";
							std::getline(std::cin >> std::ws, input);
							if (!input.empty()) info.faculty = input;
							
							std::cout << "Phone [" << info.phone << "]: ";
							std::getline(std::cin >> std::ws, input);
							if (!input.empty()) info.phone = input;
							
							std::cout << "Email [" << info.email << "]: ";
							std::getline(std::cin >> std::ws, input);
							if (!input.empty()) info.email = input;
							
							std::cout << "Date of Birth [" << info.dob << "]: ";
							std::getline(std::cin >> std::ws, input);
							if (!input.empty()) info.dob = input;
							
							std::cout << "Gender [" << info.gender << "]: ";
							std::getline(std::cin >> std::ws, input);
							if (!input.empty()) info.gender = input;
							
							std::cout << "Address [" << info.address << "]: ";
							std::getline(std::cin >> std::ws, input);
							if (!input.empty()) info.address = input;
							
							std::cout << "CCCD [" << info.cccd << "]: ";
							std::getline(std::cin >> std::ws, input);
							if (!input.empty()) info.cccd = input;
							
							if (DataStore::writeDoctorInfo(id, info)) {
								std::cout << "Profile updated successfully.\n";
							} else {
								std::cout << "Error updating profile.\n";
							}
						} else if (s == "3") {
							a.logout(); logged = false;
						} else std::cout << "Unknown option" << std::endl;
					} else if (role == "Patient") {
						if (s == "1") {
							// book
							auto docs = DoctorService::getDoctorIDs();
							if (docs.empty()) { std::cout << "No doctors available." << std::endl; }
							else {
								std::cout << "\nAvailable doctors:\n";
								// Get doctor details first
								std::vector<std::pair<std::string,std::string>> doctorDetails;
								for (const auto& docId : docs) {
									auto user = DataStore::readUser("Doctor", docId);
									doctorDetails.push_back({docId, user.first}); // pair of ID and name
								}
								
								// Show doctor list with names
								for (size_t i=0;i<doctorDetails.size();++i) {
									std::cout << i+1 << ") Dr. " << doctorDetails[i].second 
									         << " (ID: " << doctorDetails[i].first << ")\n";
								}
								
								// Get doctor selection
								std::cout << "\nEnter doctor number for more details (or 0 to cancel): ";
								std::string sel; std::getline(std::cin >> std::ws, sel);
								size_t idx = 0;
								try {
									idx = std::stoul(sel);
								} catch(...) {
									std::cout << "Invalid input" << std::endl;
									continue;
								}
								
								if (idx == 0) {
									std::cout << "Booking cancelled." << std::endl;
									continue;
								}
								if (idx > doctorDetails.size()) {
									std::cout << "Invalid selection" << std::endl;
									continue;
								}
								
								// Show selected doctor details
								auto docId = doctorDetails[idx-1].first;
								std::cout << "\nDoctor Details:\n";
								std::cout << "Name: Dr. " << doctorDetails[idx-1].second << "\n";
								auto docInfo = DataStore::readDoctorInfo(docId);
								if (!docInfo.specialization.empty()) {
									std::cout << "Specialization: " << docInfo.specialization << "\n";
									std::cout << "Faculty: " << docInfo.faculty << "\n";
									std::cout << "Phone: " << docInfo.phone << "\n";
									std::cout << "Email: " << docInfo.email << "\n";
								}
								
								// Confirm booking
								std::cout << "\nConfirm booking with this doctor? (y/n): ";
								std::string confirm;
								std::getline(std::cin >> std::ws, confirm);
								if (confirm != "y" && confirm != "Y") {
									std::cout << "Booking cancelled." << std::endl;
									continue;
								}
								else {
									std::string date, time, reason, apptId;
									std::cout << "Enter date (YYYY-MM-DD): "; std::getline(std::cin,date);
									std::cout << "Enter time (HH:MM): "; std::getline(std::cin,time);
									std::cout << "Enter reason: "; std::getline(std::cin,reason);
									if (PatientService::bookAppointment(id, docs[idx-1], date, time, reason, apptId))
										std::cout << "Appointment created: " << apptId << std::endl;
									else std::cout << "Failed to create appointment" << std::endl;
								}
							}
						} else if (s == "2") {
							auto appts = PatientService::getAppointments(id);
							std::cout << "Your appointments:\n";
							for (auto &p : appts) {
								std::cout << " - file=" << p.filename << " doctor=" << p.doctorId << " date=" << p.date << " time=" << p.time << " reason=" << p.reason << "\n";
							}
						} else if (s == "3") {
							// Update patient profile
							DataStore::PatientInfo info = DataStore::readPatientInfo(id);
							std::cout << "\nUpdate Profile Information\n";
							std::cout << "Current values shown in [brackets]. Press Enter to keep current value.\n\n";
							
							std::string input;
							std::cout << "Name [" << info.name << "]: ";
							std::getline(std::cin >> std::ws, input);
							if (!input.empty()) info.name = input;
							
							std::cout << "Phone [" << info.phone << "]: ";
							std::getline(std::cin >> std::ws, input);
							if (!input.empty()) info.phone = input;
							
							std::cout << "Email [" << info.email << "]: ";
							std::getline(std::cin >> std::ws, input);
							if (!input.empty()) info.email = input;
							
							std::cout << "Date of Birth [" << info.dob << "]: ";
							std::getline(std::cin >> std::ws, input);
							if (!input.empty()) info.dob = input;
							
							std::cout << "Gender [" << info.gender << "]: ";
							std::getline(std::cin >> std::ws, input);
							if (!input.empty()) info.gender = input;
							
							std::cout << "Address [" << info.address << "]: ";
							std::getline(std::cin >> std::ws, input);
							if (!input.empty()) info.address = input;
							
							std::cout << "CCCD [" << info.cccd << "]: ";
							std::getline(std::cin >> std::ws, input);
							if (!input.empty()) info.cccd = input;
							
							if (DataStore::writePatientInfo(id, info)) {
								std::cout << "Profile updated successfully.\n";
							} else {
								std::cout << "Error updating profile.\n";
							}
						} else if (s == "4") { 
							a.logout(); logged = false; 
						} else std::cout << "Unknown option" << std::endl;
					} else {
						if (s == "1") {
							std::cout << "Username: " << a.getUsername() << std::endl;
							std::cout << "ID: " << a.getID() << std::endl;
						} else if (s == "2") { a.logout(); logged = false; }
						else std::cout << "Unknown option" << std::endl;
					}
				}
			}
		} else if (choice == "3") {
			std::cout << "Datchikham demo app. Built to demonstrate startup and menu." << std::endl;
			std::cout << "Stored appointments (quick check):\n";
			for (auto &fn : std::filesystem::directory_iterator("data/appointments")) {
				if (!fn.is_regular_file()) continue;
				auto name = fn.path().filename().string();
				// only consider .txt appointment files
				if (name.size() < 5 || name.substr(name.size()-4) != ".txt") {
					std::cout << " - skipping non-txt file: " << name << "\n";
					continue;
				}
				auto d = DataStore::readAppointment(name);
				if (d) std::cout << " - " << d->filename << " patient=" << d->patientId << " doctor=" << d->doctorId << " date=" << d->date << " time=" << d->time << " reason=" << d->reason << "\n";
				else std::cout << " - skipped empty/invalid appointment file: " << name << "\n";
			}
		} else if (choice == "0") {
			std::cout << "Exiting..." << std::endl;
			break;
		} else {
			std::cout << "Unknown option: '" << choice << "'" << std::endl;
		}

		std::cout << "\n(Press Enter to continue)" << std::endl;
		std::string tmp; std::getline(std::cin, tmp);
	}

	return 0;
}

