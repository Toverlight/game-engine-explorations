package engine.base;

import java.util.HashMap;
import java.util.Map;

/**
 * <h1>场景状态管理器</h1>
 */
public class GameStateManager {
    private static final Map<String, Map<String, Object>> state = new HashMap<>();

    /**
     * <h1>保存对象状态</h1>
     * @param key 键，即对象persistKey
     * @param data 数据
     */
    public static void save(String key, Map<String, Object> data) {
        state.put(key, data);
    }

    /**
     * <h1>读取对象状态</h1>
     * <p>
     *     根据对象的persistKey加载对象状态。
     * </p>
     * @param key 键，即对象persistKey
     * @return 要加载的对象状态数据
     */
    public static Map<String, Object> load(String key) {
        return state.get(key);
    }

    /**
     * <h1>清除特定状态</h1>
     * @param key 键，即对象persistKey
     */
    public static void remove(String key) {
        state.remove(key);
    }
}
