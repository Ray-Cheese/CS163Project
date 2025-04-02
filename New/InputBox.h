#ifndef INPUTBOX_H
#define INPUTBOX_H

#include "raylib.h"
#include <string>

class InputBox {
public:
    Rectangle bounds;
    std::string text;
    bool active;
    Color boxColor;
    Color borderColor;
    Color textColor;

    InputBox(Rectangle bounds, Color boxColor, Color borderColor, Color textColor);

    void Update();
    void Draw() const;
};

#endif
