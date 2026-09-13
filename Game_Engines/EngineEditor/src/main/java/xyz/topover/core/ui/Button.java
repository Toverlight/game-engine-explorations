package xyz.topover.core.ui;

import javafx.scene.input.KeyEvent;
import javafx.scene.input.MouseEvent;
import javafx.scene.input.ScrollEvent;
import xyz.topover.core.base.GameObject;
import xyz.topover.core.calc.Point2D;
import xyz.topover.core.input.InputEventType;
import xyz.topover.core.input.InputListener;
import xyz.topover.core.paint.Polygon;
import xyz.topover.core.view.Camera;
import xyz.topover.core.view.CameraManager;


public abstract class Button extends GameObject implements InputListener {
    private Status status = Status.Free; // 按钮的状态
    private final Polygon region; // 按钮的事件检测范围
    private Runnable callback;

    /**
     * <h1>以范围点阵初始化按钮</h1>
     * <p>
     *     点阵是模型坐标集。
     * </p>
     * @param boundaryVertices 范围点阵
     */
    public Button(Point2D[] boundaryVertices) {
        region = new Polygon(boundaryVertices);
        region.getDrawParams().isOpaque = false;
        region.setTransform(getTransform());
    }

    /**
     * <h1>设置按钮位置</h1>
     * @param pos 世界坐标
     */
    public void setPosition(Point2D pos) {
        getTransform().setPosition(pos.x, pos.y);
    }

    /**
     * <h1>更改判定范围</h1>
     * <p>
     *     点阵是模型坐标集。
     * </p>
     * @param boundaryVertices 范围点阵
     */
    public void changeBoundaryVertices(Point2D[] boundaryVertices) {
        region.setVertices(boundaryVertices);
    }

    /**
     * <h1>设置按钮回调函数</h1>
     * <p>
     *     在按钮被点击后触发。
     * </p>
     * @param callback 回调函数
     */
    public void setCallback(Runnable callback) {
        this.callback = callback;
    }

    @Override
    public int getPriority() {
        return 10;
    }

    @Override
    public boolean onInput(InputEventType eventType, KeyEvent keyEvent, MouseEvent mouseEvent) {
        Point2D m;
        Camera camera = CameraManager.getMainCamera();
        if (eventType == InputEventType.MOUSE_MOVED) {
            m = camera.cameraToWorld(new Point2D(mouseEvent.getX(), mouseEvent.getY()));
            if (status == Status.Free && region.contains(m)) { // Free->Touched
                status = Status.Touched;
            } else if (status == Status.Touched && !region.contains(m)) { // Touched->Free
                status = Status.Free;
            }
        } else if (eventType == InputEventType.MOUSE_PRESSED) {
            if (status == Status.Touched && mouseEvent.isPrimaryButtonDown()) { // Touched->Pressed
                status = Status.Pressed;
            }
        } else if (eventType == InputEventType.MOUSE_RELEASED) {
            m = camera.cameraToWorld(new Point2D(mouseEvent.getX(), mouseEvent.getY()));
            if (status == Status.Pressed && mouseEvent.isPrimaryButtonDown() && region.contains(m)) { // Pressed->Touched
                status = Status.Touched;
            }
        }

        return false;
    }

    @Override
    public boolean onScroll(InputEventType eventType, ScrollEvent scrollEvent) {
        return false;
    }
}

/**
 * <h1>按钮状态</h1>
 */
enum Status {
    /**
     * 平常
     */
    Free,
    /**
     * 触碰
     */
    Touched,
    /**
     * 压下
     */
    Pressed
}
