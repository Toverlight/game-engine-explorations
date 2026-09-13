package engine.derive;

import engine.base.GameComponent;
import engine.base.LogUtils;
import engine.base.Shape;
import project.inherent.GameRunner;

/**
 * <h1>Shape渲染组件</h1>
 */
public class SpriteRenderer extends GameComponent {
    private Shape shape; // 持有的形状

    public SpriteRenderer(Shape shape) {
        this.shape = shape;
    }

    @Override
    public void start() {
        // 绑定GameObject的Transform到Shape
        if (getGameObject() == null) {
            LogUtils.severe("SpriteRenderer(id %s) must start after GameObject being set! " +
                    "Maybe you add it without using addComponent method.", getId());
        }
        shape.setTransform(getGameObject().getTransform());
    }

    @Override
    public void update(double deltaTime) {
        if (isActive()) {// 每帧绘制
            shape.draw(GameRunner.getGc());
        }
    }
}
