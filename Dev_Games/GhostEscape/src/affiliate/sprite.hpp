#ifndef SPRITE_HPP
#define SPRITE_HPP

#include "../core/object_affiliate.hpp"
#include <string>

struct Texture {
    SDL_Texture *texture = nullptr;
    SDL_FRect src_rect = {0, 0, 0, 0};
    float angle = 0;
    bool is_flip = false;
    Texture() = default;
    Texture(const std::string& file_path);
};

class Sprite : public ObjectAffiliate {
protected:
    Texture texture_;
    bool is_finish_ = false;
    glm::vec2 percentage_ = glm::vec2(1.0f); // 决定图片原始区域的百分比

public:
    static Sprite* addSpriteChild(ObjectScreen* parent, const std::string& file_path, float scale = 1.0f, Anchor anchor = Anchor::CENTER);

    virtual void render() override;

    // getters and setters
    Texture getTexture() const { return texture_; }
    virtual void setTexture(const Texture &texture);
    float getAngle() const { return texture_.angle; }
    void setAngle(float angle) { texture_.angle = angle; }
    bool isFlip() const { return texture_.is_flip; }
    void setFlip(bool is_flip) { texture_.is_flip = is_flip; }
    bool isFinish() const { return is_finish_; }
    void setFinish(bool is_finish) { is_finish_ = is_finish; }
    glm::vec2 getPercentage() const { return percentage_; }
    void setPercentage(const glm::vec2 &percentage) { percentage_ = percentage; }
};

#endif // SPRITE_HPP