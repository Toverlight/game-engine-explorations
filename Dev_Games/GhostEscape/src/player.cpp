#include "player.hpp"
#include "core/scene.hpp"
#include "affiliate/sprite_anim.hpp"
#include "raw/stats.hpp"
#include "affiliate/text_label.hpp"
#include "raw/timer.hpp"

void Player::init()
{
    Actor::init();
    flash_timer_ = Timer::addTimerChild(this, 0.4f);
    flash_timer_->start();
    max_speed_ = 500.0f;
    sprite_idle_ = SpriteAnim::addSpriteAnimChild(this, "assets/sprite/ghost-idle.png", 2.0f);
    sprite_move_ = SpriteAnim::addSpriteAnimChild(this, "assets/sprite/ghost-move.png", 2.0f);
    sprite_move_->setActive(false);

    collider_ = Collider::addColliderChild(this, sprite_idle_->getSize() / 2.0f);
    stats_ = Stats::addStatsChild(this);
    effect_ = Effect::addEffectChild(game_.getCurrentScene(), "assets/effect/1764.png", glm::vec2(0), 1.5f);
    effect_->setActive(false);
    weapon_thunder_ = WeaponThunder::addWeaponThunderChild(this, 2.0f, 40.0f);
}

bool Player::handleEvents(SDL_Event& event)
{
    return Actor::handleEvents(event);
}

void Player::update(float dt)
{
    velocity_ *= 0.9f;
    keyboardControl();
    checkState();
    move(dt);
    syncCamera();
    checkIsDead();
    Actor::update(dt);
}

void Player::render()
{
    if (stats_->isInvincible() && flash_timer_->getProgress() < 0.5f) {
        return;
    }
    Actor::render();
}

void Player::clean()
{
    Actor::clean();
}

void Player::takeDamage(float damage)
{
    if (!stats_ || stats_->isInvincible()) return;
    Actor::takeDamage(damage);
    game_.playSound("assets/sound/hit-flesh-02-266309.mp3");
}

void Player::keyboardControl()
{
    auto keystates = SDL_GetKeyboardState(NULL);

    if (keystates[SDL_SCANCODE_W]) {
        velocity_.y = -max_speed_;
    }
    if (keystates[SDL_SCANCODE_S]) {
        velocity_.y = max_speed_;
    }
    if (keystates[SDL_SCANCODE_A]) {
        velocity_.x = -max_speed_;
    }
    if (keystates[SDL_SCANCODE_D]) {
        velocity_.x = max_speed_;
    }

}

void Player::syncCamera()
{
    game_.getCurrentScene()->setCameraPosition(position_ - game_.getScreenSize() / 2.0f);
}

void Player::checkState()
{
    if (velocity_.x < 0) {
        sprite_move_->setFlip(true);
        sprite_idle_->setFlip(true);        
    } else {
        sprite_move_->setFlip(false);
        sprite_idle_->setFlip(false);
    }

    bool new_is_moving = (glm::length(velocity_) > 0.1f);
    if (new_is_moving != is_moving_) {
        is_moving_ = new_is_moving;
        changeState(is_moving_);
    }

}

void Player::changeState(bool is_moving)
{
    if (is_moving) {
        sprite_idle_->setActive(false);
        sprite_move_->setActive(true);
        sprite_move_->setCurrentFrame(sprite_idle_->getCurrentFrame());
        sprite_move_->setFrameTimer(sprite_idle_->getFrameTimer());
    } else {
        sprite_idle_->setActive(true);
        sprite_move_->setActive(false);
        sprite_idle_->setCurrentFrame(sprite_move_->getCurrentFrame());
        sprite_idle_->setFrameTimer(sprite_move_->getFrameTimer());
    }
}

void Player::checkIsDead()
{
    if (!stats_->isAlive()) {
        effect_->setActive(true);
        effect_->setPosition(position_);
        setActive(false);
        game_.playSound("assets/sound/female-scream-02-89290.mp3");
    }
}
