#include "hud_button.hpp"

HUDButton *HUDButton::addHUDButtonChild(Object * parent, glm::vec2 render_pos, const std::string & file_path_normal, const std::string & file_path_hover, const std::string & file_path_press, float scale, Anchor anchor)
{
    auto hud_button = new HUDButton();
    hud_button->init();
    hud_button->setRenderPosition(render_pos);
    hud_button->sprite_normal_ = Sprite::addSpriteChild(hud_button, file_path_normal, scale, anchor);
    hud_button->sprite_hover_ = Sprite::addSpriteChild(hud_button, file_path_hover, scale, anchor);
    hud_button->sprite_press_ = Sprite::addSpriteChild(hud_button, file_path_press, scale, anchor);
    hud_button->sprite_normal_->setActive(true);
    hud_button->sprite_hover_->setActive(false);
    hud_button->sprite_press_->setActive(false);
    if (parent) {
        parent->addChild(hud_button);
    } 
    return hud_button;
}

bool HUDButton::handleEvents(SDL_Event &event)
{
    if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        if (event.button.button == SDL_BUTTON_LEFT) {
            if (is_hover_) {
                is_press_ = true;
                game_.playSound("assets/sound/UI_button08.wav");
                return true;
            }
        }
    } else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
        if (event.button.button == SDL_BUTTON_LEFT) {
            is_press_ = false;
            if (is_hover_) {
                is_trigger_ = true;
                return true;
            }
        }
    }
    return false;
}

void HUDButton::update(float)
{
    checkHover();
    checkState();
    
}

void HUDButton::checkHover()
{
    bool new_hover;
    auto pos = render_position_ + sprite_normal_->getOffset();
    auto size = sprite_normal_->getSize();
    if (game_.isMouseInRect(pos, pos + size)) {
        new_hover = true;
    } else {
        new_hover = false;
    }
    if (new_hover != is_hover_) {
        is_hover_ = new_hover;
        if (is_hover_ && !is_press_) {
            game_.playSound("assets/sound/UI_button12.wav");
        }
    }
}

void HUDButton::checkState()
{
    if (!is_press_ && !is_hover_) {
        sprite_normal_->setActive(true);
        sprite_hover_->setActive(false);
        sprite_press_->setActive(false);
    } else if (!is_press_ && is_hover_) {
        sprite_normal_->setActive(false);
        sprite_hover_->setActive(true);
        sprite_press_->setActive(false);
    } else {
        sprite_normal_->setActive(false);
        sprite_hover_->setActive(false);
        sprite_press_->setActive(true);
    }
}

bool HUDButton::isTrigger()
{
    if (is_trigger_) {
        is_trigger_ = false;
        is_press_ = false;
        is_hover_ = false;
        return true;
    }
    return false;
}

void HUDButton::setScale(float scale)
{
    sprite_normal_->setScale(scale);
    sprite_hover_->setScale(scale);
    sprite_press_->setScale(scale);
}
