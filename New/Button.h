#ifndef BUTTON_H
#define BUTTON_H

#include "raylib.h"
#include <string>

class Button {
public:
    Rectangle bounds;
    std::string label;
    Color normalColor;
    Color hoverColor;
    Color textColor;
    float scale;

    Button(Rectangle bounds, const std::string& label, Color normalColor, Color hoverColor, Color textColor);

    bool IsMouseOver(Vector2 mousePos) const;
    bool IsClicked(Vector2 mousePos) const;
    void Update();
    void Draw() const;
};

#endif
