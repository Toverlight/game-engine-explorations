package engine.base;

import engine.base.feature.Persistable;

import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.stream.Collectors;

/**
 * <h1>游戏场景</h1>
 * <p>
 *     必须重载的方法中：<br>
 *     加载资源、初始化场景操作均在start内调用；<br>
 *     演出操作在update内调用；<br>
 *     卸载资源在destroy内调用。
 * </p>
 */
public abstract class GameScene extends GameBase {
    private final List<GameObject> gameObjects = new LinkedList<>(); // 场景内的所有对象
    private boolean isLoaded = false; // 是否已经加载

    // --- 生命周期方法 --- //
    @Override
    public void start() {
        loadResources();
        initializeScene();
        isLoaded = true;
    }

    @Override
    public void update(double deltaTime) {
        if (!isActive() || !isLoaded) return;

        for (GameObject obj : gameObjects) {
            obj.update(deltaTime);
        }
        perform(deltaTime);
    }

    @Override
    public void destroy() {
        unloadResources();
        gameObjects.clear();
        isLoaded = false;
    }

    // --- 场景特有方法 ---//

    /**
     * <h1>添加游戏实体</h1>
     * <p>
     *     触发游戏实体的初始化。
     * </p>
     * @param obj 游戏实体
     */
    public void addGameObject(GameObject obj) {
        gameObjects.add(obj);
        obj.start();
    }

    /**
     * <h1>移除游戏实体</h1>
     * <p>
     *     触发游戏实体的销毁。
     * </p>
     * @param obj 游戏实体
     */
    public void removeGameObject(GameObject obj) {
        gameObjects.remove(obj);
        obj.destroy();
    }


    //--- 抽象方法（子类实现具体逻辑）---//

    /**
     * 加载资源（如图片、音效）
     */
    protected abstract void loadResources();

    /**
     * 初始化场景布局
     */
    protected abstract void initializeScene();

    /**
     * 卸载资源
     */
    protected abstract void unloadResources();

    /**
     * 场景本身的更新（演出）
     */
    protected abstract void perform(double deltaTime);

    // --- 持久化相关 --- //

    /**
     * <h1>获取持久化对象列表</h1>
     */
    public List<Persistable> getPersistableS() {
        return gameObjects.stream()
                .filter(obj -> obj instanceof Persistable)
                .map(obj -> (Persistable) obj)
                .collect(Collectors.toList());
    }

    /**
     * <h1>恢复持久化对象</h1>
     * @param persistableS 持久化对象列表
     */
    public void restorePersistableS(List<Persistable> persistableS) {
        if (persistableS == null) {
            LogUtils.finer("No persistable to be restored.");
            return;
        }
        for (Persistable obj : persistableS) {
            String key = obj.getPersistKey();
            Map<String, Object> data = GameStateManager.load(key);
            if (data != null) {
                obj.deserialize(data);
            } else {
                LogUtils.severe("Persistable(pkey %s) failed to restore, because data can't manage to load.",
                        obj.getPersistKey());
            }

        }
    }

    /**
     * <h1>销毁场景中的非持久化对象</h1>
     */
    public void destroyNonPersistent() {
        List<GameObject> toRemove = gameObjects.stream()
                .filter(obj -> !(obj instanceof Persistable))
                .collect(Collectors.toList());
        toRemove.forEach(GameObject::destroy);
        gameObjects.removeAll(toRemove);
    }
}
