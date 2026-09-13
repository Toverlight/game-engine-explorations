package xyz.topover.core.paint;

import javafx.scene.canvas.GraphicsContext;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import xyz.topover.core.base.GameComponent;
import xyz.topover.core.run.GameRunner;

/**
 * <h1>Shape渲染组件</h1>
 * <p>
 *     层级默认为5.
 * </p>
 * TODO 未来还会增加绘制图片等功能。
 */
public class SpriteRenderer extends GameComponent implements Drawable {
    private Shape shape; // 持有的形状
    private int layer = 5; // 层级
    private static final Logger logger = LoggerFactory.getLogger(SpriteRenderer.class);

    public SpriteRenderer(Shape shape) {
        this.shape = shape;
    }
    public SpriteRenderer(Shape shape, int layer) {
        this.shape = shape;
        this.layer = layer;
    }

    public Shape getShape() {
        return shape;
    }

    @Override
    public void start() {
        // 绑定GameObject的Transform到Shape
        if (getGameObject() == null) {
            logger.error("SpriteRenderer(id {}) must start after GameObject being set! Maybe you add it without using addComponent method. And isActive was set false for security.", getId());
            setActive(false);
        } else {
            shape.setTransform(getGameObject().getTransform());
            setActive(true);
        }
    }

    @Override
    public void draw(GraphicsContext gc) {
        if (shape != null) {
            shape.draw(gc);
        }
        // 绘制图片
    }

    @Override
    public int getLayer() {
        return layer;
    }

    public void setLayer(int layer) {
        this.layer = layer;
    }
}
