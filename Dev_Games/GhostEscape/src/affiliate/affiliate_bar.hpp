#ifndef AFFILIATE_BAR_HPP
#define AFFILIATE_BAR_HPP

#include "../core/object_affiliate.hpp"

class AffiliateBar : public ObjectAffiliate {
protected:
    float percentage_ = 1.0f;
    SDL_FColor color_high_ = {0, 1, 0, 1}; // Green
    SDL_FColor color_mid_ = {1, 0.65f, 0, 1}; // Orange
    SDL_FColor color_low_ = {1, 0, 0, 1}; // Red

public:
    static AffiliateBar* addAffiliateBarChild(ObjectScreen* parent, glm::vec2 size, Anchor anchor = Anchor::CENTER);
        
    virtual void render() override;

    // getters and setters
    float getPercentage() const { return percentage_; }
    void setPercentage(float percentage) { percentage_ = percentage; }
    SDL_FColor getColorHigh() const { return color_high_; }
    void setColorHigh(SDL_FColor color) { color_high_ = color; }
    SDL_FColor getColorMid() const { return color_mid_; }
    void setColorMid(SDL_FColor color) { color_mid_ = color; }
    SDL_FColor getColorLow() const { return color_low_; }
    void setColorLow(SDL_FColor color) { color_low_ = color; }

};


#endif // AFFILIATE_BAR_HPP