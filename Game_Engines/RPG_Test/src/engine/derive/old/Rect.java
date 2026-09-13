package engine.derive.old;

import engine.base.Point2D;
import engine.base.old.Shape2D;
import engine.base.old.ShapeType;

/**
 * 矩形
 */
public class Rect extends Shape2D {

    public Rect(Point2D center, Point2D size) {
        super(ShapeType.Rect);
        Point2D[] vertexList = new Point2D[1];
        vertexList[0] = Point2D.invert(Point2D.scale(size, 0.5));
        setVertexListWithoutFlash(vertexList);
        setSize(size);
        setCenterPoint(center);
    }
}
