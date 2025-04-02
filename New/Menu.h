#ifndef MENU_H
#define MENU_H

#include "Button.h"
#include <vector>

class Menu {
public:
    std::vector<Button> buttons;
    bool visible;
    float slideOffset;

    Menu();
    void AddButton(const Button& btn);
    int ProcessClick();
    void Update();
    void Draw();
};

#endif
