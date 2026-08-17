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
constexpr float playerBulletSpeed = 1000.0f;
constexpr float bossBulletSpeed = 400.0f;
constexpr float bossShootInterval = 1.0f;
constexpr int buttonWidth = 240;
constexpr int buttonHeight = 120;
constexpr Vector2 normalButtonPos = {300.0f, 240.0f};
constexpr Vector2 hardButtonPos = {740.0f, 240.0f};

// Initial value
constexpr Vector2 initialPlayerPos = {200.0f, 500.0f};
constexpr Vector2 initialBossPos = {1000.0f, 240.0f};
constexpr int initialPlayerHP = 3;
constexpr int initialBossHP = 10;

struct Bullet
{
    Vector2 bulletPos;
    bool bulletActive;
};

enum class GameState
{
    ModeSelect,
    Playing,
    Victory,
    GameOver
};

// Function prototype
void ResetGame(Vector2 &playerPos, int &playerHP, Rectangle &playerRec, int &bossHP, Bullet (&playerBullets)[maxBullets], Bullet (&bossBullets)[maxBullets], float &bossShootTimer);
float ClampValue(float value, float min, float max);
void UpdatePlayer(Vector2 &playerPos, float deltaTime);
void InitializeBullets(Bullet (&bullets)[maxBullets]);
void ShootPlayerBullets(Bullet (&bullets)[maxBullets], const Vector2 &playerPos);
void UpdatePlayerBullets(Bullet (&bullets)[maxBullets], float deltaTime, int &bossHP, const Rectangle &bossRec, bool collisionEnabled);
void DrawBullets(const Bullet (&bullets)[maxBullets], Color color);
void ShootBossBullets(Bullet (&bullets)[maxBullets], const Vector2 &bossPos, float deltaTime, float &shootTimer);
void UpdateBossBullets(Bullet (&bullets)[maxBullets], float deltaTime, int &playerHP, const Rectangle &playerRec, bool collisionEnabled);

void ResetGame(Vector2 &playerPos, int &playerHP, Rectangle &playerRec, int &bossHP, Bullet (&playerBullets)[maxBullets], Bullet (&bossBullets)[maxBullets], float &bossShootTimer)
{
    playerPos = initialPlayerPos;
    playerHP = initialPlayerHP;
    playerRec = {playerPos.x, playerPos.y, playerSize, playerSize};
    bossHP = initialBossHP;
    InitializeBullets(playerBullets);
    InitializeBullets(bossBullets);
    bossShootTimer = 0.0f;
}

float ClampValue(float value, float min, float max)
{
    if (value < min)
        value = min;
    if (value > max)
        value = max;
    return value;
}

void UpdatePlayer(Vector2 &playerPos, float deltaTime)
{
    float currentPlayerSpeed = normalPlayerSpeed;
    Vector2 moveDirection = {0.0f, 0.0f};

    // Get key input
    if (IsKeyDown(KEY_LEFT_SHIFT))
    {
        currentPlayerSpeed = dashPlayerSpeed;
    }

    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
    {
        moveDirection.x += 1.0f;
    }

    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
    {
        moveDirection.x -= 1.0f;
    }

    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
    {
        moveDirection.y -= 1.0f;
    }

    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
    {
        moveDirection.y += 1.0f;
    }

    // Normalization
    float length = sqrtf((moveDirection.x * moveDirection.x) + (moveDirection.y * moveDirection.y));

    if (length > 0.0f)
    {
        moveDirection.x = moveDirection.x / length;
        moveDirection.y = moveDirection.y / length;
    }

    // Move player
    playerPos.x += currentPlayerSpeed * moveDirection.x * deltaTime;
    playerPos.y += currentPlayerSpeed * moveDirection.y * deltaTime;

    // Clamp player position to screen bounds
    playerPos.x = ClampValue(playerPos.x, 0.0f, (float)(screenWidth - playerSize));
    playerPos.y = ClampValue(playerPos.y, 0.0f, (float)(screenHeight - playerSize));
}

void InitializeBullets(Bullet (&bullets)[maxBullets])
{
    for (Bullet &bullet : bullets)
    {
        bullet.bulletActive = false;
        bullet.bulletPos = {0.0f, 0.0f};
    }
}

void ShootPlayerBullets(Bullet (&bullets)[maxBullets], const Vector2 &playerPos)
{
    // ShootBullets
    if (IsKeyPressed(KEY_SPACE))
    {
        for (Bullet &bullet : bullets)
        {
            if (bullet.bulletActive)
                continue;

            bullet.bulletActive = true;

            bullet.bulletPos = playerPos;
            bullet.bulletPos.x += playerSize;
            bullet.bulletPos.y += (playerSize / 2.0f);
            break;
        }
    }
}

void UpdatePlayerBullets(Bullet (&bullets)[maxBullets], float deltaTime, int &bossHP, const Rectangle &bossRec, bool collisionEnabled)
{
    // Move and remove bullets
    for (Bullet &bullet : bullets)
    {
        if (bullet.bulletActive)
        {
            bullet.bulletPos.x += playerBulletSpeed * deltaTime;
            if (collisionEnabled && bossHP > 0 && CheckCollisionCircleRec(bullet.bulletPos, bulletSize, bossRec))
            {
                bossHP--;

                bullet.bulletActive = false;
                bullet.bulletPos = {0.0f, 0.0f};
            }

            if (bullet.bulletPos.x > screenWidth)
            {
                bullet.bulletActive = false;
                bullet.bulletPos = {0.0f, 0.0f};
            }
        }
    }
}

void DrawBullets(const Bullet (&bullets)[maxBullets], Color color)
{
    for (const Bullet &bullet : bullets)
    {
        if (bullet.bulletActive)
        {
            DrawCircle((int)bullet.bulletPos.x, (int)bullet.bulletPos.y, bulletSize, color);
        }
    }
}

void ShootBossBullets(Bullet (&bullets)[maxBullets], const Vector2 &bossPos, float deltaTime, float &shootTimer)
{
    // ShootBullets
    shootTimer += deltaTime;

    if (shootTimer < bossShootInterval)
        return;

    shootTimer = 0.0f;
    for (Bullet &bullet : bullets)
    {
        if (bullet.bulletActive)
            continue;

        bullet.bulletActive = true;

        bullet.bulletPos = bossPos;
        bullet.bulletPos.y += (bossHeight / 2.0f);
        break;
    }
}

void UpdateBossBullets(Bullet (&bullets)[maxBullets], float deltaTime, int &playerHP, const Rectangle &playerRec, bool collisionEnabled)
{
    for (Bullet &bullet : bullets)
    {
        if (bullet.bulletActive)
        {
            bullet.bulletPos.x -= bossBulletSpeed * deltaTime;
            if (collisionEnabled && playerHP > 0 && CheckCollisionCircleRec(bullet.bulletPos, bulletSize, playerRec))
            {
                playerHP--;

                bullet.bulletActive = false;
                bullet.bulletPos = {0.0f, 0.0f};
            }

            if (bullet.bulletPos.x < 0.0f)
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
    GameState gameState = GameState::ModeSelect;

    Rectangle normalButtonRec = {normalButtonPos.x, normalButtonPos.y, buttonWidth, buttonHeight};
    Rectangle hardButtonRec = {hardButtonPos.x, hardButtonPos.y, buttonWidth, buttonHeight};
    Vector2 playerPos = initialPlayerPos;
    int playerHP = initialPlayerHP;
    Rectangle playerRec = {playerPos.x, playerPos.y, playerSize, playerSize};
    Vector2 bossPos = initialBossPos;
    int bossHP = initialBossHP;
    Rectangle bossRec = {bossPos.x, bossPos.y, bossWidth, bossHeight};
    Bullet playerBullets[maxBullets];
    Bullet bossBullets[maxBullets];
    InitializeBullets(playerBullets);
    InitializeBullets(bossBullets);
    float bossShootTimer = 0.0f;

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();

        // Update game logic based on current state
        switch (gameState)
        {
        case GameState::ModeSelect:
            UpdatePlayer(playerPos, deltaTime);
            playerRec.x = playerPos.x;
            playerRec.y = playerPos.y;
            if (CheckCollisionRecs(playerRec, normalButtonRec) && IsKeyPressed(KEY_ENTER))
            {
                gameState = GameState::Playing;
                ResetGame(playerPos, playerHP, playerRec, bossHP, playerBullets, bossBullets, bossShootTimer);
            }
            else if (CheckCollisionRecs(playerRec, hardButtonRec) && IsKeyPressed(KEY_ENTER))
            {
                gameState = GameState::Playing;
                ResetGame(playerPos, playerHP, playerRec, bossHP, playerBullets, bossBullets, bossShootTimer);
            }
            break;
        case GameState::Playing:
            UpdatePlayer(playerPos, deltaTime);
            playerRec.x = playerPos.x;
            playerRec.y = playerPos.y;
            ShootPlayerBullets(playerBullets, playerPos);
            ShootBossBullets(bossBullets, bossPos, deltaTime, bossShootTimer);
            UpdatePlayerBullets(playerBullets, deltaTime, bossHP, bossRec, true);
            UpdateBossBullets(bossBullets, deltaTime, playerHP, playerRec, true);
            // Check win/lose conditions
            if (playerHP <= 0)
            {
                gameState = GameState::GameOver;
            }
            else if (bossHP <= 0)
            {
                gameState = GameState::Victory;
            }
            break;
        case GameState::Victory:
            UpdatePlayer(playerPos, deltaTime);
            playerRec.x = playerPos.x;
            playerRec.y = playerPos.y;
            ShootPlayerBullets(playerBullets, playerPos);
            UpdatePlayerBullets(playerBullets, deltaTime, bossHP, bossRec, false);
            UpdateBossBullets(bossBullets, deltaTime, playerHP, playerRec, false);
            if (IsKeyPressed(KEY_R))
            {
                gameState = GameState::ModeSelect;
                ResetGame(playerPos, playerHP, playerRec, bossHP, playerBullets, bossBullets, bossShootTimer);
            }
            break;
        case GameState::GameOver:
            if (IsKeyPressed(KEY_R))
            {
                gameState = GameState::ModeSelect;
                ResetGame(playerPos, playerHP, playerRec, bossHP, playerBullets, bossBullets, bossShootTimer);
            }
            UpdatePlayerBullets(playerBullets, deltaTime, bossHP, bossRec, false);
            UpdateBossBullets(bossBullets, deltaTime, playerHP, playerRec, false);
            break;
        }

        // Drawing
        BeginDrawing();

        ClearBackground(RAYWHITE);

        // Draw scene based on current state
        switch (gameState)
        {
        case GameState::ModeSelect:
            DrawText("Select mode and press ENTER !", 300, 100, 36, BLACK);
            DrawRectangleRec(normalButtonRec, GREEN);
            DrawText("normal", normalButtonPos.x + (buttonWidth / 2.0), normalButtonPos.y + (buttonHeight / 2.0), 32, BLACK);
            DrawRectangleRec(hardButtonRec, RED);
            DrawText("hard", hardButtonPos.x + (buttonWidth / 2.0), hardButtonPos.y + (buttonHeight / 2.0), 32, BLACK);
            DrawRectangle((int)playerPos.x, (int)playerPos.y, playerSize, playerSize, SKYBLUE);
            break;
        case GameState::Playing:
            DrawText(TextFormat("Player HP: %d", playerHP), 20, 80, 20, BLACK);
            DrawRectangle((int)playerPos.x, (int)playerPos.y, playerSize, playerSize, SKYBLUE);
            DrawText(TextFormat("Boss HP: %d", bossHP), 1000, 200, 20, BLACK);
            DrawRectangleRec(bossRec, RED);
            break;
        case GameState::Victory:
            DrawText("Press R key to select mode !", 300, 100, 36, BLACK);
            DrawText(TextFormat("Player HP: %d", playerHP), 20, 80, 20, BLACK);
            DrawRectangleRec(playerRec, SKYBLUE);
            DrawText("Boss defeated!", 1000, 200, 20, BLACK);
            break;
        case GameState::GameOver:
            DrawText("Press R key to select mode !", 300, 100, 36, BLACK);
            DrawText(TextFormat("Boss HP: %d", bossHP), 1000, 200, 20, BLACK);
            DrawRectangleRec(bossRec, RED);
            DrawText("You Lose...", 20, 80, 20, BLACK);
            break;
        }
        // Draw common objects
        DrawBullets(playerBullets, BLACK);
        DrawBullets(bossBullets, RED);
        DrawText("Boss Battle", 20, 20, 24, BLACK);
        DrawFPS(20, 55);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}