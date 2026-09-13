#ifndef WEAPON_HPP
#define WEAPON_HPP

#include "../core/object.hpp"

class Spell;
class Actor;
class Weapon : public Object {
protected:
    Actor* parent_ = nullptr; // 武器的父对象
    float cool_down_ = 1.0f; // 冷却时间
    float mana_cost_ = 0.0f; // 法力消耗
    float cool_down_timer_ = 0.0f; // 冷却时间计时器

public:
    virtual void update(float dt) override;

    void attack(glm::vec2 position, Spell* spell); // 确保调用这个函数的时候，一定执行了attack()
    bool canAttack();

    // getters and setters
    float getCoolDown() const { return cool_down_; }
    void setCoolDown(float cool_down) { cool_down_ = cool_down; }
    float getManaCost() const { return mana_cost_; }
    void setManaCost(float mana_cost) { mana_cost_ = mana_cost; }
    Actor* getParent() const { return parent_; }
    void setParent(Actor* parent) { parent_ = parent; }

};

#endif // WEAPON_HPP