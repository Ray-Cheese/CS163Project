#include "Menu.h"
#include "raylib.h"
#include "raymath.h"

// Constructor: Khởi tạo menu với trạng thái ẩn và hiệu ứng trượt ban đầu
Menu::Menu() : visible(false), slideOffset(-200.0f) {}

// Thêm một nút vào menu
void Menu::AddButton(const Button& btn) {
    buttons.push_back(btn);
}

// Xử lý sự kiện click chuột, trả về chỉ mục của nút được nhấn
int Menu::ProcessClick() {
    Vector2 mousePos = GetMousePosition();
    for (size_t i = 0; i < buttons.size(); i++) {
        if (buttons[i].IsClicked(mousePos))
            return static_cast<int>(i);
    }
    return -1; // Không có nút nào được nhấn
}

// Cập nhật hiệu ứng trượt của menu
void Menu::Update() {
    if (visible) {
        slideOffset = Lerp(slideOffset, 0.0f, 0.1f);  // Trượt ra
    }
    else {
        slideOffset = Lerp(slideOffset, -200.0f, 0.1f); // Trượt vào
    }
}

// Vẽ menu và các nút bên trong
void Menu::Draw() {
    if (!visible && slideOffset <= -190.0f) return; // Nếu menu đã ẩn hoàn toàn thì không vẽ

    for (auto& btn : buttons) {
        btn.bounds.x += slideOffset;  // Dịch chuyển nút theo menu
        btn.Draw();
        btn.bounds.x -= slideOffset;  // Trả về vị trí cũ để không ảnh hưởng đến lần vẽ tiếp theo
    }
}
