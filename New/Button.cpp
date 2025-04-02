#include "Button.h"
#include "raymath.h"

Button::Button(Rectangle bounds, const std::string& label, Color normalColor, Color hoverColor, Color textColor)
    : bounds(bounds), label(label), normalColor(normalColor), hoverColor(hoverColor), textColor(textColor), scale(1.0f) {}

bool Button::IsMouseOver(Vector2 mousePos) const {
    return CheckCollisionPointRec(mousePos, bounds);
}

bool Button::IsClicked(Vector2 mousePos) const {
    return IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && IsMouseOver(mousePos);
}

void Button::Update() {
    Vector2 mousePos = GetMousePosition();
    if (IsMouseOver(mousePos)) {
        scale += (1.1f - scale) * 0.1f; // Hiệu ứng hover
    }
    else {
        scale += (1.0f - scale) * 0.1f;
    }
}

void Button::Draw() const {
    Color drawColor = IsMouseOver(GetMousePosition()) ? hoverColor : normalColor;
    Rectangle scaledBounds = {
        bounds.x + (bounds.width * (1 - scale)) / 2,
        bounds.y + (bounds.height * (1 - scale)) / 2,
        bounds.width * scale,
        bounds.height * scale
    };
    DrawRectangleRounded(scaledBounds, 0.3f, 5, drawColor);
    DrawText(label.c_str(), scaledBounds.x + 20, scaledBounds.y + 15, 20, textColor);
}
