#include "raylib.h"
#include "raymath.h"
#include <cstring>
#include <string>
#include <vector>
#include <list>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <set> // Thêm thư viện set để kiểm tra trùng lặp

// Constants
const float NODE_RADIUS = 25.0f;
const int HASH_TABLE_SIZE = 10;
const float MENU_SLIDE_SPEED = 10.0f;
const float BUTTON_SCALE_FACTOR = 1.1f;

// Button Class
class Button {
public:
    Rectangle bounds;
    std::string label;
    Color normalColor;
    Color hoverColor;
    Color textColor;
    float scale;

    Button(Rectangle bounds, const std::string& label, Color normalColor, Color hoverColor, Color textColor)
        : bounds(bounds), label(label), normalColor(normalColor), hoverColor(hoverColor), textColor(textColor), scale(1.0f) {}

    bool IsMouseOver(Vector2 mousePos) const {
        return CheckCollisionPointRec(mousePos, bounds);
    }

    bool IsClicked(Vector2 mousePos) const {
        return IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && IsMouseOver(mousePos);
    }

    void Update() {
        Vector2 mousePos = GetMousePosition();
        if (IsMouseOver(mousePos)) {
            scale = Lerp(scale, BUTTON_SCALE_FACTOR, 0.1f);
        }
        else {
            scale = Lerp(scale, 1.0f, 0.1f);
        }
    }

    void Draw() const {
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
};

// InputBox Class
class InputBox {
public:
    Rectangle bounds;
    std::string text;
    bool active;
    Color boxColor;
    Color borderColor;
    Color textColor;

    InputBox(Rectangle bounds, Color boxColor, Color borderColor, Color textColor)
        : bounds(bounds), text(""), active(false), boxColor(boxColor), borderColor(borderColor), textColor(textColor) {}

    void Update() {
        Vector2 mousePos = GetMousePosition();
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            active = CheckCollisionPointRec(mousePos, bounds);
        }
        if (active) {
            int key = GetCharPressed();
            while (key > 0) {
                if (key >= 32 && key <= 125 && text.size() < 255)
                    text.push_back(static_cast<char>(key));
                key = GetCharPressed();
            }
            if (IsKeyPressed(KEY_BACKSPACE) && !text.empty())
                text.pop_back();
        }
    }

    void Draw() const {
        DrawRectangle(bounds.x, bounds.y, bounds.width, bounds.height, boxColor);
        DrawRectangleLines(bounds.x, bounds.y, bounds.width, bounds.height, borderColor);
        DrawText(text.c_str(), bounds.x + 10, bounds.y + 10, 20, textColor);
    }
};

// Menu Class
class Menu {
public:
    std::vector<Button> buttons;
    bool visible;
    float slideOffset;

    Menu() : visible(false), slideOffset(-200.0f) {}

    void AddButton(const Button& btn) {
        buttons.push_back(btn);
    }

    int ProcessClick() {
        Vector2 mousePos = GetMousePosition();
        for (size_t i = 0; i < buttons.size(); i++) {
            if (buttons[i].IsClicked(mousePos))
                return static_cast<int>(i);
        }
        return -1;
    }

    void Update() {
        if (visible) {
            slideOffset = Lerp(slideOffset, 0.0f, 0.1f);
        }
        else {
            slideOffset = Lerp(slideOffset, -200.0f, 0.1f);
        }
    }

    void Draw() {
        if (!visible && slideOffset <= -190.0f) return;
        for (auto& btn : buttons) {
            btn.bounds.x += slideOffset;
            btn.Draw();
            btn.bounds.x -= slideOffset;
        }
    }
};

// Application Class
class Application {
public:
    int w, h;
    float leftWidth;
    bool showCreateMenu;
    bool showInitMenu;
    std::string selectedOption;
    std::string selectedInitOption;
    float animationProgress;

    Button createButton;
    Button startButton;
    Menu createMenu;
    Menu initMenu;
    InputBox inputBox;

    std::vector<std::list<int>> hashTable;

    Application(int width, int height)
        : w(width), h(height), leftWidth(width * 0.3f),
        showCreateMenu(false), showInitMenu(false),
        selectedOption("Create"), selectedInitOption(""),
        animationProgress(0.0f),
        createButton({ leftWidth * 0.05f, h * 0.05f, leftWidth * 0.4f, 50 }, "Create", ORANGE, GOLD, WHITE),
        startButton({ leftWidth * 0.4f, h * 0.05f, leftWidth * 0.4f, 50 }, "Start", GREEN, LIME, WHITE),
        inputBox({ leftWidth * 0.05f, h * 0.45f, leftWidth * 0.9f, 40 }, Color{ 255, 255, 204, 255 }, DARKBLUE, BLACK),
        hashTable(HASH_TABLE_SIZE) {
        // Cấu hình create menu
        float btnY = createButton.bounds.y + createButton.bounds.height + 10;
        float btnWidth = createButton.bounds.width;
        Rectangle btnRect;
        const char* options[] = { "Init", "Add", "Del", "Search" };
        for (int i = 0; i < 4; i++) {
            btnRect = { createButton.bounds.x, btnY + i * 55, btnWidth, 50 };
            createMenu.AddButton(Button(btnRect, options[i], SKYBLUE, GOLD, WHITE));
        }

        // Cấu hình init menu
        float initX = createButton.bounds.x + createButton.bounds.width + 10;
        float initY = createButton.bounds.y;
        Rectangle initRect;
        const char* initOptions[] = { "Random", "File", "Input" };
        for (int i = 0; i < 3; i++) {
            initRect = { initX, initY + i * 55, btnWidth, 50 };
            initMenu.AddButton(Button(initRect, initOptions[i], SKYBLUE, GOLD, WHITE));
        }
    }

    int HashFunction(int key) {
        return key % HASH_TABLE_SIZE;
    }

    void Insert(int key) {
        int index = HashFunction(key);
        // Kiểm tra xem key đã tồn tại trong bucket chưa
        for (int k : hashTable[index]) {
            if (k == key) {
                std::cout << "Key " << key << " already exists in the hash table!" << std::endl;
                return; // Không thêm nếu key đã tồn tại
            }
        }
        // Nếu key chưa tồn tại, thêm vào bucket
        hashTable[index].push_back(key);
    }

    void Delete(int key) {
        int index = HashFunction(key);
        hashTable[index].remove(key);
    }

    bool Search(int key) {
        int index = HashFunction(key);
        for (int k : hashTable[index]) {
            if (k == key)
                return true;
        }
        return false;
    }

    void InitRandom() {
        hashTable.clear();
        hashTable.resize(HASH_TABLE_SIZE);
        srand(static_cast<unsigned int>(time(0)));

        std::set<int> uniqueKeys; // Sử dụng set để lưu trữ các giá trị duy nhất
        while (uniqueKeys.size() < 20) { // Tạo 20 giá trị ngẫu nhiên không trùng lặp
            int key = rand() % 100;
            if (uniqueKeys.find(key) == uniqueKeys.end()) { // Nếu key chưa tồn tại
                uniqueKeys.insert(key);
                Insert(key); // Thêm vào hash table
            }
        }
    }

    void InitFromFile(const std::string& filename) {
        hashTable.clear();
        hashTable.resize(HASH_TABLE_SIZE);
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << filename << std::endl;
            return;
        }

        std::set<int> uniqueKeys; // Sử dụng set để lưu trữ các giá trị duy nhất
        int key;
        while (file >> key) {
            if (uniqueKeys.find(key) == uniqueKeys.end()) { // Nếu key chưa tồn tại
                uniqueKeys.insert(key);
                Insert(key); // Thêm vào hash table
            }
        }
        file.close();
    }

    void DrawHashTable() {
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

    void DrawAlgorithmSteps() {
        DrawRectangle(0, h * 0.5f, leftWidth, h * 0.5f, ColorAlpha(RAYWHITE, 0.9f));
        DrawRectangleLines(0, h * 0.5f, leftWidth, h * 0.5f, DARKBLUE);
        DrawLine(0, h * 0.5f, leftWidth, h * 0.5f, DARKBLUE);
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

    void UpdateHashTable() {
        if (selectedOption == "Add" && IsKeyPressed(KEY_ENTER)) {
            try {
                int key = std::stoi(inputBox.text);
                Insert(key); // Gọi hàm Insert đã cập nhật
                inputBox.text = "";
            }
            catch (const std::invalid_argument& e) {
                std::cerr << "Invalid input! Please enter a number." << std::endl;
                inputBox.text = "";
            }
        }
        else if (selectedOption == "Del" && IsKeyPressed(KEY_ENTER)) {
            try {
                int key = std::stoi(inputBox.text);
                Delete(key);
                inputBox.text = "";
            }
            catch (const std::invalid_argument& e) {
                std::cerr << "Invalid input! Please enter a number." << std::endl;
                inputBox.text = "";
            }
        }
        else if (selectedOption == "Search" && IsKeyPressed(KEY_ENTER)) {
            try {
                int key = std::stoi(inputBox.text);
                if (Search(key))
                    std::cout << "Key found!" << std::endl;
                else
                    std::cout << "Key not found!" << std::endl;
                inputBox.text = "";
            }
            catch (const std::invalid_argument& e) {
                std::cerr << "Invalid input! Please enter a number." << std::endl;
                inputBox.text = "";
            }
        }
    }

    void Update() {
        Vector2 mousePos = GetMousePosition();

        createButton.Update();
        startButton.Update();

        if (createButton.IsClicked(mousePos)) {
            showCreateMenu = !showCreateMenu;
            createMenu.visible = showCreateMenu;
            showInitMenu = false; // Ẩn init menu khi create menu được hiển thị
        }

        if (startButton.IsClicked(mousePos))
            inputBox.text = "";

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
                showInitMenu = false;
            }
            else if (selectedInitOption == "File") {
                InitFromFile("data.txt"); // Thay "data.txt" bằng tên file của bạn
                showInitMenu = false;
            }
            else if (selectedInitOption == "Input") {
                inputBox.text = "";
                inputBox.active = true;
                showInitMenu = false;
            }
        }

        inputBox.Update();
        UpdateHashTable();
    }

    void Draw() {
        ClearBackground(RAYWHITE);
        DrawRectangle(0, 0, leftWidth, h, Color{ 173, 216, 230, 255 });

        DrawText("HASH TABLE", leftWidth + (w - leftWidth) / 2 - MeasureText("HASH TABLE", 50) / 2, 20, 50, RED);

        createButton.Draw();
        startButton.Draw();

        createMenu.Draw();
        if (showInitMenu) {
            initMenu.Draw();
        }

        if (selectedOption == "Add" || selectedOption == "Del" || selectedOption == "Search")
            DrawText("Input number:", inputBox.bounds.x, inputBox.bounds.y - 30, 20, DARKBLUE);

        if (selectedOption == "Add" || selectedOption == "Del" || selectedOption == "Search") {
            inputBox.Draw();
        }

        DrawHashTable();
        DrawAlgorithmSteps();
    }
};

// Main Function
int main() {
    const int screenWidth = 2000;
    const int screenHeight = 1000;
    InitWindow(screenWidth, screenHeight, "Hash Table with Chaining");
    SetTargetFPS(60);

    Application app(screenWidth, screenHeight);

    while (!WindowShouldClose()) {
        app.Update();

        BeginDrawing();
        app.Draw();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}