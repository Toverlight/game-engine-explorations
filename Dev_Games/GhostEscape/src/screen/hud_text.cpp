#include "hud_text.hpp"

HUDText *HUDText::addHUDTextChild(Object *parent, const std::string &text, glm::vec2 render_pos, glm::vec2 size,
                                  const std::string &font_path, int font_size, const std::string &bg_path,
                                  Anchor anchor)
{
    auto hud_text = new HUDText();
    hud_text->init();
    hud_text->setRenderPosition(render_pos);
    hud_text->setSpriteBg(Sprite::addSpriteChild(hud_text, bg_path, 1, anchor));
    hud_text->setSize(size);
    hud_text->setTextLabel(TextLabel::addTextLabelChild(hud_text, text, font_path, font_size, anchor));
    if (parent) parent->addChild(hud_text);
    return hud_text;
}

void HUDText::setBgSizeByText(float margin)
{
    auto text_size = text_label_->getSize();
    setSize(text_size + glm::vec2(margin * 2, margin * 2));
}

void HUDText::setSize(const glm::vec2 &size)
{
    size_ = size;
    sprite_bg_->setSize(size);
}

void HUDText::setBackground(const std::string &path)
{
    if (sprite_bg_) sprite_bg_->setTexture(path);
    else sprite_bg_ = Sprite::addSpriteChild(this, path, 1, Anchor::CENTER);
}
