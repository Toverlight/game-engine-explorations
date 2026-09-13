#ifndef OBJECT_WORLD_HPP
#define OBJECT_WORLD_HPP

#include "object_screen.hpp"
#include "../affiliate/collider.hpp"

class ObjectWorld : public ObjectScreen {
protected:
    glm::vec2 position_ = glm::vec2(0, 0); // 世界位置
    Collider* collider_ = nullptr;

public:
    virtual void init() override {type_ = ObjectType::OBJECT_WORLD;}
    virtual void update(float dt) override;
    // getters and setters
    virtual glm::vec2 getPosition() const override { return position_; }
    void setPosition(const glm::vec2& position);
    virtual void setRenderPosition(const glm::vec2& render_position);
    Collider* getCollider() const { return collider_; }
    void setCollider(Collider* collider) { collider_ = collider; }
    virtual void takeDamage(float damage) { return; };
};

#endif // OBJECT_WORLD_HPP
