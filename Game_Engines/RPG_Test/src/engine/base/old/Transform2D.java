package engine.base.old;

import engine.base.LogUtils;
import engine.base.Point2D;
import engine.derive.old.*;

import java.util.logging.Level;

/**
 * 二维变换类
 */
public class Transform2D {
    private Point2D location;// 变换坐标
    private Point2D showLocation;// 显示坐标
    private Shape2D shape;// 形状
    private Point2D displacement;// 与上一帧的位移

    public Transform2D() {
        location = new Point2D();
        showLocation = new Point2D();
        displacement = new Point2D();
    }

    /**
     * 设置变换位置并刷新显示位置
     * @param location 新变换坐标
     */
    public void setLocation(Point2D location) {
        displacement = Point2D.minus(location, this.location);
        this.location = location;
        flashShowLocation();
        if (shape != null) {
            shape.setCenterPoint(this.location);
        }
    }
    /**
     * 仅设置变换位置，而不刷新显示位置
     * @param location 新变换坐标
     */
    public void setLocationWithoutFlash(Point2D location) {
        displacement = Point2D.minus(location, this.location);
        this.location = location;
        if (shape != null) {
            shape.setCenterPointWithoutFlash(this.location);
        }
    }

    /**
     * 忽略图形位置仅设置变换位置并刷新坐标位置
     * @param location 新变换坐标
     */
    public void setLocationIgnoreShape(Point2D location) {
        this.location = location;
        flashShowLocation();
    }

    /**
     * 刷新坐标显示
     */
    public void flashShowLocation() {
        showLocation = SceneMapManager.getCamera().worldToCamera(this.location);
    }

    /**
     * 平移并刷新显示
     * @param displacement 位移
     */
    public void move(Point2D displacement) {
        this.displacement = displacement;
        location.add(displacement);
        flashShowLocation();
        if (shape != null) {
            shape.setCenterPoint(location);
        }
    }

    /**
     * 仅平移，但不刷新显示
     * @param displacement 位移
     */
    public void moveWithoutFlash(Point2D displacement) {
        this.displacement = displacement;
        location.add(displacement);
        if (shape != null) {
            shape.setCenterPointWithoutFlash(location);
        }
    }

    /**
     * 获取坐标的拷贝
     */
    @Deprecated
    public Point2D getLocationCopy() {
        return new Point2D(location);
    }

    /**
     * 获取位移的拷贝
     */
    @Deprecated
    public Point2D getDisplacementCopy() {
        return new Point2D(displacement);
    }

    /**
     * 获取形状的拷贝（包含放缩向量）
     */
    @Deprecated
    public Shape2D getShapeCopy() {
        Shape2D shape2D = null;
        switch (shape.getShapeType()) {

            case Circle:
                shape2D = new Circle(shape.getCenterPointRelative(), shape.getRadius());
                shape2D.setScalePoint(shape.getScalePoint());
                break;
            case Oval:
                shape2D = new Oval(shape.getCenterPointRelative(), shape.getSize());
                shape2D.setScalePoint(shape.getScalePoint());
                break;
            case Rect:
                shape2D = new Rect(shape.getCenterPointRelative(), shape.getSize());
                shape2D.setScalePoint(shape.getScalePoint());
                break;
            case RoundRect:
                shape2D = new RoundRect(shape.getCenterPointRelative(), shape.getSize(), shape.getArc());
                shape2D.setScalePoint(shape.getScalePoint());
                break;
            case Triangle:
                Point2D[] vertexListTri = shape.getVertexList();
                shape2D = new Triangle(shape.getCenterPointRelative(), vertexListTri[0], vertexListTri[1], vertexListTri[2]);
                shape2D.setScalePoint(shape.getScalePoint());
                break;
            case Trapezoid:
                Point2D[] vertexListTra = shape.getVertexList();
                shape2D = new Trapezoid(shape.getCenterPointRelative(), vertexListTra[0], vertexListTra[1], vertexListTra[2],
                        vertexListTra[3]);
                shape2D.setScalePoint(shape.getScalePoint());
                break;
            case Polygon:
                Point2D[] vertexListPoly = shape.getVertexList();
                shape2D = new Polygon(shape.getCenterPointRelative(), vertexListPoly);
                shape2D.setScalePoint(shape.getScalePoint());
                break;
        }
        return shape2D;
    }

    /**
     * 获取坐标
     */
    public Point2D getLocation() {
        return location;
    }

    /**
     * 获取显示坐标
     */
    public Point2D getShowLocation() {
        return showLocation;
    }

    /**
     * 获取位移
     */
    public Point2D getDisplacement() {
        return displacement;
    }

    /**
     * 获取形状
     */
    public Shape2D getShape() {
        return shape;
    }

    /**
     * 等比放缩并刷新
     */
    public void scale(double factor) {
        shape.setScalePoint(new Point2D(factor, factor));
    }

    /**
     * 仅等比放缩，而不刷新
     */
    public void scaleWithoutFlash(double factor) {
        shape.setScalePointWithoutFlash(new Point2D(factor, factor));
    }

    /**
     * 不等比放缩并刷新
     * <p>安全地设置放缩向量，对 <0 的分量取 0</p>
     */
    public void setScalePoint(Point2D scalePoint) {
        shape.setScalePoint(scalePoint);
    }

    /**
     * 仅不等比放缩，而不刷新
     * <p>安全地设置放缩向量，对 <0 的分量取 0</p>
     */
    public void setScalePointWithoutFlash(Point2D scalePoint) {
        shape.setScalePointWithoutFlash(scalePoint);
    }

    /**
     * 获取放缩向量的拷贝
     */
    public Point2D getScalePoint() {
        return new Point2D(shape.getScalePoint());
    }

    /**
     * 设置图形并刷新
     */
    public void setShape(Shape2D shape) {
        this.shape = shape;
        if (this.shape != null){
            this.shape.flashShowCenterPoint();
            this.shape.flashShowSizeParams();
            this.shape.flashShowVertexList();
            if (LogUtils.isLoggable(Level.FINER)) {
                LogUtils.finer("Transform2D(%d) set shape %s(%d) and flashed.", this.hashCode(),
                        this.shape.getShapeType(), this.shape.hashCode());
            }
        } else {
            if (LogUtils.isLoggable(Level.FINER)) {
                LogUtils.finer("Transform2D(%d) set shape null and flashed.", this.hashCode());
            }
        }
    }

    /**
     * 仅设置图形，而不刷新
     */
    public void setShapeWithoutFlash(Shape2D shape) {
        this.shape = shape;
        if (this.shape != null) {
            if (LogUtils.isLoggable(Level.FINER)) {
                LogUtils.finer("Transform2D(%d) set shape %s(%d).", this.hashCode(),
                        this.shape.getShapeType(), this.shape.hashCode());
            }
        } else {
            if (LogUtils.isLoggable(Level.FINER)) {
                LogUtils.finer("Transform2D(%d) set shape null.", this.hashCode());
            }
        }
    }

    public ShapeType getShapeType() {
        return shape.getShapeType();
    }

    /**
     * 安全地旋转坐标向量（图形平移跟进）并刷新
     * @param radian 弧度
     */
    public void rotateLocation(double radian) {
        setLocation(Point2D.rotate(location, radian));
    }

    /**
     * 仅旋转坐标向量，但不刷新
     * @param radian 弧度
     */
    public void rotateLocationWithoutFlash(double radian) {
        setLocationWithoutFlash(Point2D.rotate(location, radian));
    }

    /**
     * 安全地进行图形旋转并刷新
     * @param radian 弧度
     */
    public void rotateShape(double radian) {
        if (shape != null) {
            shape.rotate(radian);
        }
    }

    /**
     * 仅图形旋转，而不刷新
     * @param radian 弧度
     */
    public void rotateShapeWithoutFlash(double radian) {
        if (shape != null) {
            shape.rotateWithoutFlash(radian);
        }
    }

    /**
     * 安全地进行对象旋转（坐标旋转+图形旋转）并刷新
     * @param radian 弧度
     */
    public void rotate(double radian) {
        rotateLocation(radian);
        rotateShape(radian);
    }
    /**
     * 仅对象旋转（坐标旋转+图形旋转），而不刷新
     * @param radian 弧度
     */
    public void rotateWithoutFlash(double radian) {
        rotateLocationWithoutFlash(radian);
        rotateShapeWithoutFlash(radian);
    }
}
