package xyz.topover.core.scene;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.HashMap;
import java.util.Map;

/**
 * <h1>场景状态管理器</h1>
 */
public class GameStateManager {
    private static final Map<String, Map<String, Object>> state = new HashMap<>();
    private static final Logger logger = LoggerFactory.getLogger(GameStateManager.class);

    /**
     * <h1>保存对象状态</h1>
     * @param key 键，即对象persistKey
     * @param data 数据
     */
    public static void save(String key, Map<String, Object> data) {
        state.put(key, data);
        logger.info("Saved: <key:{}, data map hashCode:{}>.", key, data.hashCode());
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
