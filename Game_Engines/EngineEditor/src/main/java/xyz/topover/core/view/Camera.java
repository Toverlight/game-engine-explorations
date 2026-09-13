package xyz.topover.core.view;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import xyz.topover.core.base.GameComponent;
import xyz.topover.core.base.GameObject;
import xyz.topover.core.calc.Point2D;
import xyz.topover.core.calc.Transform;
import xyz.topover.core.input.InputListener;
import xyz.topover.core.input.InputManager;
import xyz.topover.core.run.GameRunner;

import java.util.logging.Level;

/**
 * <h1>摄像机</h1>
 * <p>继承的transform内仿射变换为<b>世界坐标到相机坐标的变换</b>。<br>
 *    相机的所有操作，应用于该仿射变换矩阵，就是(自动处理为)相反的操作。<br>
 *    <b>注意</b>，相机的跟随实体的效果<b>不能通过addChild方法实现</b>，只能通过添加组件的形式如CameraFollow实现。
 * </p>
 */
public class Camera extends GameObject {
    private final Point2D viewport = new Point2D(GameRunner.WINDOW_WIDTH, GameRunner.WINDOW_HEIGHT);
    private final Point2D focusPoint = new Point2D();

    private static final Logger logger = LoggerFactory.getLogger(Camera.class);
    /**
     * <h1>新建摄像机</h1>
     * <p>
     *     默认名称"<b>Main Camera</b>"<br>
     *     默认跟随行为：仅平移(T)
     * </p>
     */
    public Camera() {
        setName("Main Camera");
        setPosition(0, 0); // 摄像机中心（画面中心）默认在原点
        logger.debug("Camera(id {}) created.", getId());
    }
    /**
     * <h1>以指定名称新建摄像机</h1>
     * <p>
     *     默认跟随行为：仅平移(T)
     * </p>
     */
    public Camera(String name) {
        setName(name);
        setPosition(0, 0); // 摄像机中心（画面中心）默认在原点
        logger.debug("Camera(id {}) created.", getId());
    }

    /**
     * <h1>设置相机跟随行为</h1>
     * @param i 跟随行为
     */
    public void setI(Transform.I i) {
        getTransform().setI(i);
    }

    /**
     * <h1>世界点转换为相机点</h1>
     * @param worldPoint 世界点
     * @return 相机点
     */
    public Point2D worldToCamera(Point2D worldPoint) {
        return getTransform().getLocalToWorldMatrix().transform(worldPoint);
    }

    /**
     * <h1>将相机点转换为世界点</h1>
     * @param cameraPoint 相机点
     * @return 世界点
     */
    public Point2D cameraToWorld(Point2D cameraPoint) {
        return getTransform().getWorldToLocalMatrix().transform(cameraPoint);
    }

    /**
     * <h1>判断相机点是否在视口内</h1>
     * <p>
     *     通常用在渲染之前，仅绘制在视口内的相机点达到<b>减少计算负载</b>的作用。<br>
     * </p>
     * @param cameraPoint 相机点
     * @return 是否在视口内
     */
    public boolean isCameraPointInViewport(Point2D cameraPoint) {
        return (cameraPoint.x >= 0 &&
                cameraPoint.x <= viewport.x &&
                cameraPoint.y >= 0 &&
                cameraPoint.y <= viewport.y);
    }

    /**
     * <h1>调整视口大小</h1>
     * <p>
     *     对于主摄像机，<b>窗口尺寸变化时</b>，建议动态调整视口大小。
     * </p>
     * @param viewport 视口尺寸（长，高）
     */
    public void setViewport(Point2D viewport) {
        this.viewport.x = viewport.x;
        this.viewport.y = viewport.y;
    }
    public Point2D getViewport() {
        return new Point2D(viewport);
    }

    /**
     * <h1>设置摄像机坐标</h1>
     * <p>
     *     摄像机坐标即摄像机中心点(focusPoint)在世界中的坐标。
     * </p>
     * @param x 中心点x
     * @param y 中心点y
     */
    public void setPosition(double x, double y) {
        focusPoint.x = x;
        focusPoint.y = y;
        getTransform().setPosition(viewport.x / 2 - focusPoint.x, viewport.y / 2 - focusPoint.y);
        getTransform().setBiasPoint(focusPoint);
    }
    public Point2D getPosition() {
        return getTransform().getPosition();
    }
    public void translate(double dx, double dy) {
        getTransform().translate(-dx, -dy);
    }

    public void setRotation(double rotation) {
        getTransform().setRotation(-rotation);
    }
    public double getRotation() {
        return getTransform().getRotation();
    }
    public void rotate(double deltaRadian) {
        getTransform().rotate(-deltaRadian);
    }

    public void setZoom(double s) {
        getTransform().setScale(s);
    }
    public void setZoom(double sx, double sy) {
        getTransform().setScale(sx, sy);
    }
    public Point2D getZoom() {
        return getTransform().getScale();
    }
    public void zoom(double ds) {
        getTransform().scale(ds, ds);
    }
    public void zoom(double dsx, double dsy) {
        getTransform().scale(dsx, dsy);
    }

    public void setShearX(double hx) {
        getTransform().setShearX(-hx);
    }
    public void setShearY(double hy) {
        getTransform().setShearY(-hy);
    }

    /**
     * <h1>重新利用输入组件</h1>
     * <p>
     *     将输入相关组件重新注册进InputManager。
     * </p>
     */
    void reuseInputComponents() {
        for (GameComponent c : components) {
            if (c instanceof InputListener) {
                InputManager.registerListener((InputListener) c);
            }
        }
    }

    /**
     * <h1>删除输入组件</h1>
     * <p>
     *     将输入相关组件从InputManager中删除。
     * </p>
     */
    void dropInputComponents() {
        for (GameComponent c : components) {
            if (c instanceof InputListener) {
                InputManager.unregisterListener((InputListener) c);
            }
        }
    }

}
