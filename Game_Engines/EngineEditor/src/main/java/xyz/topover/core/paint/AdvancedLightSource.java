package xyz.topover.core.paint;

import javafx.scene.paint.Color;
import xyz.topover.core.base.GameComponent;
import xyz.topover.core.calc.Point2D;

/**
 * <h1>高级光源组件</h1>
 * <p>
 *     TODO 还没开发好。
 * </p>
 */
public class AdvancedLightSource extends GameComponent {
    private double len; // 最大照明范围
    private Point2D dir; // 中轴方向
    private double angle; // 张角
    private Color color; // 光源颜色
    private double intensity; // 光强
    private FadeFunc fadeFunc; // 衰减函数

    public AdvancedLightSource(double len, Point2D dir, double angle, Color color, double srcIntensity, FadeFunc fadeFunc) {
        this.len = len;
        this.dir = new Point2D(dir);
        this.angle = angle;
        this.color = color;
        this.intensity = srcIntensity;
        this.fadeFunc = fadeFunc;
    }

    @Override
    public void start() {
        // TODO start
    }

    @Override
    public void update(double deltaTime) {
        // TODO update
    }
}
