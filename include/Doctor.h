#pragma once
#include <iostream>
#include "User.h"

using namespace std;

class Doctor: public User{
    protected:
        string doctorID;
        string specialization;
        string doctorRole;
        string faculty;
        static int doctorCount; 
    public:
        Doctor();
        Doctor(string id, string name, string sdt, string email, string address, 
            string dob, string gender, string cccd, string specialization, 
            string faculty, Account acc);
        ~Doctor();

        // Information access
        string getName() const { return Name; }
        string getSDT() const { return SDT; }
        string getEmail() const { return Email; }
        string getAddress() const { return Address; }
        string getDOB() const { return DOB; }
        string getGender() const { return Gender; }
        string getCCCD() const { return CCCD; }
        string getSpecialization() const { return specialization; }
        string getDoctorRole() const { return doctorRole; }
        string getFaculty() const { return faculty; }

        // Update methods
        void setSpecialization(string specialization);
        void setDoctorRole(string doctorRole);
        void setFaculty(string faculty);
        
        // Update all information at once
        void updateInformation(const string& name, const string& sdt, 
            const string& email, const string& address, const string& dob,
            const string& gender, const string& cccd, const string& specialization,
            const string& faculty);

        friend ostream& operator<<(ostream&, const Doctor&);
        // friend istream& operator>>(istream&, Doctor&);

        void findPatient(); // xây dựng class find riêng & rating - bac si
        void viewAppointment(); // myTools.viewAppointment();
        void markAsBusy(); // tương tự
};

// chung mot ham find => hash (băm trong băm)
// 