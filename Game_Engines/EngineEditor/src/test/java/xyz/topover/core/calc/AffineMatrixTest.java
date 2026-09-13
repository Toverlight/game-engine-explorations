package xyz.topover.core.calc;

import static org.junit.jupiter.api.Assertions.*;
import org.junit.jupiter.api.Test;

public class AffineMatrixTest {
    @Test
    public void testTranslation() {
        AffineMatrix a = new AffineMatrix();
        a.translate(3, 4);
        a.translate(-2, -8);
        AffineMatrix b = new AffineMatrix();
        b.translate(1, -4);
        affineEquation(a, b);
        Point2D p = new Point2D(-2, 3);
        Point2DTest.pointsEquation(new Point2D(-1, -1), a.transform(p));
    }

    @Test
    public void testRotation() {
        AffineMatrix a = new AffineMatrix();
        a.translate(2, 1);
        a.rotate(Math.PI / 2);
        Point2D p = new Point2D(1, 3);
        Point2DTest.pointsEquation(new Point2D(-4, 3), a.transform(p));
    }

    @Test
    public void testScaling() {
        AffineMatrix a = new AffineMatrix();
        a.translate(2, 3);
        a.scale(5);
        Point2D p = new Point2D(1, -4);
        Point2DTest.pointsEquation(new Point2D(15, -5), a.transform(p));
    }

    @Test
    public void testTRST() {
        AffineMatrix a = new AffineMatrix();
        a.translate(4, 5);
        a.scale(2, -1);
        a.rotate(Math.PI);
        a.translate(4, 2);
        Point2D p = new Point2D(1, -2);
        Point2DTest.pointsEquation(new Point2D(-6, 5), a.transform(p));
    }

    private void affineEquation(AffineMatrix a, AffineMatrix b) {
        assertEquals(a.get(0, 0), b.get(0, 0), 1e-9);
        assertEquals(a.get(1, 0), b.get(1, 0), 1e-9);
        assertEquals(a.get(0, 1), b.get(0, 1), 1e-9);
        assertEquals(a.get(1, 1), b.get(1, 1), 1e-9);
        assertEquals(a.get(0, 2), b.get(0, 2), 1e-9);
        assertEquals(a.get(1, 2), b.get(1, 2), 1e-9);
    }
}
