package xyz.topover.core.paint;

import javafx.scene.canvas.GraphicsContext;
import javafx.scene.paint.Color;
import xyz.topover.core.calc.Point2D;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Random;

/**
 * <h1>粒子系统</h1>
 * <p>
 *     负责粒子的发射、更新和渲染。
 * </p>
 */
public class ParticleSystem {
    private final List<Particle> particles = new ArrayList<>();
    private final Random random = new Random();

    public void emit(int count, Point2D origin) {
        for (int i = 0; i < count; i++) {
            double angle = random.nextDouble() * Math.PI * 2;
            double speed = random.nextDouble() * 90 + 10; // 速度范围：10～100
            Point2D velocity = new Point2D(Math.cos(angle) * speed, Math.sin(angle) * speed);
            double lifetime = random.nextDouble() * 5 + 1; // 生命周期：1～6秒
            // 粒子初始颜色为白色
            Particle p = new Particle(origin, velocity, lifetime, Color.WHITE);
            particles.add(p);
        }
    }

    // 更新所有粒子状态，并移除已经过期的粒子
    public void update(double deltaTime) {
        Iterator<Particle> it = particles.iterator();
        while (it.hasNext()) {
            Particle p = it.next();
            p.update(deltaTime);
            if (!p.isAlive()) {
                it.remove();
            }
        }
    }

    // 渲染所有粒子
    public void render(GraphicsContext gc) {
        for (Particle p : particles) {
            // 根据粒子的存活比例计算透明度
            float alpha = (float)(1 - p.passedTime / p.lifetime);
            alpha = Math.max(0, Math.min(1, alpha));
            Color c = new Color(p.color.getRed(), p.color.getGreen(), p.color.getBlue(), alpha);
            gc.setFill(c);
            // 绘制粒子（这里用一个4x4的小圆点）
            gc.fillOval((int)p.position.x, (int)p.position.y, 4, 4);
        }
    }

}
