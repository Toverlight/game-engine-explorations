package xyz.topover.core.base;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * <h1>可附加到GameObject的功能组件</h1>
 * <p>渲染、物理等</p>
 */
public abstract class GameComponent extends GameBase{
    protected GameObject gameObject; // 所属的GameObject
    private static final Logger logger = LoggerFactory.getLogger(GameComponent.class);

    @Override
    protected void logConstruction() {
        logger.debug("GameComponent(id {}) created.", getId());
    }

    public void setGameObject(GameObject obj) {
        this.gameObject = obj;
    }

    public GameObject getGameObject() { return gameObject; }
}
