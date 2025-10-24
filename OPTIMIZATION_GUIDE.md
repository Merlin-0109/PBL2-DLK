# Hệ Thống Đặt Lịch Khám - Hướng Dẫn Tối Ưu

## Các Cải Tiến Đã Thực Hiện

### 1. Font Tiếng Việt ✅
**Vấn đề cũ:** Arial không hỗ trợ tốt dấu tiếng Việt
**Giải pháp:**
- Đổi sang **Segoe UI** (font mặc định Windows, hỗ trợ Unicode hoàn hảo)
- Fallback chain: Segoe UI → Arial Unicode → Tahoma → Arial
- Thêm u8 prefix cho tất cả string tiếng Việt
- Lưu tất cả file UTF-8 encoding

**Kết quả:** Tiếng Việt hiển thị đẹp, rõ ràng, đầy đủ dấu

### 2. Hiệu Năng ⚡
**Tối ưu đã làm:**
- ✅ Giới hạn 60 FPS (`window.setFramerateLimit(60)`)
- ✅ Tắt VSync để dùng framerate limit
- ✅ Smooth text rendering (outline thickness = 0)
- ✅ Light blue background (#F0F8FF) thay vì trắng (giảm mỏi mắt)

### 3. Account System 🔐
**Cải tiến:**
- Validation username (3-20 ký tự, chỉ chữ/số/_/.)
- Validation password (6-50 ký tự)
- Error handling đầy đủ với message tiếng Việt
- Check username trùng trước khi đăng ký
- Hỗ trợ đổi mật khẩu

## Nếu Vẫn Còn Lag

### Giảm độ phức tạp UI:
1. Mở `src/GUI.cpp`
2. Tìm dòng `sf::RectangleShape shadow` 
3. Comment hoặc xóa các shadow effect:
```cpp
// sf::RectangleShape shadow({740, 670});
// shadow.setPosition({275, 125});
// shadow.setFillColor(sf::Color(200, 200, 200, 100));
// window.draw(shadow);
```

### Giảm kích thước font:
- Đổi các font size lớn (48, 36, 32) xuống 24-28
- Giảm số lượng text objects được render

### Tắt cursor animation:
Trong `GUI::handleEvents()`, comment phần cursor blink:
```cpp
// if (cursorClock.getElapsedTime().asSeconds() > 0.5f) {
//     showCursor = !showCursor;
//     cursorClock.restart();
// }
```

## Kiểm Tra Font Tiếng Việt

### Test font hiện tại:
```bash
cd build
.\Datchikham.exe
```

Kiểm tra các chữ có dấu:
- Đặt, Đăng, Đã
- Lịch, Khám, Bệnh
- Chọn, Thông tin
- Bác sĩ, Bệnh nhân

### Nếu font vẫn lỗi:
1. Kiểm tra console output xem font nào được load
2. Thử font khác trong danh sách:
   - `C:/Windows/Fonts/segoeui.ttf` (khuyên dùng)
   - `C:/Windows/Fonts/tahomabd.ttf`
   - `C:/Windows/Fonts/msgothic.ttf`

## Specs Đề Nghị
- **CPU:** Intel i3 hoặc tương đương
- **RAM:** 2GB trở lên
- **GPU:** Integrated graphics đủ (Intel HD Graphics)
- **OS:** Windows 10/11

## Build Lại Sau Khi Thay Đổi
```bash
cd "C:\Users\admin\OneDrive - The University of Technology\Desktop\datlichkham"
cmake --build build
.\build\Datchikham.exe
```

## Liên Hệ Hỗ Trợ
Nếu vẫn gặp vấn đề, cung cấp:
1. Screenshot lỗi font
2. Console output khi chạy app
3. Thông số PC (CPU, RAM, GPU)
