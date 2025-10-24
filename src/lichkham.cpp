#include "lichkham.h"

LichKham::LichKham() {
    MaLichKham = "";
    NgayKham = "";
    GioKham = "";
    TrangThai = "";
    TrieuChung = "";
    LydoKham = "";
}

LichKham::LichKham(std::string maLichKham, std::string ngayKham, std::string gioKham, std::string trangThai, std::string trieuChung, std::string lydoKham, Doctor bacsi) {
    MaLichKham = maLichKham;
    NgayKham = ngayKham;
    GioKham = gioKham;
    TrangThai = trangThai;
    TrieuChung = trieuChung;
    LydoKham = lydoKham;
    Bacsi = bacsi;
}

LichKham::~LichKham() {}

void LichKham::showLichKham() {
    std::cout << "Ma Lich Kham: " << MaLichKham << std::endl;
    std::cout << "Ngay Kham: " << NgayKham << std::endl;
    std::cout << "Gio Kham: " << GioKham << std::endl;
    std::cout << "Trang Thai: " << TrangThai << std::endl;
    std::cout << "Trieu Chung: " << TrieuChung << std::endl;
    std::cout << "Ly do Kham: " << LydoKham << std::endl;
    std::cout << "Bac si: " << Bacsi << std::endl; 
}

void LichKham::setTrangThai(std::string trangThai) {
    TrangThai = trangThai;
}

void LichKham::setNgayKham(std::string ngayKham) {
    NgayKham = ngayKham;
}
void LichKham::setGioKham(std::string gioKham) {
    GioKham = gioKham;
}
void LichKham::setTrieuChung(std::string trieuChung) {
    TrieuChung = trieuChung;
}
void LichKham::setLydoKham(std::string lydoKham) {
    LydoKham = lydoKham;
}
void LichKham::setBacsi(Doctor bacsi) {
    Bacsi = bacsi;
}

std::istream& operator>>(std::istream& is, LichKham& lichKham) {
    std::cout << "Enter Ma Lich Kham: ";
    is >> lichKham.MaLichKham;
    std::cout << "Enter Ngay Kham: ";
    is >> lichKham.NgayKham;
    std::cout << "Enter Gio Kham: ";
    is >> lichKham.GioKham;
    std::cout << "Enter Trang Thai: ";
    is >> lichKham.TrangThai;
    std::cout << "Enter Trieu Chung: ";
    is >> lichKham.TrieuChung;
    std::cout << "Enter Ly do Kham: ";
    is >> lichKham.LydoKham;
    // Note: Input for Doctor object (Bacsi) should be handled separately
    return is;
}

