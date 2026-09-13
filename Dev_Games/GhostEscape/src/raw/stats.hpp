#ifndef STATS_HPP
#define STATS_HPP

#include "../core/actor.hpp"

class Stats : public Actor {
protected:
    Actor* parent_ = nullptr;
    float health_ = 100.0f;
    float max_health_ = 100.0f;
    float mana_ = 100.0f;
    float max_mana_ = 100.0f;
    float damage_ = 40.0f;
    float mana_regen_ = 10.0f;

    float invincible_timer_ = 0.0f; // 无敌时间计时器
    float invincible_time_ = 1.5f; // 受伤后，无敌时间有多长
    bool is_invincible_ = false;
    bool is_alive_ = true;

public:
    static Stats* addStatsChild(Actor* parent, float max_health = 100.0f, float max_mana = 100.0f, float damage = 40.0f, float mana_regen = 10.0f);
    virtual void update(float dt) override;

    bool canUseMana(float mana_cost);
    void useMana(float mana_cost);
    void regenMana(float dt);
    void takeDamage(float damage);

    // getters and setters
    float getHealth() const { return health_; }
    void setHealth(float health) { health_ = health; }
    float getMaxHealth() const { return max_health_; }
    void setMaxHealth(float max_health) { max_health_ = max_health; }
    float getMana() const { return mana_; }
    void setMana(float mana) { mana_ = mana; }
    float getMaxMana() const { return max_mana_; }
    void setMaxMana(float max_mana) { max_mana_ = max_mana; }
    float getDamage() const { return damage_; }
    void setDamage(float damage) { damage_ = damage; }
    float getManaRegen() const { return mana_regen_; }
    void setManaRegen(float mana_regen) { mana_regen_ = mana_regen; }
    bool isAlive() const { return is_alive_; }
    void setAlive(bool is_alive) { is_alive_ = is_alive; }
    void setParent(Actor* parent) { parent_ = parent; }
    Actor* getParent() const { return parent_; }
    void setInvincible(bool is_invincible) { is_invincible_ = is_invincible; }
    bool isInvincible() const { return is_invincible_; }
};


#endif // STATS_HPP