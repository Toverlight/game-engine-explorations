package engine.base.old;

import com.sun.istack.internal.NotNull;
import com.sun.istack.internal.Nullable;
import engine.base.LogUtils;
import engine.base.Point2D;

import java.util.LinkedList;
import java.util.logging.Level;

/**
 * 功能抽象——组件类。
 * 搭载功能与效果
 */
abstract public class Comp extends Base{
    private final LinkedList<Comp> childrenComp;// 组件孩子列表
    private Obj parentObj;// 父物体指针
    private Comp parentComp;// 父组件指针
    private ParentType parentType;
    private boolean isListening;// 是否监听事件

    /**
     * 监听事件默认开启
     */
    public Comp() {
        childrenComp = new LinkedList<>();
        parentType = ParentType.None;
        isListening = true;
        if (LogUtils.isLoggable(Level.FINE)) {
            LogUtils.fine("An Comp(%d) CREATED.", this.hashCode());
        }
    }

    /**
     * 设置是否监听事件
     */
    public void setListening(boolean listening) {
        isListening = listening;
    }

    /**
     * 是否监听事件
     */
    public boolean isListening() {
        return isListening;
    }

    /**
     * 绑定到对象。
     * @param parent 父物体或父组件
     */
    public void setParent(@Nullable Base parent) {
        callToUntieThis();
        if (parent == null) {
            parentType = ParentType.None;
            parentObj = null;
            parentComp = null;
            getTransform2D().getShape().setParent(null);
            if (LogUtils.isLoggable(Level.WARNING)) {
                LogUtils.warning("Comp(%d) with name '%s' set parent->null.This Comp won't be updated,drawn," +
                        "listened and etc.It seems to be no pointing.Is it your expected?", this.hashCode(), getName());
            }
        } else {
            if (parent instanceof Obj) {
                parentType = ParentType.Obj;
                parentObj = (Obj)parent;
                parentObj.tie(this);
                parentComp = null;
                if (LogUtils.isLoggable(Level.FINE)) {
                    LogUtils.fine("Comp(%d) with name '%s' was tied to obj parent(%d).", this.hashCode(),
                            getName(), parentObj.hashCode());
                }
            } else if (parent instanceof Comp) {
                parentType = ParentType.Comp;
                parentComp = (Comp)parent;
                parentComp.tie(this);
                parentObj = null;
                if (LogUtils.isLoggable(Level.FINE)) {
                    LogUtils.fine("Comp(%d) with name '%s' was tied to comp parent(%d).", this.hashCode(),
                            getName(), parentComp.hashCode());
                }
            }
            getTransform2D().getShape().setParent(parent.getTransform2D().getShape());
        }
    }

    /**
     * 绑定到场景/地图顶层
     */
    public void setParent(@NotNull SceneMap sceneMap) {
        callToUntieThis();
        parentType = ParentType.SceneMap;
        sceneMap.insert(this);
        setSceneMap(sceneMap);
        getTransform2D().getShape().setParent(null);
        if (LogUtils.isLoggable(Level.FINE)) {
            LogUtils.fine("Comp(%d) with name '%s' was tied to sceneMap(%d).", this.hashCode(), getName(),
                    sceneMap.hashCode());
        }
    }

    /**
     * 绑定到全局顶层
     */
    public void setParent() {
        callToUntieThis();
        parentType = ParentType.SceneMapManager;
        SceneMapManager.insert(this);
        getTransform2D().getShape().setParent(null);
        if (LogUtils.isLoggable(Level.FINE)) {
            LogUtils.fine("Comp(%d) with name '%s' was tied to global.", this.hashCode(), getName());
        }
    }

    /**
     * 让绑定到的父对象将自己从列表中删除
     */
    private void callToUntieThis() {
        switch (parentType) {
            case None:
                break;
            case Obj:
                parentObj.untie(this);
                if (LogUtils.isLoggable(Level.FINER)) {
                    LogUtils.finer("Comp(%d) with name '%s' called to be untied by obj parent(%d).",
                            this.hashCode(), getName(), parentObj.hashCode());
                }
                parentObj = null;
                break;
            case Comp:
                parentComp.untie(this);
                if (LogUtils.isLoggable(Level.FINER)) {
                    LogUtils.finer("Comp(%d) with name '%s' called to be untied by comp parent(%d).",
                            this.hashCode(), getName(), parentComp.hashCode());
                }
                parentComp = null;
                break;
            case SceneMap:
                getSceneMap().delete(this);
                if (LogUtils.isLoggable(Level.FINER)) {
                    LogUtils.finer("Comp(%d) with name '%s' called to be untied by sceneMap(%d).",
                            this.hashCode(), getName(), getSceneMap().hashCode());
                }
                break;
            case SceneMapManager:
                SceneMapManager.delete(this);
                if (LogUtils.isLoggable(Level.FINER)) {
                    LogUtils.finer("Comp(%d) with name '%s' called to be untied by SceneMapManager.",
                            this.hashCode(), getName());
                }
                break;
        }
    }

    /**
     * 获取父实例标识
     */
    @NotNull
    public ParentType getParentType() {
        return parentType;
    }

    /**
     * 获取父实例
     */
    @Nullable
    public Base getParent() {
        Base base = null;
        if (parentType != ParentType.None) {
            if (parentType == ParentType.Obj) {
                base = parentObj;
            } else if (parentType == ParentType.Comp) {
                base = parentComp;
            }
        }
        return base;
    }

    /**
     * 解绑子组件
     */
    private void untie(Comp comp) {
        childrenComp.remove(comp);
    }

    /**
     * 绑定子组件
     */
    private void tie(Comp comp) {
        childrenComp.add(comp);
    }

    /**
     * 获取绝对坐标
     */
    @NotNull
    public Point2D getLocationAbsolute() {
        Point2D locationAbs = new Point2D();
        locationAbs.add(getLocationRelative());
        ParentType parentType = this.parentType;
        Obj parentObj;
        Comp parentComp;
        while (parentType != ParentType.None) {
            if (parentType == ParentType.Obj) {
                parentObj = (Obj)getParent();
                locationAbs.add(parentObj.getLocationAbsolute());
                break;
            } else if (parentType == ParentType.Comp) {
                parentComp = (Comp)getParent();
                locationAbs.add(parentComp.getLocationRelative());
                parentType = parentComp.getParentType();
            }
        }
        return locationAbs;
    }

    public void Listen() {}

    void RecursivelyUpdate() {
        for (Comp comp : childrenComp) {
            comp.Update();
            comp.RecursivelyUpdate();
        }
    }

    void RecursivelyDraw() {
        for (Comp comp : childrenComp) {
            comp.Draw();
            comp.RecursivelyDraw();
        }
    }

    void RecursivelyListen() {
        for (Comp comp : childrenComp) {
            comp.RecursivelyListen();
            comp.Listen();
        }
    }

    @Nullable
    Comp findChildCompRecursively(@NotNull String name) {
        Comp comp = null;
        for (Comp c : childrenComp) {
            if (name.equals(c.getName())) {
                comp = c;
                break;
            }
            comp = c.findChildCompRecursively(name);
            if (comp != null) break;
        }
        return comp;
    }
}
