package xyz.topover.core.view;

import javafx.scene.input.KeyEvent;
import javafx.scene.input.MouseEvent;
import javafx.scene.input.ScrollEvent;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import xyz.topover.core.base.GameComponent;
import xyz.topover.core.calc.Point2D;
import xyz.topover.core.input.InputEventType;
import xyz.topover.core.input.InputListener;
import xyz.topover.core.input.InputManager;

/**
 * <h1>（测试用）相机滚轮缩放组件</h1>
 */
public class CameraWheelZoom extends GameComponent implements InputListener {
    private Camera camera;
    private static final double ZOOM_MIN = 0.1;
    private static final double ZOOM_MAX = 10.0;
    private static final double ZOOM_DELTA = 0.3;
    private static final Logger logger = LoggerFactory.getLogger(CameraWheelZoom.class);

    public CameraWheelZoom() {}

    @Override
    public void start() {
        if (gameObject != null) {
            if (gameObject instanceof Camera) {
                camera = (Camera)gameObject;
            } else {
                logger.error("CameraWheelZoom(id {}) is a Camera component that cannot be applied to other" +
                        "types!It was wrongly added to GameObject(id {}) named '{}'!", getId(), gameObject.getId(),
                        gameObject.getName());
            }
        } else {
            logger.error("CameraWheelZoom(id {}) has no effective gameObject ref(null)!", getId());
        }
    }

    @Override
    public int getPriority() {
        return 10;
    }

    @Override
    public boolean onInput(InputEventType eventType, KeyEvent keyEvent, MouseEvent mouseEvent) {
        return false;
    }

    @Override
    public boolean onScroll(InputEventType eventType, ScrollEvent scrollEvent) {
        // --- 相机输入相关组件的基本结构：if(~){<处理逻辑>}和else{~} --- //
        if (CameraManager.getMainCamera() == camera) {
            Point2D oriZoom = camera.getZoom();
            if (scrollEvent.getDeltaY() > 0) { // 上滚放大
                if (oriZoom.x + ZOOM_DELTA >= ZOOM_MAX || oriZoom.y + ZOOM_DELTA >= ZOOM_MAX) {
                    camera.setZoom(ZOOM_MAX);
                } else {
                    camera.zoom(ZOOM_DELTA);
                }
            } else if (scrollEvent.getDeltaY() < 0) { // 下滚缩小
                if (oriZoom.x - ZOOM_DELTA <= ZOOM_MIN || oriZoom.y - ZOOM_DELTA <= ZOOM_MIN) {
                    camera.setZoom(ZOOM_MIN);
                } else {
                    camera.zoom(-ZOOM_DELTA);
                }
            }
        } else {
            InputManager.unregisterListener(this);
        }

        return false;
    }
}
