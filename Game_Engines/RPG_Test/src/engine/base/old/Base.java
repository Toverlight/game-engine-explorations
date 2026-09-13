package engine.base.old;

import engine.base.LogUtils;
import engine.base.Point2D;
import project.inherent.old.GameRunner;

import java.util.Objects;
import java.util.logging.Level;

/**
 * 顶层基类。
 * 具有基本变换功能
 */
abstract public class Base {
    private final Transform2D transform2D;
    private String name;// 名称标识
    private SceneMap sceneMap;// 隶属的场景/地图（如果是局部顶层对象）
    private boolean isVisible;// 是否可见
    private boolean isUpdatable;// 是否更新
    private boolean isInDrawList;// 是否在绘制列表中
    private boolean hasWarned; // 是否触发过警告

    public Base() {
        transform2D = new Transform2D();
        isUpdatable = true;
        isVisible = true;
        isInDrawList = false;
        if (LogUtils.isLoggable(Level.FINER)) {
            LogUtils.finer("A Base(%d) CREATED.", this.hashCode());
        }
    }

    @Override
    public int hashCode() {
        return Objects.hash(transform2D);
    }

    public final Transform2D getTransform2D() {
        return transform2D;
    }

    public void setName(String name) {
        this.name = name;
    }

    public String getName() {
        return name;
    }

    public void setVisible(boolean visible) {
        isVisible = visible;
    }

    public boolean isVisible() {
        return isVisible;
    }

    public void setUpdatable(boolean updatable) {
        isUpdatable = updatable;
    }

    public boolean isUpdatable() {
        return isUpdatable;
    }

    protected void setSceneMap(SceneMap sceneMap) {
        this.sceneMap = sceneMap;
    }

    protected SceneMap getSceneMap() {
        return sceneMap;
    }

    /**
     * 获取相对坐标
     */
    public Point2D getLocationRelative() {
        return transform2D.getLocation();
    }

    /**
     * 添加到绘制列表
     * <p>等价于addToDrawListLast()</p>
     */
    public final void addToDrawList() {
        addToDrawListLast();
    }

    /**
     * 添加到绘制列表末尾
     */
    public final void addToDrawListLast() {
        if (!isInDrawList) {
            SceneMapManager.toDrawLast(this);
            isInDrawList = true;
            if (LogUtils.isLoggable(Level.FINE)) {
                LogUtils.fine("Base(%d) with name '%s' is added to the last of drawList.", this.hashCode(),
                        name);
            }
        }
    }

    /**
     * 添加到绘制列表头部
     */
    public final void addToDrawListFirst() {
        if (!isInDrawList) {
            SceneMapManager.toDrawFirst(this);
            isInDrawList = true;
            if (LogUtils.isLoggable(Level.FINE)) {
                LogUtils.fine("Base(%d) with name '%s' is added to the first of drawList.", this.hashCode(),
                        name);
            }
        }
    }

    /**
     * 从绘制列表中删除
     */
    public final void dropFromDrawList() {
        if (isInDrawList) {
            SceneMapManager.dropFromDrawList(this);
            isInDrawList = false;
            if (LogUtils.isLoggable(Level.FINE)) {
                LogUtils.fine("Base(%d) with name '%s' is dropped from drawList.", this.hashCode(), name);
            }
        }
    }

    /**
     * 默认绘制图形
     */
    public void Draw() {
        Shape2D shape2D = transform2D.getShape();
        if (shape2D != null) {
            transform2D.getShape().Draw(GameRunner.getGc());
        } else {
            if (!hasWarned) {
                LogUtils.warning("Base(%d) with name '%s' hasn't set any shape yet!Every base should set one " +
                        "by rule,before the base being drawn.", this.hashCode(), name);
                hasWarned = true;
            }
        }
    }
    public void Update() {}
}
