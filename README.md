# MediCare - Hệ Thống Đặt Lịch Khám Bệnh

## Tổng quan
MediCare là một ứng dụng desktop cho hệ thống đặt lịch khám bệnh, được phát triển bằng C++ và SFML 3.x.

## Tính năng

### 1. Giao diện đăng ký tài khoản
- **Chọn vai trò**: Người dùng có thể chọn đăng ký với vai trò Bệnh nhân hoặc Bác sĩ
- **Form đăng ký đầy đủ**:
  - Bệnh nhân: Username, Password, Họ tên, Email, SĐT, Địa chỉ, Ngày sinh, Giới tính, Nhóm máu, CCCD
  - Bác sĩ: Tất cả thông tin trên + Chuyên môn và Khoa

### 2. Hệ thống đăng nhập
- Đăng nhập bằng username và password
- Tự động chuyển đến dashboard tương ứng với vai trò

### 3. Lưu trữ dữ liệu
Dữ liệu được lưu trữ theo cấu trúc sau trong thư mục `build/data`:
```
data/
├── Admin.txt              # Danh sách ID Admin
├── Admin/                 # Thư mục chứa thông tin Admin
│   └── 00.txt            # File thông tin admin (mặc định)
├── Patient.txt            # Danh sách ID Bệnh nhân
├── Patient/               # Thư mục chứa thông tin Bệnh nhân
│   ├── 0101.txt          # File credential (username, password)
│   └── 0101_info.txt     # File thông tin chi tiết
├── Doctor.txt             # Danh sách ID Bác sĩ
├── Doctor/                # Thư mục chứa thông tin Bác sĩ
│   ├── 020101.txt        # File credential (username, password)
│   └── 020101_info.txt   # File thông tin chi tiết
└── appointments/          # Thư mục chứa lịch hẹn
    └── APTyyyymmddhhmmss.txt
```

## Cách build và chạy

### Yêu cầu hệ thống
- Windows 10/11
- MSYS2 với UCRT64 environment
- CMake 3.16+
- SFML 3.0+
- Font Arial (có sẵn trong Windows)

### Build ứng dụng
```powershell
# Trong thư mục gốc của project
cmake -S . -B build
cmake --build build
```

### Chạy ứng dụng
```powershell
cd build
.\Datchikham.exe
```

## Cấu trúc project
```
datlichkham/
├── CMakeLists.txt           # File cấu hình CMake
├── include/                 # Header files
│   ├── GUI.h               # Giao diện chính
│   ├── Account.h           # Hệ thống tài khoản
│   ├── DataStore.h         # Quản lý dữ liệu
│   ├── Patient.h           # Class Bệnh nhân
│   ├── Doctor.h            # Class Bác sĩ
│   └── ...
├── src/                    # Source files
│   ├── main_gui.cpp        # Entry point
│   ├── GUI.cpp             # Implementation giao diện
│   ├── Account.cpp         # Implementation tài khoản
│   ├── DataStore.cpp       # Implementation quản lý dữ liệu
│   └── ...
├── assets/                 # Tài nguyên (fonts, images)
│   └── fonts/
└── build/                  # Thư mục build
    ├── Datchikham.exe      # Executable
    └── data/               # Dữ liệu ứng dụng
```

## Giao diện ứng dụng

### 1. Màn hình chọn vai trò
- Nền xanh lam nhạt với thanh tiêu đề xanh đậm
- Logo chữ thập y tế ở giữa
- 2 thẻ lựa chọn: "Tôi là Bệnh nhân" và "Tôi là Bác sĩ"
- Nút "Tiếp tục" để chuyển sang màn hình đăng ký
- Link "Đã có tài khoản? Đăng nhập ngay"

### 2. Màn hình đăng ký
- Form nhập liệu đầy đủ thông tin
- Các trường input có viền xanh khi được focus
- Password được ẩn hiện dạng dấu *
- Nút "Hoàn thành" để submit đăng ký

### 3. Màn hình đăng nhập
- Form đơn giản với username và password
- Nút "Đăng nhập"
- Link "Chưa có tài khoản? Đăng ký ngay"

### 4. Dashboard
- Dashboard cho Bệnh nhân (chưa hoàn thiện đầy đủ)
- Dashboard cho Bác sĩ (chưa hoàn thiện đầy đủ)

## Tài khoản mặc định
- **Admin**:
  - Username: `admin`
  - Password: `admin`

## Mở rộng trong tương lai
- [ ] Chức năng đặt lịch khám cho bệnh nhân
- [ ] Chức năng quản lý lịch hẹn cho bác sĩ
- [ ] Chức năng tìm kiếm bác sĩ theo chuyên khoa
- [ ] Chức năng đánh giá bác sĩ
- [ ] Chức năng hủy lịch khám
- [ ] Thông báo và nhắc nhở lịch hẹn
- [ ] Lịch sử khám bệnh
- [ ] Xuất báo cáo

## Tác giả
Project PBL2 - DLK

## License
[Thêm thông tin license nếu có]
