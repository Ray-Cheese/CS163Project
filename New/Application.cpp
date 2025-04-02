#include "Application.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>

Application::Application(int width, int height)
    : w(width), h(height), leftWidth(width * 0.3f),
    showCreateMenu(false), showInitMenu(false),
    selectedOption("Create"), selectedInitOption(""),
    createButton({ leftWidth * 0.05f, h * 0.05f, leftWidth * 0.4f, 50 }, "Create", ORANGE, GOLD, WHITE),
    startButton({ leftWidth * 0.4f, h * 0.05f, leftWidth * 0.4f, 50 }, "Start", GREEN, LIME, WHITE),
    inputBox({ leftWidth * 0.05f, h * 0.45f, leftWidth * 0.9f, 40 }, Color{ 255, 255, 204, 255 }, DARKBLUE, BLACK),
    hashTable(HASH_TABLE_SIZE) {

    // Cấu hình menu Create
    float btnY = createButton.bounds.y + createButton.bounds.height + 10;
    float btnWidth = createButton.bounds.width;
    const char* options[] = { "Init", "Add", "Del", "Search" };

    for (int i = 0; i < 4; i++) {
        createMenu.AddButton(Button({ createButton.bounds.x, btnY + i * 55, btnWidth, 50 }, options[i], SKYBLUE, GOLD, WHITE));
    }

    // Cấu hình menu Init
    float initX = createButton.bounds.x + createButton.bounds.width + 10;
    const char* initOptions[] = { "Random", "File", "Input" };

    for (int i = 0; i < 3; i++) {
        initMenu.AddButton(Button({ initX, btnY + i * 55, btnWidth, 50 }, initOptions[i], SKYBLUE, GOLD, WHITE));
    }
}

int Application::HashFunction(int key) {
    return key % HASH_TABLE_SIZE;
}

void Application::Insert(int key) {
    int index = HashFunction(key);

    for (int k : hashTable[index]) {
        if (k == key) {
            std::cout << "Key " << key << " already exists in the hash table!" << std::endl;
            return;
        }
    }
    hashTable[index].push_back(key);
}

void Application::Delete(int key) {
    int index = HashFunction(key);
    hashTable[index].remove(key);
}

bool Application::Search(int key) {
    int index = HashFunction(key);
    for (int k : hashTable[index]) {
        if (k == key)
            return true;
    }
    return false;
}

void Application::InitRandom() {
    hashTable.clear();
    hashTable.resize(HASH_TABLE_SIZE);
    srand(static_cast<unsigned int>(time(0)));

    std::set<int> uniqueKeys;
    while (uniqueKeys.size() < 20) {
        int key = rand() % 100;
        if (uniqueKeys.find(key) == uniqueKeys.end()) {
            uniqueKeys.insert(key);
            Insert(key);
        }
    }
}

void Application::InitFromFile(const std::string& filename) {
    hashTable.clear();
    hashTable.resize(HASH_TABLE_SIZE);
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    std::set<int> uniqueKeys;
    int key;
    while (file >> key) {
        if (uniqueKeys.find(key) == uniqueKeys.end()) {
            uniqueKeys.insert(key);
            Insert(key);
        }
    }
    file.close();
}

void Application::DrawHashTable() {
    float bucketSpacing = 150.0f;
    float nodeSpacing = 60.0f;
    float startX = leftWidth + 50;
    float startY = h * 0.2f;

    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        float x = startX + i * bucketSpacing;
        float y = startY;

        for (int key : hashTable[i]) {
            DrawRectangle(x - 25, y - 25, 50, 50, LIGHTGRAY);
            std::string text = std::to_string(key);
            int textWidth = MeasureText(text.c_str(), 20);
            DrawText(text.c_str(), x - textWidth / 2, y - 10, 20, BLACK);

            if (key != hashTable[i].back()) {
                DrawLine(x, y + 25, x, y + nodeSpacing - 25, BLACK);
                DrawTriangle(
                    { x - 5, y + nodeSpacing - 25 },
                    { x + 5, y + nodeSpacing - 25 },
                    { x, y + nodeSpacing - 15 },
                    BLACK
                );
            }

            y += nodeSpacing;
        }
    }
}

void Application::DrawAlgorithmSteps() {
    DrawRectangle(0, h * 0.5f, leftWidth, h * 0.5f, ColorAlpha(RAYWHITE, 0.9f));
    DrawRectangleLines(0, h * 0.5f, leftWidth, h * 0.5f, DARKBLUE);
    DrawText("Algorithm Steps:", 20, h * 0.5f + 20, 20, DARKBLUE);

    if (selectedOption == "Add") {
        DrawText("1. Calculate hash index.", 20, h * 0.5f + 50, 20, DARKBLUE);
        DrawText("2. Insert key into the bucket.", 20, h * 0.5f + 80, 20, DARKBLUE);
    }
    else if (selectedOption == "Del") {
        DrawText("1. Calculate hash index.", 20, h * 0.5f + 50, 20, DARKBLUE);
        DrawText("2. Remove key from the bucket.", 20, h * 0.5f + 80, 20, DARKBLUE);
    }
    else if (selectedOption == "Search") {
        DrawText("1. Calculate hash index.", 20, h * 0.5f + 50, 20, DARKBLUE);
        DrawText("2. Search for key in the bucket.", 20, h * 0.5f + 80, 20, DARKBLUE);
    }
}

void Application::UpdateHashTable() {
    if (IsKeyPressed(KEY_ENTER)) {
        try {
            int key = std::stoi(inputBox.text);
            if (selectedOption == "Add") {
                Insert(key);
            }
            else if (selectedOption == "Del") {
                Delete(key);
            }
            else if (selectedOption == "Search") {
                std::cout << (Search(key) ? "Key found!" : "Key not found!") << std::endl;
            }
            inputBox.text = "";
        }
        catch (const std::invalid_argument&) {
            std::cerr << "Invalid input! Please enter a number." << std::endl;
            inputBox.text = "";
        }
    }
}

void Application::Update() {
    Vector2 mousePos = GetMousePosition();

    createButton.Update();
    startButton.Update();

    if (createButton.IsClicked(mousePos)) {
        showCreateMenu = !showCreateMenu;
        createMenu.visible = showCreateMenu;
        showInitMenu = false;
    }

    if (startButton.IsClicked(mousePos)) {
        inputBox.text = "";
    }

    createMenu.Update();
    initMenu.Update();

    int clickedIndex = createMenu.ProcessClick();
    if (clickedIndex != -1) {
        selectedOption = createMenu.buttons[clickedIndex].label;
        if (selectedOption == "Init") {
            showInitMenu = !showInitMenu;
            initMenu.visible = showInitMenu;
        }
    }

    int initClickedIndex = initMenu.ProcessClick();
    if (initClickedIndex != -1) {
        selectedInitOption = initMenu.buttons[initClickedIndex].label;
        if (selectedInitOption == "Random") {
            InitRandom();
        }
        else if (selectedInitOption == "File") {
            InitFromFile("data.txt");
        }
    }

    inputBox.Update();
    UpdateHashTable();
}

void Application::Draw() {

	ClearBackground(RAYWHITE);
    DrawRectangle(0, 0, leftWidth, h, SKYBLUE);
    DrawRectangleLines(0, 0, leftWidth, h, BLACK);

    DrawText("HASH TABLE VISUALIZATION", 900, 20, 50, RED);
    
    createButton.Draw();
    startButton.Draw();
    createMenu.Draw();
    initMenu.Draw();
    inputBox.Draw();
    DrawHashTable();
    DrawAlgorithmSteps();
}
