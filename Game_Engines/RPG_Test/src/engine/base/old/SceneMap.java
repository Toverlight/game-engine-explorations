package engine.base.old;

import com.sun.istack.internal.NotNull;
import com.sun.istack.internal.Nullable;
import engine.base.LogUtils;

import java.util.LinkedList;
import java.util.Objects;
import java.util.logging.Level;

/**
 * 场景/地图类
 */
abstract public class SceneMap {
    // 维护局部的对象
    private final LinkedList<Obj> objLinkedList;
    private final LinkedList<Comp> compLinkedList;
    private String name;

    public SceneMap() {
        objLinkedList = new LinkedList<>();
        compLinkedList = new LinkedList<>();
        if (LogUtils.isLoggable(Level.FINE)) {
            LogUtils.fine("SceneMap(%d) CREATED.", this.hashCode());
        }
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    @Override
    public int hashCode() {
        return Objects.hash(objLinkedList, compLinkedList);
    }

    void insert(@NotNull Obj obj) {
        objLinkedList.add(obj);
    }

    void insert(@NotNull Comp comp) {
        compLinkedList.add(comp);
    }

    void delete(@NotNull Obj obj) {
        objLinkedList.remove(obj);
    }

    void delete(@NotNull Comp comp) {
        compLinkedList.remove(comp);
    }

    void ForeachObjUpdate() {
        for (Obj obj : objLinkedList) {
            if (obj.isUpdatable()) {
                obj.Update();
                obj.RecursivelyUpdateObj();
                obj.RecursivelyUpdateComp();
            }
        }
    }

    void ForeachCompUpdate() {
        for (Comp comp : compLinkedList) {
            if (comp.isUpdatable()) {
                comp.Update();
                comp.RecursivelyUpdate();
            }
        }
    }

    @Deprecated
    void ForeachObjDraw() {
        for (Obj obj : objLinkedList) {
            if (obj.isVisible()) {
                obj.Draw();
                obj.RecursivelyDrawObj();
                obj.RecursivelyDrawComp();
            }
        }
    }
    @Deprecated
    void ForeachCompDraw() {
        for (Comp comp : compLinkedList) {
            if (comp.isVisible()) {
                comp.Draw();
                comp.RecursivelyDraw();
            }
        }
    }

    void ForeachListen() {
        for (Comp comp : compLinkedList) {
            if (comp.isListening()) {
                comp.RecursivelyListen();
                comp.Listen();
            }
        }
        for (Obj obj : objLinkedList) {
            obj.RecursivelyListen();
        }
    }

    @Nullable
    Obj findObjLocally(@NotNull String name) {
        Obj resObj = null;
        for (Obj obj : objLinkedList) {
            if (name.equals(obj.getName())) {
                resObj = obj;
                break;
            }
            resObj = obj.findChildObjRecursively(name);
            if (resObj != null) break;
        }
        return resObj;
    }

    @Nullable
    Comp findCompLocally(@NotNull String name) {
        Comp resComp = null;
        boolean findIt = false;
        for (Obj obj : objLinkedList) {
            resComp = obj.findChildCompRecursively(name);
            if (resComp != null) {
                findIt = true;
                break;
            }
        }
        if (!findIt) {
            for (Comp comp : compLinkedList) {
                if (name.equals(comp.getName())) {
                    resComp = comp;
                    break;
                }
                resComp = comp.findChildCompRecursively(name);
                if (resComp != null) break;
            }
        }
        return resComp;
    }

    /**
     * （局部操作）移至底层（按监听顺序）
     */
    void MoveToBottomLocally(Obj obj) {
        if (objLinkedList.remove(obj))
            objLinkedList.addLast(obj);
    }

    /**
     * （局部操作）移至顶层（按监听顺序）
     */
    void MoveToTopLocally(Obj obj) {
        if (objLinkedList.remove(obj))
            objLinkedList.addFirst(obj);
    }

    /**
     * （局部操作）移至底层（按监听顺序）
     */
    void MoveToBottomLocally(Comp comp) {
        if (compLinkedList.remove(comp))
            compLinkedList.addLast(comp);
    }

    /**
     * （局部操作）移至顶层（按监听顺序）
     */
    void MoveToTopLocally(Comp comp) {
        if (compLinkedList.remove(comp))
            compLinkedList.addFirst(comp);
    }
    /**
     * （局部操作）上移一层（按监听顺序）
     */
    void MoveUpLocally(Obj obj) {
        int index = objLinkedList.indexOf(obj);
        if (index != -1) {
            if (index > 0) {
                objLinkedList.remove(obj);
                objLinkedList.add(--index, obj);
            }
        }
    }
    /**
     * （局部操作）下移一层（按监听顺序）
     */
    void MoveDownLocally(Obj obj) {
        int index = objLinkedList.indexOf(obj);
        if (index != -1) {
            if (index < objLinkedList.size() - 1) {
                objLinkedList.remove(obj);
                objLinkedList.add(++index, obj);
            }
        }
    }
    /**
     * （局部操作）上移一层（按监听顺序）
     */
    void MoveUpLocally(Comp comp) {
        int index = compLinkedList.indexOf(comp);
        if (index != -1) {
            if (index > 0) {
                compLinkedList.remove(comp);
                compLinkedList.add(--index, comp);
            }
        }
    }
    /**
     * （局部操作）下移一层（按监听顺序）
     */
    void MoveDownLocally(Comp comp) {
        int index = compLinkedList.indexOf(comp);
        if (index != -1) {
            if (index < compLinkedList.size() - 1) {
                compLinkedList.remove(comp);
                compLinkedList.add(++index, comp);
            }
        }
    }

    /**
     * 当进入时
     */
    public void Enter() {}

    /**
     * 当退出时
     */
    public void Exit() {}

    public void UpdateBefore() {}

    public void UpdateAfter() {}

    public void DrawBefore() {}

    public void DrawAfter() {}

    public void ListenBefore() {}

    public void ListenAfter() {}

}
