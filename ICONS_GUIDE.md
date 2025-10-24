# 🎨 Hướng Dẫn Icons & Symbols - MediCare UI

## ✅ Đã Cải Thiện

### 1. **Navigation Menu Icons (Thay thế Emoji)**

| Old (Emoji) | New (Unicode) | Ý nghĩa | Code |
|-------------|---------------|---------|------|
| 📅 | ⊕ | Đặt lịch (Plus in circle) | `u8"⊕"` |
| 📋 | ☰ | Danh sách (Hamburger menu) | `u8"☰"` |
| 🔔 | ◉ | Thông báo (Circle dot) | `u8"◉"` |
| 👤 | ◈ | User profile (Diamond) | `u8"◈"` |

**Lý do thay đổi:**
- Unicode symbols render nhanh hơn và ổn định hơn
- Không phụ thuộc vào hệ thống emoji
- Font Roboto hỗ trợ tốt các ký tự này

---

### 2. **Header Icons**

#### Logo Icon:
```cpp
⚕  // Medical symbol (Staff of Asclepius)
Màu: Green (#4CAF50)
Size: 32px
Position: Header left
```

#### User Avatar:
```cpp
◉  // Circle with dot (represents user)
Màu: Green (#4CAF50)
Size: 24px
Position: Top-right
```

#### Logout Icon:
```cpp
⊗  // Circle with X (exit symbol)
Màu: Red-ish (#DC6464)
Size: 14px
Position: Next to "Đăng xuất"
```

---

### 3. **Button Icons**

#### Search Buttons:
```cpp
⚕ Chuyên khoa   // Medical cross
📆 Chọn ngày     // Calendar (emoji vẫn OK cho buttons)
🔍 Tìm kiếm      // Magnifying glass
```

---

### 4. **Appointment Card Icons**

#### Doctor Avatar:
```cpp
⚕  // Medical symbol
Màu: Blue (#1E88E5)
Size: 32px
```

#### Specialty Indicator:
```cpp
●  // Bullet point
Màu: Gray (#647884)
Size: 12px
```

#### Time Icon:
```cpp
◷  // Clock symbol
Màu: Gray (#506478)
Size: 14px
```

#### Cancel Button:
```cpp
✕ Hủy  // X mark + text
Màu: White on Red
Size: 12px
```

#### Arrow:
```cpp
›  // Right chevron
Màu: Gray (#96AABE)
Size: 24px
```

---

### 5. **Empty State Icon**

```cpp
📭  // Empty mailbox (emoji OK cho empty state)
Màu: Gray (#96AABE)
Size: 48px
Text: "Không có lịch khám nào"
```

---

## 📐 Kích Thước Đã Điều Chỉnh

### Buttons (Rộng hơn để không bị mất chữ):

| Element | Old Size | New Size | Reason |
|---------|----------|----------|--------|
| Search bar | 350x45 | 380x45 | +30px width |
| Chuyên khoa btn | 170x45 | 185x45 | +15px width |
| Chọn ngày btn | 170x45 | 185x45 | +15px width |
| Tìm kiếm btn | 350x45 | 380x45 | +30px width |
| Appointment card | 450x100 | 470x100 | +20px width |
| Cancel button | 120x30 | 100x28 | Smaller for balance |

### Font Sizes (Giảm để vừa hơn):

| Element | Old Size | New Size |
|---------|----------|----------|
| Nav menu text | 16px | 14px |
| Nav menu icon | 24px | 28px |
| Card doctor name | 18px | 16px |
| Card specialty | 14px | 13px |
| Card date/time | 14px | 13px |
| Card cancel btn | 14px | 12px |
| User name | 16px | 14px |
| Logout link | 14px | 13px |

---

## 🎯 Danh Sách Unicode Symbols Hữu Ích

### Medical & Health:
```
⚕  Staff of Asclepius (medical symbol)
✚  Medical cross
♥  Heart
⊕  Plus in circle
⊗  Circle with X
```

### UI Elements:
```
◉  Circle dot (notifications, active state)
◈  Diamond (settings, profile)
☰  Hamburger menu (list)
›  Right chevron (navigation)
●  Bullet point
◷  Clock
✕  X mark (close, cancel)
```

### Status Indicators:
```
✓  Checkmark (success)
✗  X mark (error)
⚠  Warning triangle
ℹ  Information
```

### Navigation:
```
←  Left arrow
→  Right arrow
↑  Up arrow
↓  Down arrow
⇦  Double left
⇨  Double right
```

### Shapes:
```
■  Square
□  Empty square
▲  Triangle up
▼  Triangle down
◆  Diamond filled
◇  Diamond empty
```

---

## 💡 Gợi Ý Thêm Icons

### Cho Dashboard:
```cpp
// Statistics
📊  Graph (hoặc ▬ bar chart symbol)
📈  Trending up
💰  Money/payments

// Doctor profile
🩺  Stethoscope
💊  Medicine
🏥  Hospital

// Calendar
📅  Calendar (hoặc ▦ grid symbol)
⏰  Alarm clock
📆  Tear-off calendar
```

### Cho Notifications:
```cpp
🔔  Bell (new notification)
✓  Checkmark (read)
⚠  Warning
ℹ  Info
```

### Cho Settings:
```cpp
⚙  Gear (settings)
🔒  Lock (security)
🔓  Unlock
👁  Eye (visibility)
```

---

## 🔧 How to Use

### Trong Code:
```cpp
// Define icon as UTF-8 string
sf::Text icon = makeText(font, u8"⚕", 28);
icon.setPosition({x, y});
icon.setFillColor(sf::Color(30, 136, 229));
window.draw(icon);
```

### Combine Icon + Text:
```cpp
// Method 1: Separate elements
sf::Text icon = makeText(font, u8"⚕", 20);
sf::Text text = makeText(font, u8"Chuyên khoa", 16);

// Method 2: Combined string
sf::Text button = makeText(font, u8"⚕ Chuyên khoa", 16);
```

---

## ✅ Testing Checklist

### Icons to Test:
- [ ] Navigation menu icons hiển thị đúng
- [ ] Header logo (⚕) hiển thị
- [ ] User avatar (◉) hiển thị
- [ ] Logout icon (⊗) hiển thị
- [ ] Appointment card icons (⚕, ●, ◷, ›) hiển thị
- [ ] Empty state icon (📭) hiển thị khi không có lịch
- [ ] Button icons hiển thị trong buttons

### Layout to Test:
- [ ] Buttons đủ rộng, không bị cắt chữ
- [ ] Text size vừa phải, dễ đọc
- [ ] Icons có kích thước phù hợp
- [ ] Spacing giữa elements hợp lý

---

## 🚀 Kết Quả

✅ **Đã giải quyết:**
1. Emoji bị lỗi → Thay bằng Unicode symbols ổn định
2. Text bị cắt → Tăng width buttons/cards + giảm font size
3. Không có data → Hiển thị "Không có lịch khám nào"
4. Icons đơn giản → Thêm medical symbols đẹp hơn

✅ **UI/UX tốt hơn:**
- Clean, professional icons
- Better text spacing
- Clear visual hierarchy
- Proper empty state handling

**Giao diện bây giờ chuyên nghiệp và ổn định hơn!** 🎉
