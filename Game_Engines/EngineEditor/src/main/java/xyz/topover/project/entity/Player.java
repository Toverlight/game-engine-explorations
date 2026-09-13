package xyz.topover.project.entity;

import javafx.scene.input.KeyEvent;
import javafx.scene.input.MouseEvent;
import javafx.scene.input.ScrollEvent;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import xyz.topover.core.base.GameObject;
import xyz.topover.core.calc.Point2D;
import xyz.topover.core.calc.Transform;
import xyz.topover.core.input.InputEventType;
import xyz.topover.core.input.InputListener;

/**
 * 测试用玩家类
 */
public class Player extends GameObject implements InputListener {
    private double speed; // 每秒像素数
    private final Point2D velocity;
    private static final Logger logger = LoggerFactory.getLogger(Player.class);

    public Player() {
        speed = 200;
        velocity = new Point2D(0, 0);
    }

    @Override
    public int getPriority() {
        return 9;
    }

    @Override
    public boolean onInput(InputEventType eventType, KeyEvent keyEvent, MouseEvent mouseEvent) {
        // 按下按键
        if (eventType == InputEventType.KEY_PRESSED) {
            logger.trace("Player(id {}) named '{}':onInput:{}(PRESSED)", getId(), getName(), keyEvent.getCode());
            switch (keyEvent.getCode()) {
                case W:
                    velocity.y = -speed;
                    break;
                case A:
                    velocity.x = -speed;
                    break;
                case S:
                    velocity.y = speed;
                    break;
                case D:
                    velocity.x = speed;
                    break;
            }
        }
        // 松开按键
        else if (eventType == InputEventType.KEY_RELEASED) {
            logger.trace("Player(id {}) named '{}':onInput:{}(RELEASED)", getId(), getName(), keyEvent.getCode());
            switch (keyEvent.getCode()) {
                case W: case S:
                    velocity.y = 0;
                    break;
                case A: case D:
                    velocity.x = 0;
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
        transform.translate(velocity.x * deltaTime, velocity.y * deltaTime);
    }
}
