package xyz.topover.core.paint;

import xyz.topover.core.calc.Point2D;
import xyz.topover.core.calc.Transform;

public interface CollisionBox {
    void setRotation(double angle);
    void rotate(double angle);
    double getRotation();
    Point2D getScale();
    Point2D getPosition();
    /**
     * <h1>获取顶点的世界坐标</h1>
     */
    Point2D[] getWorldVertices();
    /**
     * <h1>获取分离轴</h1>
     * @param worldVertices 顶点的世界坐标
     * @return 分离轴。单位向量
     */
    Point2D[] getAxes(Point2D[] worldVertices);
    /**
     * <h1>向指定轴投影</h1>
     * @param worldVertices 顶点的世界坐标
     * @param axis 分离轴（单位向量）
     * @return [0]:x:min, y:max;[1]:pointMin;[2]:pointMax
     */
    Point2D[] projectMinMax(Point2D[] worldVertices, Point2D axis);
}
