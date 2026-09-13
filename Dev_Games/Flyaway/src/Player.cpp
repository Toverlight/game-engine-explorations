#include "Player.hpp"
#include "FallingStone.hpp"
#include "Bullet.hpp"

void Player::Draw()
{
    Vector2 drawVertices[3]{0};
    for (int i = 0; i < 3; i++) {
        drawVertices[i].x = position.x + bodyVerticesOffsets[i].x;
        drawVertices[i].y = position.y + bodyVerticesOffsets[i].y;
    }
    DrawTriangle(drawVertices[0], drawVertices[1], drawVertices[2], statusColor);
}

void Player::Move()
{
    if (IsKeyDown(KEY_LEFT)) {
        velocity.x = -15;
    } else if (IsKeyDown(KEY_RIGHT)) {
        velocity.x = 15;
    } else if (IsKeyUp(KEY_LEFT) || IsKeyUp(KEY_RIGHT)) {
        velocity.x = 0;
    }
}

void Player::Update()
{
    position.x += velocity.x;
    // position.y += velocity.y;
    if (position.x + bodyVerticesOffsets[LEFT].x < 0)
        position.x = -bodyVerticesOffsets[LEFT].x;
    else if (position.x + bodyVerticesOffsets[RIGHT].x > GetScreenWidth())
        position.x = GetScreenWidth() - bodyVerticesOffsets[RIGHT].x;

}

bool Player::IfHitFallingStone(FallingStone& fs)
{
    for (int i = 0; i < 3; i++) {
        if (fs.PointInside(
            {(position.x + bodyVerticesOffsets[i].x), (position.y + bodyVerticesOffsets[i].y)}
        ))    return true;
    }
    return false;
}

void Player::FlashIfCondition(float dt, float flashTime)
{
    const float interval = 0.15f;
    static bool isWhite = false;
    static float passTime = 0;
    static float flashPassTime = 0;
    if (!isFlashing) return;

    passTime += dt;
    flashPassTime += dt;

    if (passTime > flashTime) {
        isFlashing = false;
        passTime = 0;
        GoWhite();
        isWhite = false;
        return;
    }
    if (flashPassTime > interval) {
        if (isWhite = !isWhite) GoWhite(); 
        else GoRed();
        flashPassTime = 0;
    }
    
}

void Player::GoRed()
{
    statusColor = RED;
}

void Player::GoWhite() 
{
    statusColor = WHITE;
}

bool Player::IsFlashing()
{
    return isFlashing;
}

void Player::Flash()
{
    isFlashing = true;
    GoRed();
}

void Player::HandleShooting(vector<Bullet> &bullets)
{
    if (IsKeyPressed(KEY_A)) {
        for (auto& bullet : bullets) {
            if (!bullet.IsValid()) {
                bullet.Spawn({position.x + bodyVerticesOffsets[UP].x, position.y + bodyVerticesOffsets[UP].y});
                return;
            }
        }
    }
}

bool Player::PointInTriangle(Vector2 point)
{
    Vector2 vertices[3];
    for (int i = 0; i < 3; i++) {
        vertices[i] = {position.x + bodyVerticesOffsets[i].x, position.y + bodyVerticesOffsets[i].y};
    }
    return CheckCollisionPointTriangle(point, vertices[0], vertices[1], vertices[2]);
}
