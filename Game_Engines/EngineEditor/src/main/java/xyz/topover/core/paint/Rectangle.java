package xyz.topover.core.paint;

import xyz.topover.core.calc.Point2D;
import xyz.topover.core.view.CameraManager;

import java.util.Arrays;

/**
 * <h1>矩形</h1>
 * <p>
 *     适配物理引擎Physics
 * </p>
 */
public class Rectangle extends Polygon {
    private double width;
    private double height;

    public Rectangle(double width, double height) {
        super(new Point2D[] { new Point2D(-width / 2, height / 2), new Point2D(width / 2, height / 2),
            new Point2D(width / 2, -height / 2), new Point2D(-width / 2, -height / 2) });
        this.width = width;
        this.height = height;
    }

    public void setWidth(double width) {
        this.width = width;
        vertices[0].x = -width / 2;
        vertices[1].x = width / 2;
        vertices[2].x = width / 2;
        vertices[3].x = -width / 2;
    }

    public double getWidth() {
        return width;
    }

    public void setHeight(double height) {
        this.height = height;
        vertices[0].y = height / 2;
        vertices[1].y = height / 2;
        vertices[2].y = -height / 2;
        vertices[3].y = -height / 2;
    }

    public double getHeight() {
        return height;
    }

    /**
     * 不推荐在该派生类中使用，会导致width和height与顶点集数据不一致。
     */
    @Deprecated
    @Override
    public void setVertices(Point2D[] vertices) {
        super.setVertices(vertices);
    }
}
