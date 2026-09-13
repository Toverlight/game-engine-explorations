package engine.base.old;

/**
 * 形状类型
 */
public enum ShapeType {
    /**
     * 圆。
     * leftTopPoint;
     * radius
     */
    Circle,
    /**
     * 椭圆。
     * leftTopPoint,sizePoint
     */
    Oval,
    /**
     * 矩形。
     * leftTopPoint,sizePoint
     */
    Rect,
    /**
     * 圆角矩形。
     * leftTopPoint,sizePoint,arcPoint
     */
    RoundRect,
    /**
     * 三角形。
     * 3(point)
     */
    Triangle,
    /**
     * 梯形（不规则四边形）。
     * 4(point)
     */
    Trapezoid,
    /**
     * 多边形。
     * n(>4)(point)
     */
    Polygon
}
