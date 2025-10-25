#pragma once
#include <iostream>
#include <string>
#include "Account.h"

class User {
    protected:
        std::string ID,Name,SDT,Email,Address,DOB,Gender,Nhommau,CCCD;
        Account acc;
        static std::string role;
    public:
        User();
        User(std::string id, std::string name, std::string sdt, std::string email, std::string address, std::string dob, std::string gender, std::string nhommau, std::string cccd, Account acc);
        virtual ~User();
        void showInfo();
        void setName(std::string name);
        void setSDT(std::string sdt);
        void setEmail(std::string email);
        void setAddress(std::string address);
        void setDOB(std::string dob);
        void setGender(std::string gender);
        void setNhommau(std::string nhommau);
        void setCCCD(std::string cccd);
        virtual void setID();
};