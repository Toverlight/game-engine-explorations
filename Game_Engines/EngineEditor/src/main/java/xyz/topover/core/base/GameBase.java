package xyz.topover.core.base;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.UUID;
import java.util.logging.Level;

/**
 * <h1>游戏基类</h1>
 * <p>提供通用属性和生命周期方法。</p>
 * <p>
 *     <h2>通用属性：</h2>
 *     <b>· id</b>: 对象的UUID，唯一标识符。一经实例化不可更改。<br>
 *     <b>· name</b>: 对象的名称。用于查找和获取实例，允许重复。<br>
 *     <b>· isActive</b>: 对象的激活状态。规定未激活的对象不予更新。默认激活。<br>
 *
 *     <h2>生命周期方法：</h2>
 *     <b>· start():void</b>: 初始化方法。<br>
 *     <b>· update():void</b>: 逻辑更新方法。<br>
 *     <b>· destroy():void</b>: 销毁对象。
 * </p>
 */
public abstract class GameBase {
    private static final Logger logger = LoggerFactory.getLogger(GameBase.class);

    private String id;
    private String name;
    private boolean isActive;

    public GameBase() {
        id = UUID.randomUUID().toString();
        name = "Unnamed";
        isActive = true;
        logConstruction();
    }

    /**
     * <h1>构造时日志输出</h1>
     * <p>
     *     建议重载。标识什么子类构造了。
     * </p>
     */
    protected void logConstruction() {
        logger.debug("GameBase(id {}) created.", id);
    }

    //---生命周期方法---//

    /**
     * <h1>对象初始化</h1>
     */
    public void start() {}

    /**
     * <h1>每帧更新</h1>
     * <p>
     *     基本结构：if (isActive()) [逻辑]
     * </p>
     */
    public void update(double deltaTime) {}

    /**
     * <h1>销毁对象</h1>
     */
    public void destroy() {}

    //---getter & setter---//
    public String getId() {
        return id;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public boolean isActive() {
        return isActive;
    }

    public void setActive(boolean active) {
        isActive = active;
    }
}
