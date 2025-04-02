#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <vector>
#include <string>
#include "raylib.h"

using namespace std;

class HashTable {
public:
    struct Node {
        std::string key;
        Node* next;
        Node(std::string k) : key(k), next(nullptr) {}
    };

    std::vector<Node*> table;
    int size;

    HashTable(int size);
    ~HashTable();
    int HashFunction(std::string key);
    void Insert(std::string key);
    bool Search(std::string key);
    void Draw(float x, float y);
	void Delete(std::string key);
};

#endif
