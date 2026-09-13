package xyz.topover.core.input;

/**
 * <h1>输入事件类型</h1>
 */
public enum InputEventType {
    /**
     * 按键按下
     */
    KEY_PRESSED,
    /**
     * 按键释放
     */
    KEY_RELEASED,
    /**
     * 按键按下后释放（仅捕获字符键，常用于输入文本）
     */
    KEY_TYPED,
    /**
     * 鼠标按下
     */
    MOUSE_PRESSED,
    /**
     * 鼠标点击
     */
    MOUSE_CLICKED,
    /**
     * 鼠标释放
     */
    MOUSE_RELEASED,
    /**
     * 鼠标移动
     */
    MOUSE_MOVED,
    /**
     * 鼠标拖动
     */
    MOUSE_DRAGGED,
    /**
     * 鼠标滚轮滚动
     */
    MOUSE_SCROLL,

}
