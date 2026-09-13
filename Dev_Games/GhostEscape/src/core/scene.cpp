#include "scene.hpp"

bool Scene::handleEvents(SDL_Event &event)
{
    for (auto& child : children_screens_) {
        if (child->isActive())
        {
            if (child->handleEvents(event)) return true;
        }
    }
    if (is_paused_) return false;
    Object::handleEvents(event);
    for (auto& child : children_worlds_) {
        if (child->isActive())
        {
            if (child->handleEvents(event)) return true;
        }
    }
    return false;
}

void Scene::update(float dt)
{
    if (!is_paused_) {    
        Object::update(dt);
        for (auto it = children_worlds_.begin(); it != children_worlds_.end();) {
            ObjectWorld* child = *it;
            if (child->getNeedRemove()) {
                it = children_worlds_.erase(it);
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
    for (auto it = children_screens_.begin(); it != children_screens_.end();) {
        ObjectScreen* child = *it;
        if (child->getNeedRemove()) {
            it = children_screens_.erase(it);
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

void Scene::render()
{
    Object::render();
    for (auto& child : children_worlds_) {
        if (child->isActive())
        {
            child->render();
        }
    }
    for (auto& child : children_screens_) {
        if (child->isActive())
        {
            child->render();
        }
    }
}

void Scene::clean()
{
    Object::clean();
    for (auto& child : children_worlds_) {
        child->clean();
        delete child;
        child = nullptr;
    }
    children_worlds_.clear();
    for (auto& child : children_screens_) {
        child->clean();
        delete child;
        child = nullptr;
    }
    children_screens_.clear();
}

void Scene::addChild(Object *child)
{
    switch (child->getType())
    {
    case ObjectType::OBJECT_WORLD:
    case ObjectType::ENEMY:
        children_worlds_.push_back(dynamic_cast<ObjectWorld*>(child));
        break;
    case ObjectType::OBJECT_SCREEN:
        children_screens_.push_back(dynamic_cast<ObjectScreen*>(child));
        break;
    default:
        children_.push_back(child);
        break;
    }
}

void Scene::removeChild(Object *child)
{
    switch (child->getType())
    {
    case ObjectType::OBJECT_WORLD:
    case ObjectType::ENEMY:
        children_worlds_.erase(std::remove(children_worlds_.begin(), children_worlds_.end(), dynamic_cast<ObjectWorld*>(child)), children_worlds_.end());
        break;
    case ObjectType::OBJECT_SCREEN:
        children_screens_.erase(std::remove(children_screens_.begin(), children_screens_.end(), dynamic_cast<ObjectScreen*>(child)), children_screens_.end());
        break;
    default:
        children_.erase(std::remove(children_.begin(), children_.end(), child), children_.end());
        break;
    }
}

void Scene::pause()
{
    is_paused_ = true;
    game_.pauseSound();
    game_.pauseMusic();
}

void Scene::resume()
{
    is_paused_ = false;
    game_.resumeSound();
    game_.resumeMusic();
}

void Scene::setCameraPosition(const glm::vec2 &camera_position)
{
    camera_position_ = camera_position;
    camera_position_ = glm::clamp(camera_position_, glm::vec2(-30), world_size_ - game_.getScreenSize() + glm::vec2(30));
}