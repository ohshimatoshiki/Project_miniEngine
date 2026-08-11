#include "raylib.h"
#include <cmath>

struct Bullet
{
    Vector2 bulletPos;
    bool bulletActive;
};

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
    constexpr int bulletSize = 5;

    InitWindow(screenWidth, screenHeight, "Boss Battle");
    SetTargetFPS(60);

    Vector2 playerPos = {200.f, 500.f};
    const float playerSpeed = 300.0f;

    const int maxBullets = 3;
    Bullet bullets[maxBullets];
    const float bulletSpeed = 1000.f;
    for(Bullet& bullet : bullets)
    {
        bullet.bulletPos = {0.0f, 0.0f};
        bullet.bulletActive = false;
    }

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();
        Vector2 moveDirection = {0.0f, 0.0f};

        //Get key input
        if(IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
        {
            moveDirection.x += 1.0f;
        }

        if(IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
        {
            moveDirection.x -= 1.0f;
        }

        if(IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
        {
            moveDirection.y -= 1.0f;
        }

        if(IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
        {
            moveDirection.y += 1.0f;
        }

        //Normalization
        float length = sqrtf((moveDirection.x * moveDirection.x) + (moveDirection.y * moveDirection.y));

        if(length > 0.0f){
            moveDirection.x = moveDirection.x / length;
            moveDirection.y = moveDirection.y / length;
        }

        //Move player
        playerPos.x += playerSpeed * moveDirection.x * deltaTime;
        playerPos.y += playerSpeed * moveDirection.y * deltaTime;

        //Clamp player position to screen bounds
        playerPos.x = ClampValue(playerPos.x, 0.0f, (float)(screenWidth - playerSize));
        playerPos.y = ClampValue(playerPos.y, 0.0f, (float)(screenHeight - playerSize));

        //ShootBullets
        if(IsKeyPressed(KEY_SPACE))
        {
            for(Bullet& bullet : bullets)
            {
                if(bullet.bulletActive) continue;
                TraceLog(LOG_INFO, "make bullet");

                bullet.bulletActive = true;

                bullet.bulletPos = playerPos;
                bullet.bulletPos.x += playerSize;
                bullet.bulletPos.y += (playerSize / 2.0f);
                break;
            }
        }
        //Move and remove bullets
        for(Bullet& bullet : bullets)
        {
            if(bullet.bulletActive)
            {
                bullet.bulletPos.x += bulletSpeed * deltaTime;
                TraceLog(LOG_INFO, "move bullet");
                if(bullet.bulletPos.x > screenWidth)
                {
                    bullet.bulletActive = false;
                    bullet.bulletPos = {0.0f, 0.0f};
                    TraceLog(LOG_INFO, "remove bullet");
                }
            }
        }

        //Drawing
        BeginDrawing();

        ClearBackground(RAYWHITE);
        DrawRectangle((int)playerPos.x, (int)playerPos.y, playerSize, playerSize, SKYBLUE);
        for(const Bullet& bullet : bullets)
        {
            if(bullet.bulletActive)
            {
                TraceLog(LOG_INFO, "draw bullet");
                DrawCircle((int)bullet.bulletPos.x, (int)bullet.bulletPos.y, bulletSize, BLACK);
            }
        }
        DrawText("Boss Battle", 20, 20, 24, BLACK);
        DrawFPS(20, 55);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}