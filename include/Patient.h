#pragma once
#include "User.h"
#include "lichkham.h"


class Patient : public User {
    protected:
        LichKham myLichKham; 
        static int count;
    public:
        Patient();
        Patient(std::string id, std::string name, std::string sdt, std::string email, std::string address, std::string dob, std::string gender, std::string nhommau, std::string cccd, Account acc);
        ~Patient();
        void showInfo();
        void chinhsuathongtin();
        void datlichkham();
        void danhgia();
        void huylichkham();
        void setID() override;
};
