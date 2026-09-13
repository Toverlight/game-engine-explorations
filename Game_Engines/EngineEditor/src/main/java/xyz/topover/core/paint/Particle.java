package xyz.topover.core.paint;

import javafx.scene.paint.Color;
import xyz.topover.core.calc.Point2D;

/**
 * <h1>粒子</h1>
 * <p>
 *     模拟各种视觉效果。如雨雪、火焰、水花等。
 * </p>
 */
public class Particle {
    Point2D position;
    Point2D velocity;
    double lifetime; // 总生存时间（秒）
    double passedTime; // 度过的时间（秒）
    Color color;

    public Particle(Point2D position, Point2D velocity, double lifetime, Color color) {
        this.position = new Point2D(position);
        this.velocity = new Point2D(velocity);
        this.lifetime = lifetime;
        this.color = color;
        passedTime = 0;
    }

    /**
     * <h1>粒子是否存活</h1>
     */
    public boolean isAlive() {
        return passedTime < lifetime;
    }

    /**
     * <h1>根据时间步长更新粒子状态</h1>
     */
    public void update(double deltaTime) {
        position.add(Point2D.scale(velocity, deltaTime));
        passedTime += deltaTime;
    }
}
