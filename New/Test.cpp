#include "Application.h"

int main() {
    InitWindow(2000, 1000, "Hash Table with Chaining");
    SetTargetFPS(60);

    Application app(2000, 1000);

    while (!WindowShouldClose()) {
        app.Update();
        BeginDrawing();
        app.Draw();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
