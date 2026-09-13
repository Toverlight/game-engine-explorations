package engine.derive.old;

import engine.base.Point2D;
import engine.base.old.Shape2D;
import engine.base.old.ShapeType;

/**
 * 多边形
 */
public class Polygon extends Shape2D {

    public Polygon(Point2D center, Point2D[] point2DS) {
        super(ShapeType.Polygon);
        setVertexListWithoutFlash(point2DS);
        setCenterPoint(center);
    }
}
