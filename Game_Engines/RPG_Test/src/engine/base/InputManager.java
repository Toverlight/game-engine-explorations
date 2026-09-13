package engine.base;

import engine.base.feature.InputListener;
import javafx.scene.Scene;
import javafx.scene.input.KeyEvent;
import javafx.scene.input.MouseEvent;
import javafx.scene.input.ScrollEvent;

import java.util.ArrayList;
import java.util.Comparator;
import java.util.List;

/**
 * <h1>输入管理器</h1>
 * <p>
 *     全局监听所有输入事件，并将事件分发给注册的对象。
 * </p>
 */
public class InputManager {
    private static final List<InputListener> listeners = new ArrayList<>();
    private static boolean needsSorting = false; // 是否需要排序的标记

    /**
     * <h1>注册监听器</h1>
     * @param listener 输入监听器
     */
    public static void registerListener(InputListener listener) {
        listeners.add(listener);
        needsSorting = true;
    }
    /**
     * <h1>注销监听器</h1>
     * @param listener 输入监听器
     */
    public static void unregisterListener(InputListener listener) {
        listeners.remove(listener);
    }

    /**
     * <h1>分发事件</h1>
     * @param eventType 事件类型
     * @param keyEvent 键盘事件
     * @param mouseEvent 鼠标事件
     */
    public static void dispatchEvent(InputEventType eventType, KeyEvent keyEvent, MouseEvent mouseEvent) {
        if (needsSorting) {
            listeners.sort(Comparator.comparingInt(InputListener::getPriority).reversed());
            needsSorting = false;
        }
        for (InputListener listener : listeners) {
            if (listener.onInput(eventType, keyEvent, mouseEvent)) break;
        }
    }

    /**
     * <h1>分发滚轮事件</h1>
     * @param eventType 滚轮事件类型
     * @param scrollEvent 滚轮事件
     */
    public static void dispatchEvent(InputEventType eventType, ScrollEvent scrollEvent) {
        if (needsSorting) {
            listeners.sort(Comparator.comparingInt(InputListener::getPriority).reversed());
            needsSorting = false;
        }
        for (InputListener listener : listeners) {
            if (listener.onScroll(eventType, scrollEvent)) break;
        }
    }

    /**
     * <h1>绑定各类输入监听器</h1>
     * @param scene javafx场景
     */
    public static void bindListeners(Scene scene) {
        scene.setOnKeyPressed(event -> InputManager.dispatchEvent(InputEventType.KEY_PRESSED, event, null));
        scene.setOnKeyReleased(event -> InputManager.dispatchEvent(InputEventType.KEY_RELEASED, event, null));
        scene.setOnKeyTyped(event -> InputManager.dispatchEvent(InputEventType.KEY_TYPED, event, null));

        scene.setOnMousePressed(event -> InputManager.dispatchEvent(InputEventType.MOUSE_PRESSED, null, event));
        scene.setOnMouseClicked(event -> InputManager.dispatchEvent(InputEventType.MOUSE_CLICKED, null, event));
        scene.setOnMouseReleased(event -> InputManager.dispatchEvent(InputEventType.MOUSE_RELEASED, null, event));
        scene.setOnMouseMoved(event -> InputManager.dispatchEvent(InputEventType.MOUSE_MOVED, null, event));
        scene.setOnMouseDragged(event -> InputManager.dispatchEvent(InputEventType.MOUSE_DRAGGED, null, event));
        scene.setOnScroll(event -> InputManager.dispatchEvent(InputEventType.MOUSE_SCROLL, event));

    }
}
