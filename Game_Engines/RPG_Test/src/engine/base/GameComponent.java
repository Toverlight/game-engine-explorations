package engine.base;

/**
 * <h1>可附加到GameObject的功能组件</h1>
 * <p>渲染、物理等</p>
 */
public abstract class GameComponent extends GameBase{
    protected GameObject gameObject; // 所属的GameObject

    public void setGameObject(GameObject obj) {
        this.gameObject = obj;
    }

    public GameObject getGameObject() { return gameObject; }
}
