package engine.base.old;

import com.sun.istack.internal.NotNull;
import com.sun.istack.internal.Nullable;
import project.inherent.old.GameRunner;

import java.util.LinkedList;

/**
 * 场景/地图类管理器
 */
public final class SceneMapManager {
    private static SceneMap sceneMap; // 当前场景/地图
    private static final Camera camera; // 全局唯一相机：每个场景/地图Enter()中要对其初始化
    // 维护全局的对象
    private static final LinkedList<Obj> objLinkedList;
    private static final LinkedList<Comp> compLinkedList;
    // 总绘制链表
    private static final LinkedList<Base> drawLinkedList;

    static {
        camera = new Camera(GameRunner.WINDOW_WIDTH, GameRunner.WINDOW_HEIGHT);
        objLinkedList = new LinkedList<>();
        compLinkedList = new LinkedList<>();
        drawLinkedList = new LinkedList<>();
        camera.setBaseFlashingList(drawLinkedList);
    }

    private SceneMapManager() {}

    /**
     * 获取相机
     */
    public static Camera getCamera() {
        return camera;
    }

    public static void setSceneMap(SceneMap sceneMap) {
        if (SceneMapManager.sceneMap != null) SceneMapManager.sceneMap.Exit();
        SceneMapManager.sceneMap = sceneMap;
        if (SceneMapManager.sceneMap != null) SceneMapManager.sceneMap.Enter();
    }

    static void insert(@NotNull Obj obj) {
        objLinkedList.add(obj);
    }

    static void insert(@NotNull Comp comp) {
        compLinkedList.add(comp);
    }

    static void delete(@NotNull Obj obj) {
        objLinkedList.remove(obj);
    }

    static void delete(@NotNull Comp comp) {
        compLinkedList.remove(comp);
    }

    @Nullable
    static Obj findObjLocally(@NotNull String name) {
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
    static Comp findCompLocally(@NotNull String name) {
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
     * 全局查找物体
     * @param name 物体名称标识
     */
    @Nullable
    public static Obj findObjGlobally(@NotNull String name) {
        Obj obj = findObjLocally(name);
        if (obj == null) {
            if (sceneMap != null) {
                obj = sceneMap.findObjLocally(name);
            }
        }
        return obj;
    }

    /**
     * 全局查找组件
     * @param name 组件名称标识
     */
    @Nullable
    public static Comp findCompGlobally(@NotNull String name) {
        Comp comp = findCompLocally(name);
        if (comp == null) {
            if (sceneMap != null) {
                comp = sceneMap.findCompLocally(name);
            }
        }
        return comp;
    }

    /**
     * 添加到绘制链表末尾，最后绘制
     */
    static void toDrawLast(Base base) {
        drawLinkedList.addLast(base);
        camera.flashBaseShowing(base);
    }

    /**
     * 添加到绘制链表头部，最先绘制
     */
    static void toDrawFirst(Base base) {
        drawLinkedList.addFirst(base);
        camera.flashBaseShowing(base);
    }

    /**
     * 将指定对象从绘制链表中删除
     */
    static void dropFromDrawList(Base base) {
        drawLinkedList.remove(base);
    }

    /**
     * 清空绘制列表
     */
    public static void clearDrawList() {
        drawLinkedList.clear();
    }

    /*
    遍历：全局 > 局部
     */

    public static void ForeachObjUpdate() {
        for (Obj obj : objLinkedList) {
            if (obj.isUpdatable()) {
                obj.Update();
                obj.RecursivelyUpdateObj();
                obj.RecursivelyUpdateComp();
            }
        }
        if (sceneMap != null) sceneMap.ForeachObjUpdate();

    }

    public static void ForeachCompUpdate() {
        for (Comp comp : compLinkedList) {
            if (comp.isUpdatable()) {
                comp.Update();
                comp.RecursivelyUpdate();
            }
        }
        if (sceneMap != null) sceneMap.ForeachCompUpdate();
    }

    @Deprecated
    public static void ForeachObjDraw() {
        for (Obj obj : objLinkedList) {
            if (obj.isVisible()) {
                obj.Draw();
                obj.RecursivelyDrawObj();
                obj.RecursivelyDrawComp();
            }
        }
        if (sceneMap != null) sceneMap.ForeachObjDraw();
    }
    @Deprecated
    public static void ForeachCompDraw() {
        for (Comp comp : compLinkedList) {
            if (comp.isVisible()) {
                comp.Draw();
                comp.RecursivelyDraw();
            }
        }
        if (sceneMap != null) sceneMap.ForeachCompDraw();
    }

    public static void ForeachListen() {
        if (sceneMap != null) sceneMap.ListenBefore();
        for (Comp comp : compLinkedList) {
            if (comp.isListening()) {
                comp.RecursivelyListen();
                comp.Listen();
            }
        }
        for (Obj obj : objLinkedList) {
            obj.RecursivelyListen();
        }
        if (sceneMap != null) {
            sceneMap.ForeachListen();
            sceneMap.ListenAfter();
        }
    }

    /**
     * （全局操作）移至底层（按监听顺序）
     */
    public static void MoveToBottomGlobally(Obj obj) {
        if (objLinkedList.remove(obj))
            objLinkedList.addLast(obj);
        else if (sceneMap != null)
            sceneMap.MoveToTopLocally(obj);
    }

    /**
     * （全局操作）移至顶层（按监听顺序）
     */
    public static void MoveToTopGlobally(Obj obj) {
        if (objLinkedList.remove(obj))
            objLinkedList.addFirst(obj);
        else if (sceneMap != null) {
            sceneMap.MoveToBottomLocally(obj);
        }
    }

    /**
     * （全局操作）移至底层（按监听顺序）
     */
    public static void MoveToBottomGlobally(Comp comp) {
        if (compLinkedList.remove(comp))
            compLinkedList.addLast(comp);
        else if (sceneMap != null) {
            sceneMap.MoveToTopLocally(comp);
        }
    }

    /**
     * （全局操作）移至顶层（按监听顺序）
     */
    public static void MoveToTopGlobally(Comp comp) {
        if (compLinkedList.remove(comp))
            compLinkedList.addFirst(comp);
        else if (sceneMap != null) {
            sceneMap.MoveToBottomLocally(comp);
        }
    }

    /**
     * （全局操作）上移一层（按监听顺序）
     */
    public static void MoveUpGlobally(Obj obj) {
        int index = objLinkedList.indexOf(obj);
        if (index != -1) {
            if (index > 0) {
                objLinkedList.remove(obj);
                objLinkedList.add(--index, obj);
            }
        } else if (sceneMap != null) {
            sceneMap.MoveDownLocally(obj);
        }
    }
    /**
     * （全局操作）下移一层（按监听顺序）
     */
    public static void MoveDownGlobally(Obj obj) {
        int index = objLinkedList.indexOf(obj);
        if (index != -1) {
            if (index < objLinkedList.size() - 1) {
                objLinkedList.remove(obj);
                objLinkedList.add(++index, obj);
            }
        } else if (sceneMap != null) {
            sceneMap.MoveUpLocally(obj);
        }
    }
    /**
     * （全局操作）上移一层（按监听顺序）
     */
    public static void MoveUpGlobally(Comp comp) {
        int index = compLinkedList.indexOf(comp);
        if (index != -1) {
            if (index > 0) {
                compLinkedList.remove(comp);
                compLinkedList.add(--index, comp);
            }
        } else if (sceneMap != null) {
            sceneMap.MoveDownLocally(comp);
        }
    }
    /**
     * （全局操作）下移一层（按监听顺序）
     */
    public static void MoveDownGlobally(Comp comp) {
        int index = compLinkedList.indexOf(comp);
        if (index != -1) {
            if (index < compLinkedList.size() - 1) {
                compLinkedList.remove(comp);
                compLinkedList.add(++index, comp);
            }
        } else if (sceneMap != null) {
            sceneMap.MoveUpLocally(comp);
        }
    }

    /**
     * 一切对象的绘制操作
     */
    public static void DrawBases() {
        if (sceneMap != null) sceneMap.DrawBefore();
        for (Base base : drawLinkedList) {
            base.Draw();
        }
        if (sceneMap != null) sceneMap.DrawAfter();
    }

    public static void UpdateBefore() {
        if (sceneMap != null) sceneMap.UpdateBefore();
    }
    public static void UpdateAfter() {
        if (sceneMap != null) sceneMap.UpdateAfter();
    }

}
