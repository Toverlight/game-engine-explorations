#ifndef ACTOR_HPP
#define ACTOR_HPP

#include "object_world.hpp"

class Stats;
class AffiliateBar;
class Actor : public ObjectWorld {
protected:
    Stats* stats_ = nullptr; // 角色状态
    AffiliateBar* health_bar_ = nullptr; // 生命值条
    glm::vec2 velocity_ = glm::vec2(0, 0); // 速度
    float max_speed_ = 500.0f; // 最大速度大小

public:
    virtual void update(float dt) override; // 更新

    void move(float dt); // 移动
    virtual void takeDamage(float damage) override;
    bool isAlive() const;
    // getters and setters
    glm::vec2 getVelocity() const { return velocity_; }
    void setVelocity(const glm::vec2& velocity) { velocity_ = velocity; }
    float getMaxSpeed() const { return max_speed_; }
    void setMaxSpeed(float max_speed) { max_speed_ = max_speed; }
    AffiliateBar* getHealthBar() const { return health_bar_; }
    void setHealthBar(AffiliateBar* health_bar) { health_bar_ = health_bar; }
    Stats* getStats() const { return stats_; }
    void setStats(Stats* stats) { stats_ = stats; }

private:
    void updateHealthBar(); // 更新生命值条
};

#endif // ACTOR_HPP