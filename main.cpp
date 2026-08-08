#include "raylib.h"

int main()
{
    constexpr int screenWidth = 1280;
    constexpr int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "Boss Battle");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(BLACK);
        DrawRectangle(100, 300, 50, 50, SKYBLUE);
        DrawText("Boss Battle", 20, 20, 24, WHITE);
        DrawFPS(20, 55);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}