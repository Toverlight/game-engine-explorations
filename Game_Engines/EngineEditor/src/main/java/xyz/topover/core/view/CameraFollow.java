package xyz.topover.core.view;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import xyz.topover.core.base.GameComponent;
import xyz.topover.core.base.GameObject;
import xyz.topover.core.calc.Point2D;

/**
 * <h1>摄像机跟随组件</h1>
 */
public class CameraFollow extends GameComponent {
    private GameObject following; // 跟随对象
    private Camera camera; // 所属相机
    private static final Logger logger = LoggerFactory.getLogger(CameraFollow.class);

    /**
     * <h1>设置跟随对象</h1>
     * <p>
     *     若传参为null，则该组件不起效果。
     * </p>
     * @param following 跟随对象
     */
    public void setFollowing(GameObject following) {
        this.following = following;
    }

    @Override
    public void start() {
        if (gameObject instanceof Camera) {
            camera = (Camera)gameObject;
            logger.debug("CameraFollow(id {}) was added to Camera(id {}).", getId(), camera.getId());
        } else {
            logger.error("CameraFollow(id {}) was wrongly added to a GameObject(id {}) instance instead of" +
                    " a Camera one. It may arise any undefined problems from now on.", getId(), camera.getId());
        }
    }

    @Override
    public void update(double deltaTime) {
        if (isActive()) {
            if (camera != null && following != null) {
                Point2D pos = following.getTransform().getPosition();
                camera.setPosition(pos.x, pos.y);
            }
        }
    }
}
