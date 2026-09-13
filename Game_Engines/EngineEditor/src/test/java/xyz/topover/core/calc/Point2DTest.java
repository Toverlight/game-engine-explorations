package xyz.topover.core.calc;

import static org.junit.jupiter.api.Assertions.*;
import org.junit.jupiter.api.Test;

public class Point2DTest {
    @Test
    public void testFourCalc() {
        Point2D a = new Point2D(2,3);
        Point2D b = new Point2D(3, -4);
        a.add(b);
        pointsEquation(new Point2D(5, -1), a);
        a.sub(b);
        pointsEquation(new Point2D(2, 3), a);
        a.scale(0.5);
        pointsEquation(new Point2D(1, 1.5), a);
        a.scale(b);
        pointsEquation(new Point2D(3, -6), a);
    }

    @Test
    public void testRotation() {
        Point2D a = new Point2D(2, 0);
        a.rotate(Math.PI / 3);
        pointsEquation(new Point2D(1, Math.sqrt(3)), a);
    }

    @Test
    public void testNorm() {
        Point2D a = new Point2D(3, 4);
        assertEquals(5.0, a.norm(), 1e-9);
    }

    @Test
    public void testDot() {
        Point2D a = new Point2D(3, 4);
        Point2D b = new Point2D(-4, -5);
        assertEquals(-32.0, a.dot(b), 1e-9);
    }

    @Test
    public void testIncludeAngle() {
        Point2D a = new Point2D(3, 4);
        Point2D b = new Point2D(4, -3);
        assertEquals(Math.PI / 2, b.includedAngle(a), 1e-9);
    }

    @Test
    public void testNormalization() {
        Point2D a = new Point2D(3, 4);
        a.normalize();
        pointsEquation(new Point2D(0.6, 0.8), a);
    }

    @Test
    public void testDis() {
        Point2D a = new Point2D(3,4);
        Point2D b = new Point2D(-1, 1);
        assertEquals(5.0, a.dis(b), 1e-9);
    }

    public static void pointsEquation(Point2D a, Point2D b) {
        assertEquals(a.x, b.x, 1e-9);
        assertEquals(a.y, b.y, 1e-9);
    }
}
