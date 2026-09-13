package engine.derive.old;

import engine.base.Point2D;
import engine.base.old.Shape2D;
import engine.base.old.ShapeType;

/**
 * 梯形
 */
public class Trapezoid extends Shape2D {

    public Trapezoid(Point2D center, Point2D A, Point2D B, Point2D C, Point2D D) {
        super(ShapeType.Trapezoid);
        Point2D[] vertexList = new Point2D[4];
        vertexList[0] = A;
        vertexList[1] = B;
        vertexList[2] = C;
        vertexList[3] = D;
        setVertexListWithoutFlash(vertexList);
        setCenterPoint(center);
    }
}
