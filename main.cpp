#include "raylib.h"

float ClampValue(float value, float min, float max)
{
    if(value < min) value = min;
    if(value > max) value = max;
    return value;
}

int main()
{
    constexpr int screenWidth = 1280;
    constexpr int screenHeight = 720;
    constexpr int playerSize = 50;

    InitWindow(screenWidth, screenHeight, "Boss Battle");
    SetTargetFPS(60);

    Vector2 playerPos = {200.f, 500.f};
    const float playerSpeed = 300.0f;

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();

        //Move player
        if(IsKeyDown(KEY_RIGHT))
        {
            playerPos.x += playerSpeed * deltaTime;
        }

        if(IsKeyDown(KEY_LEFT))
        {
            playerPos.x -= playerSpeed * deltaTime;
        }

        if(IsKeyDown(KEY_UP))
        {
            playerPos.y -= playerSpeed * deltaTime;
        }

        if(IsKeyDown(KEY_DOWN))
        {
            playerPos.y += playerSpeed * deltaTime;
        }

        //Clamp player position to screen bounds
        // if(playerPos.x < 0.0f) playerPos.x = 0.0f;
        // if(playerPos.x > (float)(screenWidth - playerSize)) playerPos.x = (float)(screenWidth - playerSize);
        // if(playerPos.y < 0.0f) playerPos.y = 0.0f;
        // if(playerPos.y > (float)(screenHeight - playerSize)) playerPos.y = (float)(screenHeight - playerSize);
        playerPos.x = ClampValue(playerPos.x, 0.0f, (float)(screenWidth - playerSize));
        playerPos.y = ClampValue(playerPos.y, 0.0f, (float)(screenHeight - playerSize));

        //Drawing
        BeginDrawing();

        ClearBackground(RAYWHITE);
        DrawRectangle((int)playerPos.x, (int)playerPos.y, playerSize, playerSize, SKYBLUE);
        DrawText("Boss Battle", 20, 20, 24, BLACK);
        DrawFPS(20, 55);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}