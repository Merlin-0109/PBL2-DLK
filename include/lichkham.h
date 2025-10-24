#pragma once
#include <iostream>
#include <string>
#include "Doctor.h"

class LichKham {
    protected:
        std::string MaLichKham;
        std::string NgayKham;
        std::string GioKham;
        Doctor Bacsi;
        std::string TrieuChung;
        std::string LydoKham;
        std::string TrangThai; // e.g., "Da dat", "Da huy", "Da ket thuc"
    public:
        LichKham();
        LichKham(std::string maLichKham, std::string ngayKham, std::string gioKham, std::string trangThai, std::string trieuChung, std::string lydoKham, Doctor bacsi);
        ~LichKham();
        
        void showLichKham();
        void setTrangThai(std::string trangThai);
        void setNgayKham(std::string ngayKham);
        void setGioKham(std::string gioKham);
        void setTrieuChung(std::string trieuChung);
        void setLydoKham(std::string lydoKham);
        void setBacsi(Doctor bacsi);
        friend std::istream& operator>>(std::istream& is, LichKham& lichKham);
        
};