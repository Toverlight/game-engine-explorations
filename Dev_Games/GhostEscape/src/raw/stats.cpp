#include "stats.hpp"

Stats *Stats::addStatsChild(Actor *parent, float max_health, float max_mana, float damage, float mana_regen)
{
    Stats* stats = new Stats();
    stats->parent_ = parent;
    stats->max_health_ = max_health;
    stats->health_ = max_health;
    stats->max_mana_ = max_mana;
    stats->mana_ = max_mana;
    stats->damage_ = damage;
    stats->mana_regen_ = mana_regen;
    parent->addChild(stats);
    return stats;
}

void Stats::update(float dt)
{
    regenMana(dt);
    if (is_invincible_) {
        invincible_timer_ += dt;
        if (invincible_timer_ >= invincible_time_) is_invincible_ = false;
    }
    Actor::update(dt);
}

bool Stats::canUseMana(float mana_cost)
{
    return mana_ >= mana_cost;
}

void Stats::useMana(float mana_cost)
{
    mana_ -= mana_cost;
    if (mana_ < 0.0f) mana_ = 0.0f;
}

void Stats::regenMana(float dt)
{
    mana_ += mana_regen_ * dt;
    if (mana_ > max_mana_) mana_ = max_mana_;
}

void Stats::takeDamage(float damage)
{
    if (is_invincible_) return;

    health_ -= damage;
    if (health_ <= 0.0f) {
        health_ = 0.0f;
        is_alive_ = false;
    }

    is_invincible_ = true;
    invincible_timer_ = 0.0f;
}
