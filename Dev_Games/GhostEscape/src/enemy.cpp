#include "enemy.hpp"
#include "core/scene.hpp"
#include "raw/stats.hpp"
#include "affiliate/affiliate_bar.hpp"

Enemy *Enemy::addEnemyChild(Object *parent, glm::vec2 pos, Player *target)
{
    auto enemy = new Enemy();
    enemy->init();
    enemy->setPosition(pos);
    enemy->setTarget(target);
    enemy->setMaxSpeed(200.0f);
    if (parent) parent->addChild(enemy);
    return enemy;
}

void Enemy::init()
{
    Actor::init();
    anim_normal_ = SpriteAnim::addSpriteAnimChild(this, "assets/sprite/ghost-Sheet.png", 2.0f);
    anim_hurt_ = SpriteAnim::addSpriteAnimChild(this, "assets/sprite/ghostHurt-Sheet.png", 2.0f);
    anim_die_ = SpriteAnim::addSpriteAnimChild(this, "assets/sprite/ghostDead-Sheet.png", 2.0f);
    anim_normal_->setActive(true);
    anim_hurt_->setActive(false);
    anim_die_->setActive(false);
    anim_die_->setLoop(false);

    current_anim_ = anim_normal_;
    collider_ = Collider::addColliderChild(this, current_anim_->getSize());
    stats_ = Stats::addStatsChild(this);
    auto size = anim_normal_->getSize();
    health_bar_ = AffiliateBar::addAffiliateBarChild(this, glm::vec2(size.x - 10, 10), Anchor::BOTTOM_CENTER);
    health_bar_->setOffset(health_bar_->getOffset() + glm::vec2(0, size.y / 2.0f - 5.0f));
    setType(ObjectType::ENEMY);
}

void Enemy::update(float dt)
{
    if (target_ && target_->isActive()) {    
        aim_target(target_);
        move(dt);
        attack();
    }
    Actor::update(dt);
    checkState();
    remove();
}

void Enemy::aim_target(Player *target)
{
    if (target == nullptr) return;
    auto direction = target->getPosition() - this->getPosition();
    direction = glm::normalize(direction);
    velocity_ = direction * max_speed_;
}

void Enemy::checkState()
{
    State new_state;
    if (stats_->getHealth() <= 0) {
        new_state = State::DIE;
    } else if (stats_->isInvincible()) {
        new_state = State::HURT;
    } else {
        new_state = State::NORMAL;
    }

    if (new_state != current_state_) {
        changeState(new_state);
    }

}

void Enemy::changeState(State new_state)
{
    current_anim_->setActive(false);
    switch (new_state)
    {
    case State::NORMAL:
        current_anim_ = anim_normal_;
        break;
    case State::HURT:
        current_anim_ = anim_hurt_;
        break;
    case State::DIE:
        current_anim_ = anim_die_;
        game_.addScore(score_);
        break;
    default:
        break;
    }
    current_state_ = new_state;
    current_anim_->setActive(true);
}

void Enemy::attack()
{
    if (!collider_ || !target_ || target_->getCollider() == nullptr) return;
    if (collider_->isColliding(target_->getCollider())) {
        if (stats_ && target_->getStats()) {
            target_->takeDamage(stats_->getDamage());
        }
    }
}

void Enemy::remove()
{
    if (anim_die_->isFinish()) {
        need_remove_ = true;
    }
}
