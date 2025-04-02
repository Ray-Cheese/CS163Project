#include "InputBox.h"

// Constructor: Khởi tạo hộp nhập liệu với các thuộc tính
InputBox::InputBox(Rectangle bounds, Color boxColor, Color borderColor, Color textColor)
    : bounds(bounds), text(""), active(false), boxColor(boxColor), borderColor(borderColor), textColor(textColor) {}

// Cập nhật trạng thái hộp nhập liệu
void InputBox::Update() {
    Vector2 mousePos = GetMousePosition();

    // Kiểm tra xem có nhấp chuột vào hộp nhập liệu không
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        active = CheckCollisionPointRec(mousePos, bounds);
    }

    // Nếu hộp đang được nhập liệu, xử lý ký tự nhập vào
    if (active) {
        int key = GetCharPressed();

        while (key > 0) {
            if (key >= 32 && key <= 125 && text.size() < 255) { // Chỉ nhận ký tự ASCII từ 32-125
                text.push_back(static_cast<char>(key));
            }
            key = GetCharPressed(); // Lấy ký tự tiếp theo (nếu có)
        }

        // Xóa ký tự khi nhấn Backspace
        if (IsKeyPressed(KEY_BACKSPACE) && !text.empty()) {
            text.pop_back();
        }
    }
}

// Vẽ hộp nhập liệu
void InputBox::Draw() const {
    DrawRectangleRec(bounds, boxColor);                 // Vẽ hộp nhập
    DrawRectangleLinesEx(bounds, 2, borderColor);       // Viền hộp nhập
    DrawText(text.c_str(), bounds.x + 10, bounds.y + 10, 20, textColor); // Hiển thị nội dung nhập vào
}
