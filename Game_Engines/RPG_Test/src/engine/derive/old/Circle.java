package engine.derive.old;

import engine.base.Point2D;
import engine.base.old.Shape2D;
import engine.base.old.ShapeType;


/**
 * 圆形类
 */
public class Circle extends Shape2D {

    public Circle(Point2D center, double radius) {
        super(ShapeType.Circle);
        Point2D[] vertexList = new Point2D[1];
        vertexList[0] = new Point2D(-radius, -radius);
        setVertexListWithoutFlash(vertexList);
        setRadius(radius);
        setCenterPoint(center);
    }



}
