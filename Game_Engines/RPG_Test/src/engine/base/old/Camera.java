package engine.base.old;

import engine.base.LogUtils;
import engine.base.Point2D;

import java.util.LinkedList;
import java.util.logging.Level;

public class Camera {
    private Point2D cameraPos; // 摄像机位置（即视口左上角的世界坐标）
    private double zoomFactor; // 缩放比例
    private int viewportWidth; // 视口宽度 (一般就是窗口宽）
    private int viewportHeight; // 视口高度（一般就是窗口高）
    private LinkedList<Base> BaseFlashingList;// 正在操作的绘制刷新列表（显示列表）
    private Base target;// 跟随对象（图形引用必须非空。以其图形中心坐标为准）

    public Camera(int width, int height) {
        viewportWidth = width;
        viewportHeight = height;
        zoomFactor = 1.0;
        cameraPos = new Point2D();
        BaseFlashingList = new LinkedList<>();
        target = null;
        if (LogUtils.isLoggable(Level.FINE)) {
            LogUtils.fine("Camera(%d) CREATED.", this.hashCode());
        }
    }

    /**
     * 设置相机位置并刷新显示列表
     */
    public void setCameraPos(Point2D cameraPos) {
        this.cameraPos = cameraPos;
        flashBasesShowing();
    }

    /**
     * 仅设置相机位置，而不刷新显示列表
     */
    public void setCameraPosWithoutFlash(Point2D cameraPos) {
        this.cameraPos = cameraPos;
    }

    public Point2D getCameraPosAbs() {
        Point2D absPos = new Point2D(cameraPos);
        if (target != null) {
            absPos.add(target.getTransform2D().getShape().getCenterPointAbsolute());
            Point2D focusCenterRel = getFocusCenterRel();
            absPos.sub(focusCenterRel);
        }
        return absPos;
    }

    public Point2D getCameraPosRel() {
        return new Point2D(cameraPos);
    }

    /**
     * 设置跟随对象并刷新
     */
    public void setTarget(Base target) {
        this.target = target;
        flashBasesShowing();
    }

    /**
     * 仅设置跟随对象，而不刷新
     */
    public void setTargetWithoutFlash(Base target) {
        this.target = target;
    }

    /**
     * 设置刷新列表。
     * <p>为确保数据的一致性，一般情况下仅供SceneMapManager使用。最好不要独自使用。</p>
     */
    public void setBaseFlashingList(LinkedList<Base> baseFlashingList) {
        this.BaseFlashingList = baseFlashingList;
    }

    /**
     * 设置缩放倍数并刷新。
     * <p>0.1~10倍之间</p>
     */
    public void setZoomFactor(double zoomFactor) {
        this.zoomFactor = Math.min(Math.max(0.1, zoomFactor), 10);
        flashBasesShowing();
    }
    /**
     * 仅设置缩放倍数，但不刷新。
     * <p>0.1~10倍之间</p>
     */
    public void setZoomFactorWithoutFlash(double zoomFactor) {
        this.zoomFactor = Math.min(Math.max(0.1, zoomFactor), 10);
    }

    public double getZoomFactor() {
        return zoomFactor;
    }

    public void setViewportWidth(int viewportWidth) {
        this.viewportWidth = viewportWidth;
    }

    public void setViewportHeight(int viewportHeight) {
        this.viewportHeight = viewportHeight;
    }

    /**
     * 平移并刷新显示列表
     * @param dP 位置变化量
     */
    public void move(Point2D dP) {
        cameraPos.add(dP);
        flashBasesShowing();
    }
    /**
     * 仅平移，而不刷新显示列表
     * @param dP 位置变化量
     */
    public void moveWithoutFlash(Point2D dP) {
        cameraPos.add(dP);
    }

    /**
     * 缩放并刷新显示列表。
     * <p>0.1~10倍之间</p>
     * @param zoomDelta 缩放变化量
     */
    public void zoom(double zoomDelta) {
        zoomFactor = Math.min(Math.max(0.1, zoomFactor + zoomDelta), 10);
        flashBasesShowing();
    }
    /**
     * 仅缩放，而不刷新显示列表。
     * <p>0.1~10倍之间</p>
     * @param zoomDelta 缩放变化量
     */
    public void zoomWithoutFlash(double zoomDelta) {
        zoomFactor = Math.min(Math.max(0.1, zoomFactor + zoomDelta), 10);
    }

    /**
     * 重置相机并刷新显示列表
     */
    public void reset() {
        cameraPos = new Point2D();
        zoomFactor = 1.0;
        flashBasesShowing();
        LogUtils.fineOnly("Camera is reset.Then flashed.");
        LogUtils.finerOnly("Camera is reset.cameraPos = %s, zoomFactor = %d.Then flashed.",
                cameraPos, zoomFactor);
    }
    /**
     * 仅重置相机，但不刷新显示列表
     */
    public void resetWithoutFlash() {
        cameraPos = new Point2D();
        zoomFactor = 1.0;
        LogUtils.fineOnly("Camera is reset.");
        LogUtils.finerOnly("Camera is reset.cameraPos = %s, zoomFactor = %d.",
                cameraPos, zoomFactor);
    }

    /**
     * 获取视口中心点的世界坐标（焦点世界坐标）
     */
    public Point2D getFocusCenterAbs() {
        Point2D absPos = getCameraPosAbs();
        return new Point2D(absPos.x + viewportWidth / 2.0, absPos.y + viewportHeight / 2.0);
    }
    /**
     * 获取视口中心点的相机坐标（焦点相机坐标）
     */
    public Point2D getFocusCenterRel() {
        return new Point2D(viewportWidth / 2.0, viewportHeight / 2.0);
    }

    /**
     * 将世界坐标转换为相机坐标
     * @param worldPos 世界坐标
     * @return 相机坐标
     */
    public Point2D worldToCamera(Point2D worldPos) {
        Point2D focusCenterAbs = getFocusCenterAbs();
        Point2D focusCenterRel = getFocusCenterRel();
        return new Point2D(
                (worldPos.x - focusCenterAbs.x) * zoomFactor + focusCenterRel.x,
                (worldPos.y - focusCenterAbs.y) * zoomFactor + focusCenterRel.y
        );
    }

    /**
     * 将相机坐标转换为世界坐标
     * @param cameraPos 相机坐标
     * @return 世界坐标
     */
    public Point2D cameraToWorld(Point2D cameraPos) {
        Point2D focusCenterAbs = getFocusCenterAbs();
        Point2D focusCenterRel = getFocusCenterRel();
        return new Point2D(
                (cameraPos.x - focusCenterRel.x) / zoomFactor + focusCenterAbs.x,
                (cameraPos.y - focusCenterRel.y) / zoomFactor + focusCenterAbs.y
        );

    }

    /**
     * TODO 应用到Shape2D（若不用视口，可不应用）
     * 世界点是否可见（在视口内）
     * @param worldPos 世界点坐标
     * @return 是否可见
     */
    public boolean isWorldPointVisible(Point2D worldPos) {
        Point2D cameraPos = worldToCamera(worldPos);
        // 判断物体是否在视口内
        return (cameraPos.x >= 0 && cameraPos.x <= viewportWidth &&
                cameraPos.y >= 0 && cameraPos.y <= viewportHeight);
    }

    /**
     * TODO 应用到Shape2D（若不用视口，可不应用）
     * 相机点是否可见（在视口内）
     * @param cameraPos 相机点坐标
     * @return 是否可见
     */
    public boolean isCameraPointVisible(Point2D cameraPos) {
        return (cameraPos.x >= 0 && cameraPos.x <= viewportWidth &&
                cameraPos.y >= 0 && cameraPos.y <= viewportHeight);
    }

    /**
     * 刷新单个对象的显示
     * @param base 对象
     */
    public void flashBaseShowing(Base base) {
        Transform2D tf = base.getTransform2D();
        tf.flashShowLocation();
        Shape2D s = tf.getShape();
        if (s != null){
            s.flashShowCenterPoint();
            s.flashShowSizeParams();
            s.flashShowVertexList();
        }
    }

    /**
     * 刷新对象列表中的所有对象的显示
     */
    public void flashBasesShowing() {
        for (Base base : BaseFlashingList) {
            flashBaseShowing(base);
        }
    }

}
