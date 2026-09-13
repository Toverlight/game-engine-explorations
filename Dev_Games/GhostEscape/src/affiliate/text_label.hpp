#ifndef TEXT_LABEL_HPP
#define TEXT_LABEL_HPP

#include "../core/object_affiliate.hpp"
#include <string>

class TextLabel : public ObjectAffiliate {
protected:
    TTF_Text* ttf_text_ = nullptr; // 字体对象
    std::string font_path_; // 字体路径
    int font_size_ = 16; // 字体大小

public:
    static TextLabel* addTextLabelChild(ObjectScreen* parent, const std::string& text, const std::string& font_path, int font_size = 16, Anchor anchor = Anchor::CENTER); // 添加文本标签子对象

    virtual void render() override; // 渲染函数
    virtual void clean() override;
    // setters and getters
    void setFont(const std::string& font_path, int font_size); // 设置字体
    void setFontPath(const std::string& font_path);// 设置字体路径
    void setFontSize(int font_size);
    void setText(const std::string& ttf_text); // 设置文本内容;
    std::string getFontPath() const { return font_path_; } // 获取字体路径
    int getFontSize() const { return font_size_; } // 获取字体大小
    std::string getText() const { return ttf_text_ ? ttf_text_->text : ""; } // 获取文本内容

private:
    void updateSize(); // 根据文本内容更新大小

};

#endif // TEXT_LABEL_HPP