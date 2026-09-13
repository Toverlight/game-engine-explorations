#ifndef HUD_TEXT_HPP
#define HUD_TEXT_HPP

#include "../core/object_screen.hpp"
#include "../affiliate/text_label.hpp"
#include "../affiliate/sprite.hpp"

class HUDText : public ObjectScreen
{
protected:
    TextLabel *text_label_ = nullptr;
    Sprite* sprite_bg_ = nullptr;
    glm::vec2 size_ = glm::vec2(0, 0);

public:
    static HUDText *addHUDTextChild(Object *parent, const std::string &text, glm::vec2 render_pos, glm::vec2 size, const std::string &font_path = "assets/font/VonwaonBitmap-16px.ttf", int font_size = 32, const std::string &bg_path = "assets/UI/Textfield_01.png", Anchor anchor = Anchor::CENTER);

    void setBgSizeByText(float margin = 25.0f);

    // setters and getters
    void setTextLabel(TextLabel *text_label)
    {
        text_label_ = text_label;
    }
    TextLabel *getTextLabel() const { return text_label_; }
    void setSpriteBg(Sprite* sprite_bg) { sprite_bg_ = sprite_bg; }
    Sprite* getSpriteBg() const { return sprite_bg_; }

    void setText(const std::string &text) { text_label_->setText(text); }
    std::string getText() const { return text_label_->getText(); }
    void setSize(const glm::vec2 &size);

    void setBackground(const std::string &path);
};

#endif // HUD_TEXT_HPP