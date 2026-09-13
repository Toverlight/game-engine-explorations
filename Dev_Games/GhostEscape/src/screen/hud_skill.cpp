#include "hud_skill.hpp"
#include "../affiliate/sprite.hpp"

HUDSkill *HUDSkill::addHudSkillChild(Object *parent, const std::string &file_path, glm::vec2 pos, float scale, Anchor anchor)
{
    auto hud_skill = new HUDSkill();
    hud_skill->init();
    hud_skill->setIcon(Sprite::addSpriteChild(hud_skill, file_path, scale, anchor));
    hud_skill->setRenderPosition(pos);
    if (parent) parent->addChild(hud_skill);
    return hud_skill;
}

void HUDSkill::render()
{
    SDL_SetTextureColorModFloat(icon_->getTexture().texture, 0.3f, 0.3f, 0.3f);
    auto pos = getRenderPosition() + icon_->getOffset();
    game_.renderTexture(icon_->getTexture(), pos, icon_->getSize());
    SDL_SetTextureColorModFloat(icon_->getTexture().texture, 1.0f, 1.0f, 1.0f);
    ObjectScreen::render();
}

void HUDSkill::setPercentage(float percentage)
{
    percentage_ = glm::clamp(percentage, 0.0f, 1.0f);
    if (icon_) {
        icon_->setPercentage(glm::vec2(1.0f, percentage_));
    }
}
