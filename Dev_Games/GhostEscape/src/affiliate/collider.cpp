#include "collider.hpp"

Collider *Collider::addColliderChild(ObjectScreen *parent, const glm::vec2 &size, Type type, Anchor anchor)
{
    auto collider = new Collider();
    collider->init();
    collider->setAnchor(anchor);
    collider->setParent(parent);
    collider->setSize(size);
    collider->setType(type);
    parent->addChild(collider);
    return collider;
}

bool Collider::isColliding(Collider *other) const
{
    if (!other) return false;
    if (type_ == Type::CIRCLE && other->getType() == Type::CIRCLE) { // 两个圆的情况
        auto point1 = parent_->getPosition() + offset_ + size_ / 2.0f;
        auto point2 = other->getParent()->getPosition() + other->getOffset() + other->getSize() / 2.0f;
        return glm::distance(point1, point2) < (size_.x + other->getSize().x) / 2.0f;
    }
    // TODO: 其它形状的碰撞检测
    return false;
}

void Collider::render()
{
#ifdef DEBUG_MODE
    ObjectAffiliate::render();
    auto pos = parent_->getRenderPosition() + offset_;
    game_.renderFillCircle(pos, size_, 0.3f);
#endif // DEBUG_MODE
}
