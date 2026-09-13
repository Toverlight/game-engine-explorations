package project.test.ones;

import engine.base.GameObject;
import engine.base.InputEventType;
import engine.base.Transform;
import engine.base.feature.InputListener;
import javafx.scene.input.KeyEvent;
import javafx.scene.input.MouseEvent;
import javafx.scene.input.ScrollEvent;

/**
 * 测试用玩家类
 */
public class Player extends GameObject implements InputListener {
    private double speed = 200; // 每秒像素数
    private double vX = 0;
    private double vY = 0;
    @Override
    public int getPriority() {
        return 10;
    }

    @Override
    public boolean onInput(InputEventType eventType, KeyEvent keyEvent, MouseEvent mouseEvent) {
        // 按下按键
        if (eventType == InputEventType.KEY_PRESSED) {
            switch (keyEvent.getCode()) {
                case W:
                    vY = -speed;
                    break;
                case A:
                    vX = -speed;
                    break;
                case S:
                    vY = speed;
                    break;
                case D:
                    vX = speed;
                    break;
            }
        }
        // 松开按键
        else if (eventType == InputEventType.KEY_RELEASED) {
            switch (keyEvent.getCode()) {
                case W: case S:
                    vY = 0;
                    break;
                case A: case D:
                    vX = 0;
                    break;
            }
        }
        return true;
    }

    @Override
    public boolean onScroll(InputEventType eventType, ScrollEvent scrollEvent) {
        return false;
    }

    @Override
    public void update(double deltaTime) {
        super.update(deltaTime);
        Transform transform = getTransform();
        transform.translate(vX * deltaTime, vY * deltaTime);
    }
}
