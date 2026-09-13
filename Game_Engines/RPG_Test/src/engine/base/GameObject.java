package engine.base;

import java.util.LinkedList;
import java.util.List;

/**
 * <h1>游戏实体对象</h1>
 * <p><b>· 空间变换</b>：游戏实体具有空间仿射变换的特性。<br>
 *    <b>· 组件列表</b>：实体对象具有的额外功能。<br>
 *    <b>· 子对象列表</b>：空间变换依赖于此实体对象空间变换的实体对象的集合。</p>
 */
public class GameObject extends GameBase{
    private final Transform transform; // 空间变换组件
    protected final List<GameComponent> components = new LinkedList<>(); // 组件列表
    private final List<GameObject> children = new LinkedList<>(); // 子对象列表

    public GameObject() {
        super();
        this.transform = new Transform(Transform.I.T);
    }

    //--- 组件管理 ---//
    public <T extends GameComponent> T addComponent(T component) {
        component.setGameObject(this);
        components.add(component);
        component.start(); // 初始化组件
        return component;
    }

    public <T extends GameComponent> T getComponent(Class<T> type) {
        for (GameComponent comp : components) {
            if (type.isInstance(comp)) {
                return type.cast(comp);
            }
        }
        return null;
    }

    //--- 子对象管理 ---//
    public void addChild(GameObject child) {
        children.add(child);
        child.getTransform().setParent(this.transform);
    }

    //--- 每帧更新 ---//
    @Override
    public void update(double deltaTime) {
        if (!isActive()) return;

        // 更新所有组件
        for (GameComponent comp : components) {
            comp.update(deltaTime);
        }

        // 更新子对象
        for (GameObject child : children) {
            child.update(deltaTime);
        }
    }

    // Getter
    public Transform getTransform() { return transform; }

}
