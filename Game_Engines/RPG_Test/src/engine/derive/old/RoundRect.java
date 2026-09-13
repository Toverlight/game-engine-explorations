package engine.derive.old;

import engine.base.Point2D;
import engine.base.old.Shape2D;
import engine.base.old.ShapeType;

/**
 * 圆角矩形
 */
public class RoundRect extends Shape2D {

    public RoundRect(Point2D center, Point2D size, Point2D arc) {
        super(ShapeType.RoundRect);
        Point2D[] vertexList = new Point2D[1];
        vertexList[0] = Point2D.invert(Point2D.scale(size, 0.5));
        setVertexListWithoutFlash(vertexList);
        setSize(size);
        setArc(arc);
        setCenterPoint(center);
    }
}
