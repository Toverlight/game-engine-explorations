#include "actor.hpp"
#include "scene.hpp"
#include "../raw/stats.hpp"
#include "../affiliate/affiliate_bar.hpp"

void Actor::update(float dt)
{
    ObjectWorld::update(dt);
    if (health_bar_) {
        updateHealthBar();
    }
}

void Actor::move(float dt)
{
    setPosition(position_ += velocity_ * dt);
    position_ = glm::clamp(position_, glm::vec2(0, 0), game_.getCurrentScene()->getWorldSize());
}

void Actor::takeDamage(float damage)
{
    if (!stats_) return;
    stats_->takeDamage(damage);
}

bool Actor::isAlive() const
{
    if (!stats_) return true;
    return stats_->isAlive();
}

void Actor::updateHealthBar()
{
    if (!stats_ || !health_bar_) return;
    health_bar_->setPercentage(stats_->getHealth() / stats_->getMaxHealth());
}
