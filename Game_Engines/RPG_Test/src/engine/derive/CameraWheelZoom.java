package engine.derive;

import engine.base.*;
import engine.base.feature.InputListener;
import javafx.scene.input.KeyEvent;
import javafx.scene.input.MouseEvent;
import javafx.scene.input.ScrollEvent;

import java.util.function.Consumer;

/**
 * <h1>（测试用）相机滚轮缩放组件</h1>
 */
public class CameraWheelZoom extends GameComponent implements InputListener {
    private Camera camera;
    private static final double ZOOM_MIN = 0.1;
    private static final double ZOOM_MAX = 10.0;
    private static final double ZOOM_DELTA = 0.3;

    public CameraWheelZoom() {}

    @Override
    public void start() {
        if (gameObject != null) {
            if (gameObject instanceof Camera) {
                camera = (Camera)gameObject;
            } else {
                LogUtils.severe("CameraWheelZoom(id %s) is a Camera component that cannot be applied to other" +
                        "types!It was wrongly added to GameObject(id %s) named '%s'!", getId(), gameObject.getId(),
                        gameObject.getName());
            }
        } else {
            LogUtils.severe("CameraWheelZoom(id %s) has no effective gameObject ref(null)!", getId());
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
