package engine.base.old;

/**
 * 双亲类型
 */
public enum ParentType {
    /**
     * 无
     */
    None,
    /**
     * 物体
     */
    Obj,
    /**
     * 组件
     */
    Comp,
    /**
     * 场景/地图（局部顶层）
     */
    SceneMap,
    /**
     * 场景/地图管理器（全局顶层）
     */
    SceneMapManager
}
