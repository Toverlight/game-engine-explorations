package xyz.topover.core.paint;

import xyz.topover.core.calc.Point2D;

import java.util.Arrays;

/**
 * <h1>矩形碰撞箱</h1>
 */
public class CBoxRectangle extends Rectangle implements CollisionBox{

    public CBoxRectangle(double width, double height) {
        super(width, height);
    }

    @Override
    public Point2D[] getWorldVertices() {
        Point2D[] worldVertices = new Point2D[vertices.length];
        for (int i = 0; i < vertices.length; i++) {
            worldVertices[i] = transform.getLocalToWorldMatrix().transform(vertices[i]);
        }
        return worldVertices;
    }

    @Override
    public void setRotation(double angle) {
        transform.setRotation(angle);
    }

    @Override
    public void rotate(double angle) {
        transform.rotate(angle);
    }

    @Override
    public double getRotation() {
        return transform.getRotation();
    }

    @Override
    public Point2D getScale() {
        return transform.getScale();
    }

    @Override
    public Point2D getPosition() {
        return transform.getPosition();
    }

    @Override
    public Point2D[] getAxes(Point2D[] worldVertices) {
        Point2D[] axes = new Point2D[vertices.length];
        axes[0] = Point2D.minus(worldVertices[0], worldVertices[vertices.length - 1]);
        for (int i = 1; i < vertices.length; i++) {
            axes[i] = Point2D.normalize(Point2D.minus(worldVertices[i], worldVertices[i - 1]));
        }
        return axes;
    }

    @Override
    public Point2D[] projectMinMax(Point2D[] worldVertices, Point2D axis) {
        double[] projPoints = new double[vertices.length];
        double min = Double.MAX_VALUE;
        double max = -Double.MAX_VALUE;
        int minIndex = 0;
        int maxIndex = 0;
        for (int i = 0; i < vertices.length; i++) {
            projPoints[i] = worldVertices[i].dot(axis);
            if (projPoints[i] < min) {
                min = projPoints[i];
                minIndex = i;
            }
            else if (projPoints[i] > max) {
                max = projPoints[i];
                maxIndex = i;
            }
        }
        return new Point2D[]{new Point2D(min, max), new Point2D(worldVertices[minIndex]),
                new Point2D(worldVertices[maxIndex])};
    }
}
