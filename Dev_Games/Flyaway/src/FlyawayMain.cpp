#include "FlyawayMain.hpp"

int main(void) {
    const int WINDOW_WIDTH = 820;
    const int WINDOW_HEIGHT = 1092;
    const Color backgroundColor{13, 13, 13, 255};
    const int NUM_FALLING_STONES = 20;
    const int NUM_ENEMIES = 4;
    const int MAX_BULLETS = 10;
    const int MAX_HEARTS = 6;
    const int MAX_HP = 5;
    bool isGaming = true;

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Flyaway");
    SetTargetFPS(60);

    Player player{{WINDOW_WIDTH / 2, WINDOW_HEIGHT - 50}};
    int hp = MAX_HP;

    vector<FallingStone> fallingStones;
    for (int i = 0; i < NUM_FALLING_STONES; ++i) {
        fallingStones.emplace_back(Vector2{(float)GetRandomValue(RADIUS_MAX, GetScreenWidth() - RADIUS_MAX), (float)GetRandomValue(-300, -100)}, GetRandomValue(4, 6));
    }
    vector<Enemy> enemies;
    enemies.resize(NUM_ENEMIES);
    for (auto& enemy : enemies) {
        enemy.Regenerate();
    }
    vector<Bullet> bullets;
    bullets.resize(MAX_BULLETS);
    vector<Heart> hearts;
    hearts.resize(MAX_HEARTS);

    int count, score = 0;
    float heartGenerateInterval = 10, heartGeneratePassTime = 0;
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        if (isGaming) {   
            // input
            player.Move();
            player.HandleShooting(bullets);
            // update
            player.Update();
            if (hp == MAX_HP && heartGeneratePassTime < abs(1e-3)) heartGeneratePassTime = 0;
            if (hp < MAX_HP) {
                heartGeneratePassTime += dt;
                if (heartGeneratePassTime > heartGenerateInterval)
                {
                    for (auto& heart : hearts) {
                        if (!heart.IsValid()) 
                        {
                            heart.Spawn({20.0f + GetRandomValue(0, GetScreenWidth() - 40), -500.0f + GetRandomValue(0, 300)});
                            heartGeneratePassTime = 0;
                            heartGenerateInterval = 10.0f + GetRandomValue(-3, 3);
                            break;
                        }
                    }
                }

                for (auto& heart : hearts) {
                    if (heart.IsValid() && player.PointInTriangle(heart.GetPosition())) {
                        if (hp < MAX_HP) hp++;
                        heart.GoInvalid();
                    }
                }
            }
            for (auto& enemy : enemies) {
                enemy.Update(dt, score);
                enemy.LightIfCondition(dt, 0.2f);
            }
            for (auto& heart : hearts) {
                heart.Update(dt);
            }
            for (auto& fs : fallingStones) {
                fs.Update(dt);
                if (!player.IsFlashing() && player.IfHitFallingStone(fs)) {
                    player.Flash();
                    if (--hp <= 0) isGaming = false;
                }
            }
            player.FlashIfCondition(dt, 1.0f);
            for (auto& bullet : bullets) {
                bullet.Update(dt);
                for (auto& fs : fallingStones) {
                    bullet.DetectAndDestroy(fs, score);
                }
                for (auto& enemy : enemies) {
                    bullet.DetectAndHarm(enemy);
                }
            }
            count = count_if(bullets.begin(), bullets.end(), [](Bullet& bullet) {
                return !bullet.IsValid();
            });
        } else {
            // input 2
            if (IsKeyPressed(KEY_SPACE)) {
                isGaming = true;
                hp = MAX_HP;
                score = 0;
                for (auto& fs : fallingStones) fs.Regenerate();
                for (auto& bullet : bullets) bullet.GoInvalid();
            }
        }
        // draw
        BeginDrawing();
        ClearBackground(backgroundColor);
        for (auto& heart : hearts) {
            heart.Draw();
        }
        for (auto& fs : fallingStones) {
            fs.Draw();
        }
        for (auto& bullet : bullets) {
            bullet.Draw();
        }
        for (auto& enemy : enemies) {
            enemy.Draw();
        }
        player.Draw();
        DrawText(to_string(hp).c_str(), WINDOW_WIDTH - 30, 5, 30, GREEN);
        DrawText(to_string(count).c_str(), WINDOW_WIDTH - 30, 40, 30, PINK);
        string scoreText = to_string(score);
        DrawText(scoreText.c_str(), WINDOW_WIDTH - MeasureText(scoreText.c_str(), 30) - 10, 75, 30, YELLOW);
        if (!isGaming) {
            DrawText("Game Over", 120, 400, 90, RED);
            DrawText("Press SPACE to restart!", 10, 700, 60, RED);
            DrawText((string("Your score is ") + to_string(score)).c_str(), 80, 900, 60, YELLOW);
        }
        DrawFPS(5, 2);
        EndDrawing();
    }

    return 0;
}