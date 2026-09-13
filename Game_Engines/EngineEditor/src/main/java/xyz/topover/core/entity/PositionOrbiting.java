package xyz.topover.core.entity;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import xyz.topover.core.base.GameComponent;
import xyz.topover.core.calc.Point2D;

/**
 * <h1>实体位置环绕依赖组件</h1>
 * <p>
 *     若实体没有绑定到父实体，则会环绕世界原点。
 * </p>
 */
public class PositionOrbiting extends GameComponent {
    private Point2D lastKeyPos;
    private double rotation = 0;
    private boolean isDirty = true;
    private static final Logger logger = LoggerFactory.getLogger(PositionOrbiting.class);

    public PositionOrbiting() {}
    /**
     * <h1>以初始角初始化</h1>
     * @param rotation 相对父实体的初始角
     */
    public PositionOrbiting(double rotation) {
        this.rotation = rotation;
    }

    @Override
    public void start() {
        if (gameObject != null) {
            lastKeyPos = gameObject.getTransform().getPosition();
            logger.debug("PositionOrbiting(id {}) managed to be added to gameObject(id {}) named '{}'.",
                    getId(), gameObject.getId(), gameObject.getName());

        } else {
            logger.debug("PositionOrbiting(id {}) failed to find its belonging gameObject and " +
                    "this component won't work!", getId());
        }

    }

    @Override
    public void update(double deltaTime) {
        if (isActive()) {
            if (isDirty) {
                updatePosition();
                isDirty = false;
            }
        }
    }

    /**
     * <h1>设置上一个关键位置</h1>
     * <p>
     *     若该组件被添加于setPosition之前，<b>请在setPosition之后调用该方法</b>，以正确将关键位置设置为实体的相对位置。
     * </p>
     */
    public void updateLastKeyPos() {
        if (gameObject != null) {
            lastKeyPos = gameObject.getTransform().getPosition();
            isDirty = true;
        }
    }

    private void updatePosition() {
        if (gameObject != null) {
            if (Math.abs(rotation) > 360) {
                rotation %= 360;
            }
            Point2D rotated = Point2D.rotate(lastKeyPos, rotation);
            gameObject.getTransform().setPosition(rotated.x, rotated.y);
        }

    }

    public void setRotation(double rotation) {
        this.rotation = rotation;
        isDirty = true;
    }

    public void rotate(double rotation) {
        this.rotation += rotation;
        isDirty = true;
    }


}
