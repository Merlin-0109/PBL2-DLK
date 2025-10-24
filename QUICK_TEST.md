# Quick Test - Patient Dashboard

## 🚀 Chạy Ứng Dụng
```powershell
.\run.ps1
```

## 📝 Test Account

### Tạo Tài Khoản Mới:
1. Click "Chưa có tài khoản? Đăng Ký"
2. Chọn "Bệnh Nhân"
3. Điền:
   - Username: `demo`
   - Password: `123456`
   - Confirm: `123456`
   - Email: `demo@test.com`
4. Click "Đăng Ký"
5. Skip hoặc điền thông tin cá nhân

### Hoặc Đăng Nhập (nếu đã có account):
```
Username: demo
Password: 123456
```

## 🎨 Giao Diện Mới

### Header (Top):
- Logo: "MediCare"
- User: "Xin chào, demo" 
- Link: "Đăng xuất"

### Navigation (Left):
- 📅 Đặt lịch khám (Active - màu xanh)
- 📋 Lịch khám của tôi
- 🔔 Thông báo
- 👤 Cập nhật thông tin

### Content (Right):
- **Search bar**: "Tìm bác sĩ..."
- **2 Filter buttons**: Chuyên khoa, Ngày
- **Search button**: Tìm kiếm
- **Upcoming appointments**: 2 sample cards

### Appointment Cards Show:
- BS. Nguyễn Văn A - Nha khoa
- Ngày 20/12/2024 09:00 AM
- Button "Hủy lịch" (red)

## ✅ Test Checklist

- [ ] Header hiển thị đúng
- [ ] User name hiển thị
- [ ] Navigation menu có 4 items
- [ ] Active menu màu xanh
- [ ] Content area hiển thị search
- [ ] 2 appointment cards hiển thị
- [ ] Click menu items work
- [ ] Đăng xuất works

## 🐛 Known Limitations

- Appointment cards là sample data (hardcoded)
- Search chưa functional (UI only)
- Filter buttons chưa có popup
- Notification count chưa có

## 🎯 Next Steps

1. Integrate real appointment data
2. Implement search functionality
3. Add specialty dropdown
4. Add date picker
5. Connect to backend API
