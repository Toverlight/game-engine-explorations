package xyz.topover.core.paint;

import javafx.scene.canvas.GraphicsContext;

/**
 * <h1>可绘制的</h1>
 * <p>
 *     分为绘制和层级两个函数。<br>
 *     关于<b>层级</b>：层级越高，绘制在越上方。
 * </p>
 */
public interface Drawable {
    void draw(GraphicsContext gc);
    default int getLayer() { return 5; };
}
