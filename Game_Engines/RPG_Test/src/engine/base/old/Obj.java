package engine.base.old;

import com.sun.istack.internal.NotNull;
import com.sun.istack.internal.Nullable;
import engine.base.LogUtils;
import engine.base.Point2D;

import java.util.LinkedList;
import java.util.logging.Level;

/**
 * 万物之基——物体类。
 * 搭载物理引擎
 */
abstract public class Obj extends Base{
    private boolean isPhysicOn;// 物理引擎是否开启
    private final LinkedList<Obj> childrenObj;// 物体孩子列表
    private final LinkedList<Comp> childrenComp;// 组件孩子列表
    private Obj parent;// 父物体
    private ParentType parentType;
    /**
     * 物理引擎默认关闭
     */
    public Obj() {
        isPhysicOn = false;
        childrenObj = new LinkedList<>();
        childrenComp = new LinkedList<>();
        parentType = ParentType.None;
        if (LogUtils.isLoggable(Level.FINE)) {
            LogUtils.fine("An Obj(%d) CREATED.", this.hashCode());
        }
    }

    public void PhysicOn() {
        isPhysicOn = true;
    }

    public void PhysicOff() {
        isPhysicOn = false;
    }

    public boolean isPhysicOn() {
        return isPhysicOn;
    }

    /**
     * 绑定到对象。
     * @param parent 父物体或父组件
     */
    public void setParent(@Nullable Obj parent) {
        callToUntieThis();
        this.parent = parent;
        if (this.parent != null) {
            this.parent.tie(this);
            parentType = ParentType.Obj;
            if (LogUtils.isLoggable(Level.FINE)) {
                LogUtils.fine("Obj(%d) with name '%s' was tied to obj parent(%d).", this.hashCode(), getName(),
                        parent.hashCode());
            }
            getTransform2D().getShape().setParent(parent.getTransform2D().getShape());
        } else {
            parentType = ParentType.None;
            if (LogUtils.isLoggable(Level.WARNING)) {
                LogUtils.warning("Obj(%d) with name '%s' set parent->null.This Obj won't be updated,drawn," +
                        "listened and etc.It seems to be no pointing.Is it your expected?", this.hashCode(), getName());
            }
            getTransform2D().getShape().setParent(null);
        }
    }
    /**
     * 绑定到场景/地图。
     * @param sceneMap 父场景/地图
     */
    public void setParent(@NotNull SceneMap sceneMap) {
        callToUntieThis();
        parentType = ParentType.SceneMap;
        sceneMap.insert(this);
        setSceneMap(sceneMap);
        getTransform2D().getShape().setParent(null);
        if (LogUtils.isLoggable(Level.FINE)) {
            LogUtils.fine("Obj(%d) with name '%s' was tied to sceneMap(%d).", this.hashCode(), getName(),
                    sceneMap.hashCode());
        }
    }

    /**
     * 绑定到全局。
     */
    public void setParent() {
        callToUntieThis();
        parentType = ParentType.SceneMapManager;
        SceneMapManager.insert(this);
        getTransform2D().getShape().setParent(null);
        if (LogUtils.isLoggable(Level.FINE)) {
            LogUtils.fine("Obj(%d) with name '%s' was tied to global.", this.hashCode(), getName());
        }
    }

    @Nullable
    public Obj getParent() {
        return parent;
    }

    @NotNull
    public ParentType getParentType() {
        return parentType;
    }



    /**
     * 让绑定到的父对象将自己从列表中删除
     */
    private void callToUntieThis() {
        switch (parentType) {
            case None:
                break;
            case Obj:
                parent.untie(this);
                if (LogUtils.isLoggable(Level.FINER)) {
                    LogUtils.finer("Obj(%d) with name '%s' called to be untied by obj parent(%d).",
                            this.hashCode(), getName(), parent.hashCode());
                }
                parent = null;
                break;
            case SceneMap:
                getSceneMap().delete(this);
                if (LogUtils.isLoggable(Level.FINER)) {
                    LogUtils.finer("Obj(%d) with name '%s' called to be untied by sceneMap(%d).",
                            this.hashCode(), getName(), getSceneMap().hashCode());
                }
                break;
            case SceneMapManager:
                SceneMapManager.delete(this);
                if (LogUtils.isLoggable(Level.FINER)) {
                    LogUtils.finer("Obj(%d) with name '%s' called to be untied by SceneMapManager.",
                            this.hashCode(), getName());
                }
                break;
        }
    }

    /**
     * 解绑子对象
     */
    void untie(Obj obj) {
        childrenObj.remove(obj);
    }

    /**
     * 解绑子组件
     */
    void untie(Comp comp) {
        childrenComp.remove(comp);
    }

    /**
     * 绑定子对象
     */
    void tie(Obj obj) {
        childrenObj.add(obj);
    }

    /**
     * 绑定子组件
     */
    void tie(Comp comp) {
        childrenComp.add(comp);
    }

    /**
     * 获取绝对坐标
     */
    @NotNull
    public Point2D getLocationAbsolute() {
        Obj par = parent;
        Point2D locationAbs = new Point2D();
        locationAbs.add(getLocationRelative());
        while (par != null) {
            locationAbs.add(par.getLocationRelative());
            par = par.getParent();
        }
        return locationAbs;
    }

    void RecursivelyUpdateObj() {
        for (Obj obj : childrenObj) {
            obj.Update();
            obj.RecursivelyUpdateObj();
            obj.RecursivelyUpdateComp();
        }
    }

    void RecursivelyUpdateComp() {
        for (Comp comp : childrenComp) {
            comp.Update();
            comp.RecursivelyUpdate();
        }
    }

    void RecursivelyDrawObj() {
        for (Obj obj : childrenObj) {
            obj.Draw();
            obj.RecursivelyDrawObj();
            obj.RecursivelyDrawComp();
        }
    }

    void RecursivelyDrawComp() {
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
        for (Obj obj : childrenObj) {
            obj.RecursivelyListen();
        }
    }
    @Nullable
    Obj findChildObjRecursively(@NotNull String name) {
        Obj obj = null;
        for (Obj o : childrenObj) {
            if (name.equals(o.getName())) {
                obj = o;
                break;
            }
            obj = o.findChildObjRecursively(name);
            if (obj != null) break;
        }
        return obj;
    }
    @Nullable
    Comp findChildCompRecursively(@NotNull String name) {
        Comp comp = null;
        boolean findIt = false;
        for (Obj o : childrenObj) {
            comp = o.findChildCompRecursively(name);
            if (comp != null) {
                findIt = true;
                break;
            }
        }
        if (!findIt) {
            for (Comp c : childrenComp) {
                if (name.equals(c.getName())) {
                    comp = c;
                    break;
                }
                comp = c.findChildCompRecursively(name);
                if (comp != null) break;
            }
        }
        return comp;
    }


}
