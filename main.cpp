#include "raylib.h"
#include <cmath>
#include <random>

constexpr int screenWidth = 1280;
constexpr int screenHeight = 720;
constexpr int playerSize = 50;
constexpr int playerBulletSize = 5;
constexpr int normalBulletSize = 5;
constexpr int hardBulletSize = 10;
constexpr float normalPlayerSpeed = 300.0f;
constexpr float dashPlayerSpeed = 450.0f;
constexpr int maxBullets = 3;
constexpr float playerBulletSpeed = 1000.0f;
constexpr float bossBulletSpeed = 400.0f;
constexpr float normalBossShootInterval = 1.0f;
constexpr float hardBossShootInterval = 0.5f;
constexpr float bossChangeDirTimer = 1.0f;
constexpr float homingTime = 1.5f;
constexpr float homingTurnSpeed = PI / 2.0f;
constexpr int buttonWidth = 240;
constexpr int buttonHeight = 120;
constexpr int normalMoney = 50;
constexpr int hardMoney = 100;
constexpr Vector2 normalButtonPos = {300.0f, 240.0f};
constexpr Vector2 hardButtonPos = {740.0f, 240.0f};
constexpr Vector2 shopButtonPos = {740.0f, 480.0f};
constexpr Vector2 hpUpButtonPos = {200.0f, 240.0f};
constexpr Vector2 attackUpButtonPos = {540.0f, 240.0f};
constexpr Vector2 bossLevelUpButtonPos = {880.0f, 240.0f};

// Initial value
constexpr Vector2 initialPlayerPos = {200.0f, 500.0f};
constexpr Vector2 initialBossPos = {1000.0f, 240.0f};
constexpr int initialMaxPlayerHP = 3;
constexpr int initialAttackPower = 1;
constexpr int initialBossHP = 10;
constexpr float initialBossSpeed = 100.0f;
constexpr int initialBossWidth = 100;
constexpr int initialBossHeight = 180;

struct Bullet
{
    Vector2 bulletPos;
    Vector2 bulletDirection;
    int bulletSize;
    bool bulletActive;
    float homingTimer;
};
struct Player
{
    Vector2 playerPos;
    int playerHP;
    int maxHP;
    int attackPower;
    int money;
    Rectangle playerRec;
    Bullet playerBullets[maxBullets];
};

struct Boss
{
    Vector2 bossPos;
    float bossCurrentSpeed;
    Vector2 bossMoveDirection;
    float changeDirTimer;
    int bossHP;
    int maxBossHP;
    int bossAtk;
    int bossLevel;
    float bossWidth;
    float bossHeight;
    Rectangle bossRec;
    float shootTimer;
    Bullet bossBullets[maxBullets];
};

enum class GameState
{
    ModeSelect,
    Shop,
    Playing,
    Victory,
    GameOver
};

enum class GameMode
{
    Normal,
    Hard
};

// To use random number
std::random_device seed_gen;
std::uint32_t seed = seed_gen();
std::mt19937 engine(seed);


// Function prototype
void ResetGame(Player &player, Boss &boss, GameMode gameMode);
float ClampValue(float value, float bound1, float bound2);
void UpdatePlayer(Player &player, float deltaTime);
void UpdateBoss(Boss &boss, float deltaTime);
void InitializePlayerBullets(Bullet (&bullets)[maxBullets]);
void InitializeBossBullets(Bullet (&bullets)[maxBullets], GameMode gameMode);
void ShootPlayerBullets(Player &player);
void UpdatePlayerBullets(Player &player, Boss &boss, float deltaTime, bool collisionEnabled);
void DrawBullets(const Bullet (&bullets)[maxBullets], Color color);
void ShootBossBullets(const Player &player, Boss &boss, float deltaTime, GameMode gameMode);
void UpdateBossBullets(Player &player, Boss &boss, float deltaTime, bool collisionEnabled, GameMode gameMode);
void InitializePlayer(Player &player);
void InitializeBoss(Boss &boss, GameMode gameMode);
void ResetPlayer(Player &player);
void ResetBoss(Boss &boss, GameMode gameMode);
Vector2 NormalizeVector(Vector2 vector);
void UpgradeHP(Player &player);
void UpgradeAttack(Player &player);
void AddMoney(Player &player, Boss &boss, GameMode gameMode);
void bossLevelUp(Player &player, Boss &boss);


void ResetGame(Player &player, Boss &boss, GameMode gameMode)
{
    ResetPlayer(player);
    ResetBoss(boss, gameMode);
}

float ClampValue(float value, float bound1, float bound2)
{
    float min, max;
    if(bound1 < bound2)
    {
        min = bound1;
        max = bound2;
    }
    else
    {
        min = bound2;
        max = bound1;
    }
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
    moveDirection = NormalizeVector(moveDirection);

    // Move player
    player.playerPos.x += currentPlayerSpeed * moveDirection.x * deltaTime;
    player.playerPos.y += currentPlayerSpeed * moveDirection.y * deltaTime;

    // Clamp player position to screen bounds
    player.playerPos.x = ClampValue(player.playerPos.x, 0.0f, (float)(screenWidth - playerSize));
    player.playerPos.y = ClampValue(player.playerPos.y, 0.0f, (float)(screenHeight - playerSize));

    player.playerRec.x = player.playerPos.x;
    player.playerRec.y = player.playerPos.y;
}

void UpdateBoss(Boss &boss, float deltaTime)
{
    boss.changeDirTimer += deltaTime;
    if(boss.changeDirTimer >= bossChangeDirTimer)
    {
        std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

        boss.bossMoveDirection.y = dist(engine);

        boss.changeDirTimer = 0.0f;
    }

    // Normalization
    boss.bossMoveDirection = NormalizeVector(boss.bossMoveDirection);

    // Move player
    boss.bossPos.x += boss.bossCurrentSpeed * boss.bossMoveDirection.x * deltaTime;
    boss.bossPos.y += boss.bossCurrentSpeed * boss.bossMoveDirection.y * deltaTime;

    // Clamp player position to screen bounds
    boss.bossPos.x = ClampValue(boss.bossPos.x, 0.0f, (float)(screenWidth - boss.bossWidth));
    boss.bossPos.y = ClampValue(boss.bossPos.y, 0.0f, (float)(screenHeight - boss.bossHeight));

    boss.bossRec.x = boss.bossPos.x;
    boss.bossRec.y = boss.bossPos.y;
}

void InitializePlayerBullets(Bullet (&bullets)[maxBullets])
{
    for (Bullet &bullet : bullets)
    {
        bullet.bulletActive = false;
        bullet.bulletPos = {0.0f, 0.0f};
        bullet.bulletDirection = {0.0f, 0.0f};
        bullet.bulletSize = playerBulletSize;
        bullet.homingTimer = 0.0f;
    }
}

void InitializeBossBullets(Bullet (&bullets)[maxBullets], GameMode gameMode)
{
    for (Bullet &bullet : bullets)
    {
        bullet.bulletActive = false;
        bullet.bulletPos = {0.0f, 0.0f};
        bullet.bulletDirection = {0.0f, 0.0f};
        bullet.homingTimer = 0.0f;
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
                boss.bossHP -= player.attackPower;

                bullet.bulletActive = false;
                bullet.bulletPos = {0.0f, 0.0f};

                TraceLog(LOG_INFO, "Boss Hit! bossHP: %d / %d", boss.bossHP, boss.maxBossHP);
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
    boss.shootTimer += deltaTime;
    float bossShootInterval = normalBossShootInterval;

    if (gameMode == GameMode::Normal)
    {
        bossShootInterval = normalBossShootInterval;
    }
    else if (gameMode == GameMode::Hard)
    {
        bossShootInterval = hardBossShootInterval;
    }

    if (boss.shootTimer < bossShootInterval)
        return;

    boss.shootTimer = 0.0f;

    Vector2 direction;

    if (gameMode == GameMode::Normal)
    {
        direction = {-1.0f, 0.0f};
    }
    else
    {
        Vector2 bulletStartPos = {boss.bossPos.x, boss.bossPos.y + (boss.bossHeight / 2.0f)};
        Vector2 playerCenter = {player.playerPos.x + (playerSize / 2.0f), player.playerPos.y + (playerSize / 2.0f)};
        direction = {playerCenter.x - bulletStartPos.x, playerCenter.y - bulletStartPos.y};

        // normalization
        direction = NormalizeVector(direction);
    }

    // ShootBullets    
    for (Bullet &bullet : boss.bossBullets)
    {
        if (bullet.bulletActive)
            continue;

        bullet.bulletActive = true;
        bullet.bulletDirection = direction;
        bullet.bulletPos = boss.bossPos;
        bullet.bulletPos.y += (boss.bossHeight / 2.0f);

        break;
    }
}

void UpdateBossBullets(Player &player, Boss &boss, float deltaTime, bool collisionEnabled, GameMode gameMode)
{
    for (Bullet &bullet : boss.bossBullets)
    {
        if (bullet.bulletActive)
        {
            // horming
            if (gameMode == GameMode::Hard)
            {
                bullet.homingTimer += deltaTime;
                if (bullet.homingTimer < homingTime)
                {
                    Vector2 playerCenter = {player.playerPos.x + (playerSize / 2.0f), player.playerPos.y + (playerSize / 2.0f)};
                    Vector2 targetDirection = {playerCenter.x - bullet.bulletPos.x, playerCenter.y - bullet.bulletPos.y};

                    
                    targetDirection = NormalizeVector(targetDirection);
                    
                    //Limit the rotation angle
                    float currentAngle = atan2f(bullet.bulletDirection.y, bullet.bulletDirection.x);
                    float targetAngle = atan2f(targetDirection.y, targetDirection.x);
                    float angleDiff = targetAngle - currentAngle;

                    while(angleDiff > PI) angleDiff -= 2.0f * PI;
                    while(angleDiff < -PI) angleDiff += 2.0f * PI;

                    float maxTurn = homingTurnSpeed * deltaTime;
                    angleDiff = ClampValue(angleDiff, -maxTurn, maxTurn);

                    float newAngle = currentAngle + angleDiff;

                    bullet.bulletDirection = {cosf(newAngle), sinf(newAngle)};
                }
            }

            bullet.bulletPos.x += bullet.bulletDirection.x * bossBulletSpeed * deltaTime;
            bullet.bulletPos.y += bullet.bulletDirection.y * bossBulletSpeed * deltaTime;
            if (collisionEnabled && player.playerHP > 0 && CheckCollisionCircleRec(bullet.bulletPos, bullet.bulletSize, player.playerRec))
            {
                player.playerHP -= boss.bossAtk;

                bullet.bulletActive = false;
                bullet.bulletPos = {0.0f, 0.0f};
                bullet.homingTimer = 0.0f;

                TraceLog(LOG_INFO, "Player Hit! playerHP: %d / %d", player.playerHP, player.maxHP);
            }

            if (bullet.bulletPos.x < 0.0f || bullet.bulletPos.x > screenWidth || bullet.bulletPos.y < 0.0f || bullet.bulletPos.y > screenHeight)
            {
                bullet.bulletActive = false;
                bullet.bulletPos = {0.0f, 0.0f};
                bullet.homingTimer = 0.0f;
            }
        }
    }
}

void InitializePlayer(Player &player)
{
    player.playerPos = initialPlayerPos;
    player.maxHP = initialMaxPlayerHP;
    player.playerHP = player.maxHP;
    player.attackPower = initialAttackPower;
    player.money = 0;
    player.playerRec = {player.playerPos.x, player.playerPos.y, playerSize, playerSize};
    InitializePlayerBullets(player.playerBullets);

    TraceLog(LOG_INFO, "Initialize Player.");
}

void InitializeBoss(Boss &boss, GameMode gameMode)
{
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    boss.bossPos = initialBossPos;
    boss.bossCurrentSpeed = initialBossSpeed;
    boss.bossMoveDirection = {0.0f, dist(engine)};
    boss.maxBossHP = initialBossHP;
    boss.bossHP = boss.maxBossHP;
    boss.bossLevel = 1;
    boss.bossAtk = (1 + boss.bossLevel) / 2;
    boss.bossWidth = initialBossWidth;
    boss.bossHeight = initialBossHeight;
    boss.bossRec = {boss.bossPos.x, boss.bossPos.y, boss.bossWidth, boss.bossHeight};
    boss.shootTimer = 0.0f;
    boss.changeDirTimer = 0.0f;
    InitializeBossBullets(boss.bossBullets, gameMode);

    TraceLog(LOG_INFO, "Initialize Boss.");
}

void ResetPlayer(Player &player)
{
    player.playerPos = initialPlayerPos;
    player.playerHP = player.maxHP;
    player.playerRec = {player.playerPos.x, player.playerPos.y, playerSize, playerSize};
    InitializePlayerBullets(player.playerBullets);
    TraceLog(LOG_INFO, "Reset Player.");
}

void ResetBoss(Boss &boss, GameMode gameMode)
{
    boss.bossPos = initialBossPos;
    boss.bossHP = boss.maxBossHP;
    boss.bossRec = {boss.bossPos.x, boss.bossPos.y, boss.bossWidth, boss.bossHeight};
    boss.shootTimer = 0.0f;
    InitializeBossBullets(boss.bossBullets, gameMode);
    TraceLog(LOG_INFO, "Reset Boss.");
}

Vector2 NormalizeVector(Vector2 vector)
{
    float length = sqrt(vector.x * vector.x + vector.y * vector.y);
    if(length > 0.0f) vector = {vector.x / length, vector.y / length};
    return vector;
}

void UpgradeHP(Player &player)
{
    int cost  = 50 + (player.maxHP - 2) * 50;

    if(player.money < cost) return;

    player.money -= cost;
    player.maxHP += 1;

    TraceLog(LOG_INFO, "Upgrade HP. cost: %d, newHP: %d, Remaining Balance: %d", cost, player.maxHP, player.money);
}

void UpgradeAttack(Player &player)
{
    int cost  = 50 + player.attackPower * 50;

    if(player.money < cost) return;

    player.money -= cost;
    player.attackPower += 1;

    TraceLog(LOG_INFO, "Upgrade Attack. cost %d, newAtkPower: %d, Remaining Balance: %d", cost, player.attackPower, player.money);
}

void AddMoney(Player &player, Boss &boss, GameMode gameMode)
{
    int money;

    if(gameMode == GameMode::Normal)
    {
        money = normalMoney;
        player.money += money;
    }
    else if(gameMode == GameMode::Hard)
    {
        money = hardMoney * boss.bossLevel;
        player.money += money;
    }

    TraceLog(LOG_INFO, "Get money: %d, Balance: %d", money, player.money);
}

void bossLevelUp(Player &player, Boss &boss)
{
    int cost = 200 + 100 * boss.bossLevel;
    if(player.money < cost) return;

    //上げ幅は要調整
    boss.bossLevel += 1;
    boss.maxBossHP += boss.bossLevel * 2;
    boss.bossAtk = (1 + boss.bossLevel) / 2;

    //ボスの大きさ小さくする
    boss.bossHeight *= 0.8f;
    if(boss.bossHeight <= hardBulletSize * 2) boss.bossHeight = hardBulletSize * 2;
    boss.bossWidth *= 0.8f;
    if(boss.bossWidth <= hardBulletSize * 2) boss.bossWidth = hardBulletSize * 2;

    //お金減らす
    player.money -= cost;

    TraceLog(LOG_INFO, "Boss Level Up. newLv: %d, HP: %d, Atk: %d, Height: %.1f, Width: %.1f, cost: %d, Remaining Balance: %d", boss.bossLevel, boss.maxBossHP, boss.bossAtk, boss.bossHeight, boss.bossWidth, cost, player.money);
}


int main()
{
    InitWindow(screenWidth, screenHeight, "Boss Battle");
    SetTargetFPS(60);
    GameState gameState = GameState::ModeSelect;
    GameMode gameMode = GameMode::Normal;

    Rectangle normalButtonRec = {normalButtonPos.x, normalButtonPos.y, buttonWidth, buttonHeight};
    Rectangle hardButtonRec = {hardButtonPos.x, hardButtonPos.y, buttonWidth, buttonHeight};
    Rectangle shopButtonRec = {shopButtonPos.x, shopButtonPos.y, buttonWidth, buttonHeight};
    Rectangle hpUpButtonRec = {hpUpButtonPos.x, hpUpButtonPos.y, buttonWidth, buttonHeight};
    Rectangle attackUpButtonRec = {attackUpButtonPos.x, attackUpButtonPos.y, buttonWidth, buttonHeight};
    Rectangle bossLevelUpButtonRec = {bossLevelUpButtonPos.x, bossLevelUpButtonPos.y, buttonWidth, buttonHeight};

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
            ShootPlayerBullets(player);
            UpdatePlayerBullets(player, boss, deltaTime, false);
            if (CheckCollisionRecs(player.playerRec, normalButtonRec) && IsKeyPressed(KEY_ENTER))
            {
                gameState = GameState::Playing;
                gameMode = GameMode::Normal;                
                TraceLog(LOG_INFO, "===GameState changed. Modeselect -> Playing(Mode: %d)===", (int)gameMode);

                ResetGame(player, boss, gameMode);
            }
            else if (CheckCollisionRecs(player.playerRec, hardButtonRec) && IsKeyPressed(KEY_ENTER))
            {
                gameState = GameState::Playing;
                gameMode = GameMode::Hard;                
                TraceLog(LOG_INFO, "===GameState changed. Modeselect -> Playing(Mode: %d)===", (int)gameMode);

                ResetGame(player, boss, gameMode);
            }
            else if (CheckCollisionRecs(player.playerRec, shopButtonRec) && IsKeyPressed(KEY_ENTER))
            {
                gameState = GameState::Shop;
                TraceLog(LOG_INFO, "===GameState changed. Modeselect -> Shop===");

                //ResetGame(player, boss, gameMode);
                ResetPlayer(player);
            }
            break;
        case GameState::Shop:
            UpdatePlayer(player, deltaTime);
            ShootPlayerBullets(player);
            UpdatePlayerBullets(player, boss, deltaTime, false);
            if (CheckCollisionRecs(player.playerRec, hpUpButtonRec) && IsKeyPressed(KEY_ENTER))
            {
                UpgradeHP(player);
                //ResetGame(player, boss, gameMode);
                ResetPlayer(player);
            }
            else if (CheckCollisionRecs(player.playerRec, attackUpButtonRec) && IsKeyPressed(KEY_ENTER))
            {
                UpgradeAttack(player);
                //ResetGame(player, boss, gameMode);
                ResetPlayer(player);
            }
            else if (CheckCollisionRecs(player.playerRec, bossLevelUpButtonRec) && IsKeyPressed(KEY_ENTER))
            {
                bossLevelUp(player, boss);
                //ResetGame(player, boss, gameMode);
                ResetPlayer(player);
            }
            else if (CheckCollisionRecs(player.playerRec, shopButtonRec) && IsKeyPressed(KEY_ENTER))
            {
                gameState = GameState::ModeSelect;
                TraceLog(LOG_INFO, "===GameState changed. Shop -> ModeSelect===");

                //ResetGame(player, boss, gameMode);
                ResetPlayer(player);
            }
            break;
        case GameState::Playing:
            UpdatePlayer(player, deltaTime);
            UpdateBoss(boss, deltaTime);
            ShootPlayerBullets(player);
            ShootBossBullets(player, boss, deltaTime, gameMode);
            UpdatePlayerBullets(player, boss, deltaTime, true);
            UpdateBossBullets(player, boss, deltaTime, true, gameMode);
            // Check win/lose conditions
            if (player.playerHP <= 0)
            {
                gameState = GameState::GameOver;
                TraceLog(LOG_INFO, "===GameState changed. Playing -> GameOver===");
            }
            else if (boss.bossHP <= 0)
            {
                gameState = GameState::Victory;
                TraceLog(LOG_INFO, "===GameState changed. Playing -> Victory===");

                AddMoney(player, boss, gameMode);
            }
            break;
        case GameState::Victory:
            UpdatePlayer(player, deltaTime);
            ShootPlayerBullets(player);
            UpdatePlayerBullets(player, boss, deltaTime, false);
            UpdateBossBullets(player, boss, deltaTime, false, gameMode);
            if (IsKeyPressed(KEY_R))
            {
                gameState = GameState::ModeSelect;
                TraceLog(LOG_INFO, "===GameState changed. Victory -> ModeSelect===");

                ResetGame(player, boss, gameMode);
            }
            break;
        case GameState::GameOver:
            if (IsKeyPressed(KEY_R))
            {
                gameState = GameState::ModeSelect;
                TraceLog(LOG_INFO, "===GameState changed. GameOver -> ModeSelect===");

                ResetGame(player, boss, gameMode);
            }
            UpdatePlayerBullets(player, boss, deltaTime, false);
            UpdateBossBullets(player, boss, deltaTime, false, gameMode);
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
            DrawText("NORMAL", normalButtonPos.x + (buttonWidth / 4.0), normalButtonPos.y + (buttonHeight / 2.0), 32, BLACK);
            DrawRectangleRec(hardButtonRec, RED);
            DrawText("HARD", hardButtonPos.x + (buttonWidth / 4.0), hardButtonPos.y + (buttonHeight / 2.0), 32, BLACK);
            DrawRectangleRec(shopButtonRec, BLUE);
            DrawText("SHOP", shopButtonPos.x + (buttonWidth / 4.0), shopButtonPos.y + (buttonHeight / 2.0), 32, BLACK);
            DrawRectangleRec(player.playerRec, SKYBLUE);
            break;
        case GameState::Shop:
            DrawText("Select mode and press ENTER !", 300, 100, 36, BLACK);
            DrawRectangleRec(hpUpButtonRec, GREEN);
            DrawText("HP UP", hpUpButtonPos.x + (buttonWidth / 4.0), hpUpButtonPos.y + (buttonHeight / 2.0), 32, BLACK);
            DrawRectangleRec(attackUpButtonRec, RED);
            DrawText("ATK UP", attackUpButtonPos.x + (buttonWidth / 4.0), attackUpButtonPos.y + (buttonHeight / 2.0), 32, BLACK);
            DrawRectangleRec(bossLevelUpButtonRec, PURPLE);
            DrawText("BOSS LvUP", bossLevelUpButtonPos.x + (buttonWidth / 5.0), bossLevelUpButtonPos.y + (buttonHeight / 2.0), 32, BLACK);
            DrawRectangleRec(shopButtonRec, BLUE);
            DrawText("BACK", shopButtonPos.x + (buttonWidth / 4.0), shopButtonPos.y + (buttonHeight / 2.0), 32, BLACK);
            DrawRectangleRec(player.playerRec, SKYBLUE);
            break;
        case GameState::Playing:
            DrawRectangleRec(player.playerRec, SKYBLUE);
            DrawRectangleRec(boss.bossRec, RED);
            DrawText(TextFormat("Boss HP: %d", boss.bossHP), 1000, 200, 20, BLACK);
            break;
        case GameState::Victory:
            DrawText("Press R key to select mode !", 300, 100, 36, BLACK);
            DrawRectangleRec(player.playerRec, SKYBLUE);
            DrawText("Boss defeated!", 1000, 200, 20, BLACK);
            break;
        case GameState::GameOver:
            DrawText("Press R key to select mode !", 300, 100, 36, BLACK);
            DrawRectangleRec(boss.bossRec, RED);
            DrawText(TextFormat("Boss HP: %d", boss.bossHP), 1000, 200, 20, BLACK);
            DrawText("You Lose...", 300, 150, 36, BLACK);
            break;
        }
        // Draw common objects
        DrawBullets(player.playerBullets, BLACK);
        DrawBullets(boss.bossBullets, RED);
        DrawText("Boss Battle", 20, 20, 24, BLACK);
        DrawText(TextFormat("Player money: %d", player.money), 20, 80, 20, BLACK);
        DrawText(TextFormat("Player attack power: %d", player.attackPower), 20, 115, 20, BLACK);
        DrawText(TextFormat("Player HP: %d", player.playerHP), 20, 150, 20, BLACK);
        DrawFPS(20, 55);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}