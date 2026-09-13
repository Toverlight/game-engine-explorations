package engine.derive.old;

import engine.base.Point2D;
import engine.base.old.Shape2D;
import engine.base.old.ShapeType;

/**
 * 三角形
 */
public class Triangle extends Shape2D {

    public Triangle(Point2D center, Point2D A, Point2D B, Point2D C) {
        super(ShapeType.Triangle);
        Point2D[] vertexList = new Point2D[3];
        vertexList[0] = A;
        vertexList[1] = B;
        vertexList[2] = C;
        setVertexListWithoutFlash(vertexList);
        setCenterPoint(center);
    }
}
