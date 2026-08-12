#include "raylib.h"
#include <cmath>

constexpr int screenWidth = 1280;
constexpr int screenHeight = 720;
constexpr int playerSize = 50;
constexpr int bulletSize = 5;
constexpr int bossWidth = 100;
constexpr int bossHeight = 180;
constexpr float normalPlayerSpeed = 300.0f;
constexpr float dashPlayerSpeed = 450.0f;
constexpr int maxBullets = 3;
constexpr float bulletSpeed = 1000.f;

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

void UpdatePlayer(Vector2& playerPos, float deltaTime)
{
    float currentPlayerSpeed = normalPlayerSpeed;
    Vector2 moveDirection = {0.0f, 0.0f};

    //Get key input
    if(IsKeyDown(KEY_LEFT_SHIFT))
    {
        currentPlayerSpeed = dashPlayerSpeed;
    }

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
    playerPos.x += currentPlayerSpeed * moveDirection.x * deltaTime;
    playerPos.y += currentPlayerSpeed * moveDirection.y * deltaTime;

    //Clamp player position to screen bounds
    playerPos.x = ClampValue(playerPos.x, 0.0f, (float)(screenWidth - playerSize));
    playerPos.y = ClampValue(playerPos.y, 0.0f, (float)(screenHeight - playerSize));
}

void ShootBullets(Bullet (&bullets)[maxBullets], const Vector2& playerPos)
{
    //ShootBullets
    if(IsKeyPressed(KEY_SPACE))
    {
        for(Bullet& bullet : bullets)
        {
            if(bullet.bulletActive) continue;

            bullet.bulletActive = true;

            bullet.bulletPos = playerPos;
            bullet.bulletPos.x += playerSize;
            bullet.bulletPos.y += (playerSize / 2.0f);
            break;
        }
    }
}

void UpdateBullets(Bullet (&bullets)[maxBullets], float deltaTime, int& bossHP, const Rectangle& bossRec)
{
    //Move and remove bullets
    for(Bullet& bullet : bullets)
    {
        if(bullet.bulletActive)
        {
            bullet.bulletPos.x += bulletSpeed * deltaTime;
            if(bossHP > 0 && CheckCollisionCircleRec(bullet.bulletPos, bulletSize, bossRec))
            {
                bossHP--;
                    
                bullet.bulletActive = false;
                bullet.bulletPos = {0.0f, 0.0f};
            }

            if(bullet.bulletPos.x > screenWidth)
            {
                bullet.bulletActive = false;
                bullet.bulletPos = {0.0f, 0.0f};
            }
        }
    }
}

int main()
{
    InitWindow(screenWidth, screenHeight, "Boss Battle");
    SetTargetFPS(60);

    Vector2 playerPos = {200.f, 500.f};
    
    Vector2 bossPos = {1000.0f, 240.0f};
    int bossHP = 10;
    Rectangle bossRec = {bossPos.x, bossPos.y, bossWidth, bossHeight};

    Bullet bullets[maxBullets];
    for(Bullet& bullet : bullets)
    {
        bullet.bulletPos = {0.0f, 0.0f};
        bullet.bulletActive = false;
    }

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();

        UpdatePlayer(playerPos, deltaTime);        
        ShootBullets(bullets, playerPos);
        UpdateBullets(bullets, deltaTime, bossHP, bossRec);

        //Drawing
        BeginDrawing();

        ClearBackground(RAYWHITE);
        DrawRectangle((int)playerPos.x, (int)playerPos.y, playerSize, playerSize, SKYBLUE);
        for(const Bullet& bullet : bullets)
        {
            if(bullet.bulletActive)
            {
                DrawCircle((int)bullet.bulletPos.x, (int)bullet.bulletPos.y, bulletSize, BLACK);
            }
        }
        DrawText("Boss Battle", 20, 20, 24, BLACK);
        if(bossHP > 0)
        {
            DrawText(TextFormat("Boss HP: %d", bossHP), 1000, 200, 20, BLACK);
            DrawRectangleRec(bossRec, RED);
        }
        else
        {
            DrawText("Boss defeated!", 1000, 200, 20, BLACK);
        }
        DrawFPS(20, 55);

        EndDrawing();

    }

    CloseWindow();
    return 0;
}