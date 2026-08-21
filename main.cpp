#include "raylib.h"
#include <cmath>

constexpr int screenWidth = 1280;
constexpr int screenHeight = 720;
constexpr int playerSize = 50;
constexpr int playerBulletSize = 5;
constexpr int normalBulletSize = 5;
constexpr int hardBulletSize = 10;
constexpr int bossWidth = 100;
constexpr int bossHeight = 180;
constexpr float normalPlayerSpeed = 300.0f;
constexpr float dashPlayerSpeed = 450.0f;
constexpr int maxBullets = 3;
constexpr float playerBulletSpeed = 1000.0f;
constexpr float bossBulletSpeed = 400.0f;
constexpr float normalBossShootInterval = 1.0f;
constexpr float hardBossShootInterval = 0.5f;
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
    Vector2 bulletDirection;
    int bulletSize;
    bool bulletActive;
};
struct Player
{
    Vector2 playerPos;
    int playerHP;
    Rectangle playerRec;
    Bullet playerBullets[maxBullets];
};

struct Boss
{
    Vector2 bossPos;
    int bossHP;
    Rectangle bossRec;
    float shootTimer;
    Bullet bossBullets[maxBullets];
};

enum class GameState
{
    ModeSelect,
    Playing,
    Victory,
    GameOver
};

enum class GameMode
{
    Normal,
    Hard
};

// Function prototype
void ResetGame(Player &player, Boss &boss, GameMode gameMode);
float ClampValue(float value, float min, float max);
void UpdatePlayer(Player &player, float deltaTime);
void InitializePlayerBullets(Bullet (&bullets)[maxBullets]);
void InitializeBossBullets(Bullet (&bullets)[maxBullets], GameMode gameMode);
void ShootPlayerBullets(Player &player);
void UpdatePlayerBullets(Player &player, Boss &boss, float deltaTime, bool collisionEnabled);
void DrawBullets(const Bullet (&bullets)[maxBullets], Color color);
void ShootBossBullets(const Player &player, Boss &boss, float deltaTime, GameMode gameMode);
void UpdateBossBullets(Player &player, Boss &boss, float deltaTime, bool collisionEnabled);
void InitializePlayer(Player &player);
void InitializeBoss(Boss &boss, GameMode gameMode);

void ResetGame(Player &player, Boss &boss, GameMode gameMode)
{
    player.playerPos = initialPlayerPos;
    player.playerHP = initialPlayerHP;
    player.playerRec = {player.playerPos.x, player.playerPos.y, playerSize, playerSize};
    boss.bossHP = initialBossHP;
    InitializePlayerBullets(player.playerBullets);
    InitializeBossBullets(boss.bossBullets, gameMode);
    boss.shootTimer = 0.0f;
}

float ClampValue(float value, float min, float max)
{
    if (value < min)
        value = min;
    if (value > max)
        value = max;
    return value;
}

void UpdatePlayer(Player &player, float deltaTime)
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
    player.playerPos.x += currentPlayerSpeed * moveDirection.x * deltaTime;
    player.playerPos.y += currentPlayerSpeed * moveDirection.y * deltaTime;

    // Clamp player position to screen bounds
    player.playerPos.x = ClampValue(player.playerPos.x, 0.0f, (float)(screenWidth - playerSize));
    player.playerPos.y = ClampValue(player.playerPos.y, 0.0f, (float)(screenHeight - playerSize));

    player.playerRec.x = player.playerPos.x;
    player.playerRec.y = player.playerPos.y;
}

void InitializePlayerBullets(Bullet (&bullets)[maxBullets])
{
    for (Bullet &bullet : bullets)
    {
        bullet.bulletActive = false;
        bullet.bulletPos = {0.0f, 0.0f};
        bullet.bulletDirection = {0.0f, 0.0f};
        bullet.bulletSize = playerBulletSize;
    }
}

void InitializeBossBullets(Bullet (&bullets)[maxBullets], GameMode gameMode)
{
    for (Bullet &bullet : bullets)
    {
        bullet.bulletActive = false;
        bullet.bulletPos = {0.0f, 0.0f};
        bullet.bulletDirection = {0.0f, 0.0f};
        switch (gameMode)
        {
        case GameMode::Normal:
            bullet.bulletSize = normalBulletSize;
            break;
        case GameMode::Hard:
            bullet.bulletSize = hardBulletSize;
            break;
        }
    }
}

void ShootPlayerBullets(Player &player)
{
    // ShootBullets
    if (IsKeyPressed(KEY_SPACE))
    {
        for (Bullet &bullet : player.playerBullets)
        {
            if (bullet.bulletActive)
                continue;

            bullet.bulletActive = true;

            bullet.bulletPos = player.playerPos;
            bullet.bulletPos.x += playerSize;
            bullet.bulletPos.y += (playerSize / 2.0f);
            break;
        }
    }
}

void UpdatePlayerBullets(Player &player, Boss &boss, float deltaTime, bool collisionEnabled)
{
    // Move and remove bullets
    for (Bullet &bullet : player.playerBullets)
    {
        if (bullet.bulletActive)
        {
            bullet.bulletPos.x += playerBulletSpeed * deltaTime;
            if (collisionEnabled && boss.bossHP > 0 && CheckCollisionCircleRec(bullet.bulletPos, bullet.bulletSize, boss.bossRec))
            {
                boss.bossHP--;

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
            DrawCircle((int)bullet.bulletPos.x, (int)bullet.bulletPos.y, bullet.bulletSize, color);
        }
    }
}

void ShootBossBullets(const Player &player, Boss &boss, float deltaTime, GameMode gameMode)
{
    Vector2 direction;

    if (gameMode == GameMode::Normal)
    {
        direction = {-1.0f, 0.0f};
    }
    else
    {
        Vector2 bulletStartPos = {boss.bossPos.x, boss.bossPos.y + (bossHeight / 2.0f)};
        Vector2 playerCenter = {player.playerPos.x + (playerSize / 2.0f), player.playerPos.y + (playerSize / 2.0f)};
        direction = {playerCenter.x - bulletStartPos.x, playerCenter.y - bulletStartPos.y};

        // normalization
        float length = sqrt(direction.x * direction.x + direction.y * direction.y);
        if (length > 0.0f)
        {
            direction.x = direction.x / length;
            direction.y = direction.y / length;
        }
    }

    // ShootBullets
    boss.shootTimer += deltaTime;
    float bossShootInterval = normalBossShootInterval;
    if(gameMode == GameMode::Normal)
    {
        bossShootInterval = normalBossShootInterval;
    }
    else if(gameMode == GameMode::Hard)
    {
        bossShootInterval = hardBossShootInterval;
    }

    if (boss.shootTimer < bossShootInterval)
        return;

    boss.shootTimer = 0.0f;
    for (Bullet &bullet : boss.bossBullets)
    {
        if (bullet.bulletActive)
            continue;

        bullet.bulletActive = true;
        bullet.bulletDirection = direction;
        bullet.bulletPos = boss.bossPos;
        bullet.bulletPos.y += (bossHeight / 2.0f);

        break;
    }
}

void UpdateBossBullets(Player &player, Boss &boss, float deltaTime, bool collisionEnabled)
{
    for (Bullet &bullet : boss.bossBullets)
    {
        if (bullet.bulletActive)
        {
            bullet.bulletPos.x += bullet.bulletDirection.x * bossBulletSpeed * deltaTime;
            bullet.bulletPos.y += bullet.bulletDirection.y * bossBulletSpeed * deltaTime;
            if (collisionEnabled && player.playerHP > 0 && CheckCollisionCircleRec(bullet.bulletPos, bullet.bulletSize, player.playerRec))
            {
                player.playerHP--;

                bullet.bulletActive = false;
                bullet.bulletPos = {0.0f, 0.0f};
            }

            if (bullet.bulletPos.x < 0.0f || bullet.bulletPos.x > screenWidth || bullet.bulletPos.y < 0.0f || bullet.bulletPos.y > screenHeight)
            {
                bullet.bulletActive = false;
                bullet.bulletPos = {0.0f, 0.0f};
            }
        }
    }
}

void InitializePlayer(Player& player)
{
    player.playerPos = initialPlayerPos;
    player.playerHP = initialPlayerHP;
    player.playerRec = {player.playerPos.x, player.playerPos.y, playerSize, playerSize};
    InitializePlayerBullets(player.playerBullets);
}

void InitializeBoss(Boss& boss, GameMode gameMode)
{
    boss.bossPos = initialBossPos;
    boss.bossHP = initialBossHP;
    boss.bossRec = {boss.bossPos.x, boss.bossPos.y, bossWidth, bossHeight};
    boss.shootTimer = 0.0f;
    InitializeBossBullets(boss.bossBullets, gameMode);
}


int main()
{
    InitWindow(screenWidth, screenHeight, "Boss Battle");
    SetTargetFPS(60);
    GameState gameState = GameState::ModeSelect;
    GameMode gameMode = GameMode::Normal;

    Rectangle normalButtonRec = {normalButtonPos.x, normalButtonPos.y, buttonWidth, buttonHeight};
    Rectangle hardButtonRec = {hardButtonPos.x, hardButtonPos.y, buttonWidth, buttonHeight};
    
    Player player;
    Boss boss;
    InitializePlayer(player);
    InitializeBoss(boss, gameMode);

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();

        // Update game logic based on current state
        switch (gameState)
        {
        case GameState::ModeSelect:
            UpdatePlayer(player, deltaTime);
            if (CheckCollisionRecs(player.playerRec, normalButtonRec) && IsKeyPressed(KEY_ENTER))
            {
                gameState = GameState::Playing;
                gameMode = GameMode::Normal;
                ResetGame(player, boss, gameMode);
            }
            else if (CheckCollisionRecs(player.playerRec, hardButtonRec) && IsKeyPressed(KEY_ENTER))
            {
                gameState = GameState::Playing;
                gameMode = GameMode::Hard;
                ResetGame(player, boss, gameMode);
            }
            break;
        case GameState::Playing:
            UpdatePlayer(player, deltaTime);
            ShootPlayerBullets(player);
            ShootBossBullets(player, boss, deltaTime, gameMode);
            UpdatePlayerBullets(player, boss, deltaTime, true);
            UpdateBossBullets(player, boss, deltaTime, true);
            // Check win/lose conditions
            if (player.playerHP <= 0)
            {
                gameState = GameState::GameOver;
            }
            else if (boss.bossHP <= 0)
            {
                gameState = GameState::Victory;
            }
            break;
        case GameState::Victory:
            UpdatePlayer(player, deltaTime);
            ShootPlayerBullets(player);
            UpdatePlayerBullets(player, boss, deltaTime, false);
            UpdateBossBullets(player, boss, deltaTime, false);
            if (IsKeyPressed(KEY_R))
            {
                gameState = GameState::ModeSelect;
                ResetGame(player, boss, gameMode);
            }
            break;
        case GameState::GameOver:
            if (IsKeyPressed(KEY_R))
            {
                gameState = GameState::ModeSelect;
                ResetGame(player, boss, gameMode);
            }
            UpdatePlayerBullets(player, boss, deltaTime, false);
            UpdateBossBullets(player, boss, deltaTime, false);
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
            DrawRectangleRec(player.playerRec, SKYBLUE);
            break;
        case GameState::Playing:
            DrawText(TextFormat("Player HP: %d", player.playerHP), 20, 80, 20, BLACK);
            DrawRectangleRec(player.playerRec, SKYBLUE);
            DrawText(TextFormat("Boss HP: %d", boss.bossHP), 1000, 200, 20, BLACK);
            DrawRectangleRec(boss.bossRec, RED);
            break;
        case GameState::Victory:
            DrawText("Press R key to select mode !", 300, 100, 36, BLACK);
            DrawText(TextFormat("Player HP: %d", player.playerHP), 20, 80, 20, BLACK);
            DrawRectangleRec(player.playerRec, SKYBLUE);
            DrawText("Boss defeated!", 1000, 200, 20, BLACK);
            break;
        case GameState::GameOver:
            DrawText("Press R key to select mode !", 300, 100, 36, BLACK);
            DrawText(TextFormat("Boss HP: %d", boss.bossHP), 1000, 200, 20, BLACK);
            DrawRectangleRec(boss.bossRec, RED);
            DrawText("You Lose...", 20, 80, 20, BLACK);
            break;
        }
        // Draw common objects
        DrawBullets(player.playerBullets, BLACK);
        DrawBullets(boss.bossBullets, RED);
        DrawText("Boss Battle", 20, 20, 24, BLACK);
        DrawFPS(20, 55);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}