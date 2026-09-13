package engine.derive.old;

import engine.base.Point2D;
import engine.base.old.Shape2D;
import engine.base.old.ShapeType;

/**
 * 椭圆类
 */
public class Oval extends Shape2D {

    public Oval(Point2D center, Point2D size) {
        super(ShapeType.Oval);
        Point2D[] vertexList = new Point2D[1];
        vertexList[0] = Point2D.invert(Point2D.scale(size, 0.5));
        setVertexListWithoutFlash(vertexList);
        setSize(size);
        setCenterPoint(center);
    }
}
