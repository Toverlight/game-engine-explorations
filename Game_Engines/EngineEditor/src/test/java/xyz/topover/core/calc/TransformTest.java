package xyz.topover.core.calc;

import static org.junit.jupiter.api.Assertions.*;
import org.junit.jupiter.api.Test;

public class TransformTest {
    @Test
    public void testSelf() {
        Transform t = new Transform(Transform.I.T); // S->R->T
        Point2D p = new Point2D(1, 1);
        t.setPosition(2, 3);// +2, +3
        t.translate(4, 5);// +6, +8
        t.setScale(2, 3); // 2, 3
        t.setRotation(Math.PI / 2); // -3, 2
        Point2D p_ = t.getLocalToWorldMatrix().transform(p); // 3, 10
        Point2D p_2 = t.getWorldToLocalMatrix().transform(p_); // 1, 1

        Point2DTest.pointsEquation(new Point2D(3, 10), p_);
        Point2DTest.pointsEquation(p, p_2);
    }
}
