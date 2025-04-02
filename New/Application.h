#ifndef APPLICATION_H
#define APPLICATION_H

#include "Button.h"
#include "InputBox.h"
#include "Menu.h"
#include <vector>
#include <list>
#include <set>

const int HASH_TABLE_SIZE = 10;

class Application {
public:
    int w, h;
    float leftWidth;
    bool showCreateMenu;
    bool showInitMenu;
    std::string selectedOption;
    std::string selectedInitOption;

    Button createButton;
    Button startButton;
    Menu createMenu;
    Menu initMenu;
    InputBox inputBox;

    std::vector<std::list<int>> hashTable;

    Application(int width, int height);

    int HashFunction(int key);
    void Insert(int key);
    void Delete(int key);
    bool Search(int key);
    void InitRandom();
    void InitFromFile(const std::string& filename);
    void DrawHashTable();
    void DrawAlgorithmSteps();
    void UpdateHashTable();
    void Update();
    void Draw();
};

#endif
