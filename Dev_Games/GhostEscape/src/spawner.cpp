#include "spawner.hpp"
#include "enemy.hpp"
#include "world/effect.hpp"
#include "core/scene.hpp"

void Spawner::update(float dt)
{
    if (!target_ || !target_->isActive()) return;
    timer_ += dt;
    if (timer_ >= interval_) {
        timer_ = 0;
        game_.playSound("assets/sound/silly-ghost-sound-242342.mp3");
        for (int i = 0; i < num_; ++i) {
            auto pos = game_.getRandomVec2(game_.getCurrentScene()->getCameraPosition(),
                                           game_.getCurrentScene()->getCameraPosition()  + game_.getScreenSize());
            Enemy* enemy = Enemy::addEnemyChild(nullptr, pos, target_);
            Effect::addEffectChild(game_.getCurrentScene(), "assets/effect/184_3.png", pos, 1.0f, enemy);
        }
    }
}
