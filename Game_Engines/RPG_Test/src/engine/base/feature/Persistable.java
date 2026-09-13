package engine.base.feature;

import java.util.Map;

/**
 * <h1>持久化</h1>
 */
public interface Persistable {
    /**
     * <h1>序列化</h1>
     */
    Map<String, Object> serialize();

    /**
     * <h1>反序列化</h1>
     */
    void deserialize(Map<String, Object> data);

    /**
     * <h1>获取持久化键</h1>
     */
    String getPersistKey();
}
