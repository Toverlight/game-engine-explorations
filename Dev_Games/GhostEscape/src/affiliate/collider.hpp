#ifndef COLLIDER_HPP
#define COLLIDER_HPP

#include "../core/object_affiliate.hpp"

class Spell;
class Collider : public ObjectAffiliate {
protected:
    // 友元
    friend Spell; 
    enum class Type {
        CIRCLE, // size_的x是半径，默认y=x
        RECTANGLE
    };

    Type type_ = Type::CIRCLE;

public:
    static Collider* addColliderChild(ObjectScreen* parent, const glm::vec2& size, Type type = Type::CIRCLE, Anchor anchor = Anchor::CENTER);
    bool isColliding(Collider* other) const;

    void render() override;
    // getters and setters
    Type getType() const { return type_; }
    void setType(Type type) { type_ = type; }
};

#endif // COLLIDER_HPP