#include "object.hpp"

bool Object::handleEvents(SDL_Event &event)
{
    for (auto& child : children_) {
        if (child->isActive()) {
            if (child->handleEvents(event)) return true;
        }
    }
    return false;
}

void Object::update(float dt)
{
    for (auto& obj : object_to_add_) {
        addChild(obj);
    }
    object_to_add_.clear();
    for (auto it = children_.begin(); it != children_.end();) {
        auto child = *it;
        if (child->getNeedRemove()) {
            it = children_.erase(it);
            child->clean();
            delete child;
            child = nullptr;
        } else {
            if (child->isActive()) {
                child->update(dt);
            }
            ++it;
        }
    }

}

void Object::render()
{
    for (auto& child : children_) {
        if (child->isActive())
        {
            child->render();
        }
    }
}

void Object::clean()
{
    for (auto& child : children_) {
        child->clean();
    }
    children_.clear();
}

void Object::addChild(Object *child)
{
    children_.push_back(child);
}
