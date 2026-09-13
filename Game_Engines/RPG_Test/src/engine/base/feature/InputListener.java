package engine.base.feature;

import engine.base.InputEventType;
import javafx.scene.input.KeyEvent;
import javafx.scene.input.MouseEvent;
import javafx.scene.input.ScrollEvent;

/**
 * <h1>输入监听器</h1>
 * <p>
 *     每个接收输入的对象实现一个 InputListener 接口，并向 InputManager 注册。
 * </p>
 */
public interface InputListener {
    /**
     * <h1>获取优先级</h1>
     * @return 优先级（值越大优先级越高）
     */
    int getPriority();

    /**
     * <h1>处理并决定是否捕获事件</h1>
     * @param eventType 事件类型
     * @param keyEvent 键盘输入
     * @param mouseEvent 鼠标输入
     * @return 是否拦截事件（若不拦截，事件将继续传递给其它对象）
     */
    boolean onInput(InputEventType eventType, KeyEvent keyEvent, MouseEvent mouseEvent);

    /**
     * <h1>处理并决定是否捕获滚轮事件</h1>
     * @param eventType 滚轮事件类型
     * @param scrollEvent 滚轮事件
     * @return 是否拦截事件（若不拦截，滚轮事件将继续传递给其它对象）
     */
    boolean onScroll(InputEventType eventType, ScrollEvent scrollEvent);
}
