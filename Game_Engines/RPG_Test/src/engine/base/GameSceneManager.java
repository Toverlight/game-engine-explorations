package engine.base;

import engine.base.feature.Persistable;

import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * <h1>游戏场景管理器</h1>
 * <p>
 *     维护一个游戏场景列表，可添加自定义的场景。<br>
 * </p>
 */
public class GameSceneManager {
    private static GameScene currentScene;
    private static final Map<String, GameScene> sceneCache = new HashMap<>(); // 场景缓存（key: 场景ID）

    /**
     * <h1>加载场景</h1>
     * <p>
     *     若场景不存在，不会加载。
     * </p>
     * @param sceneName 场景名称
     * @param useCache 是否缓存场景
     */
    public static void loadScene(String sceneName, boolean useCache) {
        GameScene newScene;
        if (useCache && sceneCache.containsKey(sceneName)) {
            newScene = sceneCache.get(sceneName);
        } else {
            newScene = createSceneByName(sceneName);
            if (useCache) sceneCache.put(sceneName, newScene);
        }

        // 保存旧场景中的持久化对象
        if (currentScene != null) {
            List<Persistable> persistableS = currentScene.getPersistableS();
            for (Persistable obj : persistableS) {
                GameStateManager.save(obj.getPersistKey(), obj.serialize());
            }
            currentScene.destroyNonPersistent(); // 仅销毁非持久化对象
        }

        //  恢复新场景中的持久化对象，并进入新场景
        if (newScene != null) {
            newScene.restorePersistableS(newScene.getPersistableS());
            currentScene = newScene;
            currentScene.start();
        } else {
            LogUtils.severe("The target scene '%s' to be loaded is nonexistent!", sceneName);
        }
    }


    public static GameScene getCurrentScene() {
        return currentScene;
    }


    /**
     * <h1>创建场景</h1>
     * <p>
     *     一般是通过类名创建。<br>
     *     若用于特殊需求如<b>可复制的场景</b>，请手动编写或自动生成不同名称以示区别。并在此函数中添加相应的场景创建逻辑。
     * </p>
     * @param sceneName 场景名称。<b>注意，该场景必须在 project.content 下。</b>
     * @return 新场景
     */
    private static GameScene createSceneByName(String sceneName) {
        try {
            // TODO 预处理场景名称：提取类型名称（对于可复制的、需要不同名称以示区别的场景）
            String pakClsName = "project.content." + sceneName;
            Class<?> clazz = Class.forName(pakClsName);

            if (!GameScene.class.isAssignableFrom(clazz)) {
                LogUtils.severe("No scene named '%s'!", sceneName);
                return null;
            }

            Constructor<?> constructor = clazz.getDeclaredConstructor();
            constructor.setAccessible(true);
            GameScene res = (GameScene) constructor.newInstance();
            res.setName(sceneName);
            return res;

        } catch (ClassNotFoundException | NoSuchMethodException e) {
            LogUtils.severe("Class named '%s' is nonexistent!\n" + e, sceneName);
        } catch (IllegalAccessException | InstantiationException | InvocationTargetException e) {
            LogUtils.severe("Scene '%s' cannot be constructed!\n" + e, sceneName);
        }

        return null;
    }

}
