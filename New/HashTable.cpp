#include "HashTable.h"
#include <iostream>

// Constructor: Khởi tạo bảng băm với số bucket là `size`
HashTable::HashTable(int size) : size(size) {
    table.resize(size, nullptr);  // Mỗi bucket khởi tạo với giá trị nullptr
}

// Destructor: Giải phóng bộ nhớ cho tất cả các danh sách liên kết
HashTable::~HashTable() {
    for (int i = 0; i < size; i++) {
        Node* current = table[i];
        while (current != nullptr) {
            Node* temp = current;
            current = current->next;
            delete temp;
        }
    }
    table.clear();
}

// Hàm băm: Tính chỉ số bucket từ khóa bằng phép chia lấy dư cho 10
int HashTable::HashFunction(std::string key) {
    int hash = 0;
    for (char c : key) {
        hash += c;  // Cộng giá trị ASCII của từng ký tự
    }
    return hash % size;  // Lấy dư cho kích thước bảng băm
}

// Chèn một khóa vào bảng băm (nếu chưa tồn tại)
void HashTable::Insert(std::string key) {
    int index = HashFunction(key);

    // Kiểm tra xem key đã tồn tại chưa
    Node* current = table[index];
    while (current != nullptr) {
        if (current->key == key) {
            std::cout << "Key '" << key << "' already exists!\n";
            return;
        }
        current = current->next;
    }

    // Nếu chưa có, thêm vào đầu danh sách liên kết
    Node* newNode = new Node(key);
    newNode->next = table[index];
    table[index] = newNode;
}

// Tìm kiếm một khóa trong bảng băm
bool HashTable::Search(std::string key) {
    int index = HashFunction(key);
    Node* current = table[index];

    while (current != nullptr) {
        if (current->key == key) {
            return true;
        }
        current = current->next;
    }
    return false;
}

// Xóa một khóa khỏi bảng băm
void HashTable::Delete(std::string key) {
    int index = HashFunction(key);
    Node* current = table[index];
    Node* prev = nullptr;

    while (current != nullptr) {
        if (current->key == key) {
            if (prev == nullptr) {
                table[index] = current->next;  // Xóa phần tử đầu danh sách
            }
            else {
                prev->next = current->next;  // Xóa phần tử ở giữa hoặc cuối
            }
            delete current;
            return;
        }
        prev = current;
        current = current->next;
    }
}

// Vẽ bảng băm trên giao diện Raylib
void HashTable::Draw(float x, float y) {
    float bucketSpacing = 150.0f;  // Khoảng cách giữa các bucket
    float nodeSpacing = 60.0f;     // Khoảng cách giữa các node trong một bucket

    for (int i = 0; i < size; i++) {
        float bucketX = x + i * bucketSpacing;
        float bucketY = y;

        // Vẽ ô bucket
        DrawRectangle(bucketX, bucketY, 80, 40, LIGHTGRAY);
        DrawText(std::to_string(i).c_str(), bucketX + 30, bucketY + 10, 20, BLACK);

        float nodeY = bucketY + 50;
        Node* current = table[i];

        while (current != nullptr) {
            // Vẽ node
            DrawRectangle(bucketX + 10, nodeY, 60, 30, SKYBLUE);
            DrawText(current->key.c_str(), bucketX + 20, nodeY + 5, 20, BLACK);

            // Vẽ mũi tên nếu còn phần tử tiếp theo
            if (current->next != nullptr) {
                DrawLine(bucketX + 40, nodeY + 30, bucketX + 40, nodeY + nodeSpacing - 10, BLACK);
                DrawTriangle(
                    { bucketX + 35, nodeY + nodeSpacing - 10 },
                    { bucketX + 45, nodeY + nodeSpacing - 10 },
                    { bucketX + 40, nodeY + nodeSpacing },
                    BLACK
                );
            }

            nodeY += nodeSpacing;
            current = current->next;
        }
    }
}
