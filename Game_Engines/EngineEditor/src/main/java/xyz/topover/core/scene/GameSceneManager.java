package xyz.topover.core.scene;

import org.reflections.Reflections;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

/**
 * <h1>游戏场景管理器</h1>
 * <p>
 *     维护一个游戏场景列表，可添加自定义的场景。<br>
 * </p>
 */
public class GameSceneManager {
    private static GameScene currentScene;
    private static final Map<String, GameScene> sceneCache = new HashMap<>(); // 场景缓存（key: 场景ID）
    private static String pakName = ""; // 用户存放场景的包名
    private static final Logger logger = LoggerFactory.getLogger(GameSceneManager.class);

    /**
     * <h1>设置统一存放场景的包名</h1>
     * <p>
     *     之后所有的场景，根据名称（由getName()取得），从设置的包的所有子包内查找场景类并尝试加载。<br>
     *     也就是说，<b>设置的场景包名就是存放所有场景的根包名。</b><br>
     *     默认为空，即根为项目源代码根，从所有包中搜寻场景类。
     * </p>
     * @param pakName 场景根包名
     */
    public static void setPakName(String pakName) {
        GameSceneManager.pakName = pakName;
    }

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
            logger.info("Found {}(id {}) from cache.", newScene.getClass().getSimpleName(), newScene.getId());
        } else {
            newScene = createSceneByName(sceneName);
            if (useCache) {
                if (newScene != null) {
                    sceneCache.put(sceneName, newScene);
                    logger.info("Put {}(id {}) into cache.", newScene.getClass().getSimpleName(), newScene.getId());
                }
            }
        }

        // 保存旧场景中的持久化对象
        if (currentScene != null) {
            List<Persistable> persistableS = currentScene.getPersistableS();
            if (!persistableS.isEmpty()) {
                for (Persistable obj : persistableS) {
                    GameStateManager.save(obj.getPersistKey(), obj.serialize());
                }
                logger.info("Restored all of the persistable-s of the last {}(id {}).",
                        currentScene.getClass().getSimpleName(), currentScene.getId());
            }
            else {
                logger.info("Nothing to be restored in the last {}(id {}).", currentScene.getClass().getSimpleName(),
                        currentScene.getId());
            }
            currentScene.destroyNonPersistent(); // 仅销毁非持久化对象
            logger.info("Non persistable-s of the last {}(id {}) has been destroyed.",
                    currentScene.getClass().getSimpleName(), currentScene.getId());
        }

        //  恢复新场景中的持久化对象，并进入新场景
        if (newScene != null) {
            logger.info("This is the first scene - {}.", newScene.getClass().getSimpleName());
            newScene.restorePersistableS(newScene.getPersistableS());
            currentScene = newScene;
            currentScene.start();
            logger.info("Entered next scene {}(id {}).", newScene.getClass().getSimpleName(), newScene.getId());
        } else {
            logger.error("The target scene '{}' to be loaded is non existent!", sceneName);
        }
    }

    /**
     * <h1>获得当前场景</h1>
     */
    public static GameScene getCurrentScene() {
        return currentScene;
    }


    /**
     * <h1>创建场景</h1>
     * <p>
     *     一般是通过类名创建。<br>
     *     若用于特殊需求如<b>可复制的场景</b>，请手动编写或自动生成不同名称以示区别。并在此函数中添加相应的场景创建逻辑。
     * </p>
     * @param sceneName 场景名称。<b>注意，该场景必须在 所设置的pakName（setPakName方法） 下。</b>
     * @return 新场景。若创建失败，则返回null.
     */
    private static GameScene createSceneByName(String sceneName) {
        try {
            Reflections reflections = new Reflections(pakName);
            Set<Class<? extends GameScene>> sceneClasses = reflections.getSubTypesOf(GameScene.class);

            for (Class<? extends GameScene> clazz : sceneClasses) {
                if (clazz.getSimpleName().equals(sceneName)) { // 仅匹配类名
                    Constructor<? extends GameScene> constructor = clazz.getDeclaredConstructor();
                    constructor.setAccessible(true);
                    GameScene res = constructor.newInstance();
                    res.setName(sceneName);
                    logger.info("GameScene(id {}) typed '{}' created successfully.", res.getId(),
                            res.getClass().getSimpleName());
                    return res;
                }
            }

            logger.error("No scene named '{}' found in package '{}' or its subpackages!", sceneName, pakName);
        } catch (NoSuchMethodException | IllegalAccessException | InstantiationException | InvocationTargetException e) {
            logger.error("Scene '{}' cannot be constructed!\n{}", sceneName, e);
        } catch (Exception e) {
            logger.error("Unexpected error when creating scene '{}':\n{}", sceneName, e);
        }

        return null;
    }

}
