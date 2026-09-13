#ifndef OBJECT_AFFILIATE_HPP
#define OBJECT_AFFILIATE_HPP

#include "object_screen.hpp"

class ObjectAffiliate : public Object {
protected:
    ObjectScreen* parent_ = nullptr; // 父节点
    glm::vec2 offset_ = glm::vec2(0, 0); // 偏移量
    glm::vec2 size_ = glm::vec2(0, 0); // 尺寸
    Anchor anchor_ = Anchor::CENTER; // 锚点

public:
    void setOffsetByAnchor(Anchor anchor);
    // getters and setters
    ObjectScreen* getParent() const { return parent_; }
    void setParent(ObjectScreen* object_parent) { parent_ = object_parent; }
    glm::vec2 getOffset() const { return offset_; }
    void setOffset(const glm::vec2& offset) { offset_ = offset; }
    glm::vec2 getSize() const { return size_; }
    void setSize(const glm::vec2& size);
    void setScale(float scale);
    Anchor getAnchor() const { return anchor_; }
    void setAnchor(Anchor anchor) { anchor_ = anchor; }
};

#endif // OBJECT_AFFILIATE_HPP