package xyz.topover.core.view;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.ArrayList;
import java.util.List;
import java.util.logging.Level;

/**
 * <h1>相机管理器</h1>
 * <p>
 *     维持一个相机列表，可添加自定义的摄像机。<br>
 *     <b>注意</b>，在使用相机管理器前，务必先set指定(在这之前先add)主摄像机。否则使用默认摄像机。
 * </p>
 */
public class CameraManager {
    private static final List<Camera> cameras = new ArrayList<>();
    private static Camera mainCamera;
    private static final Logger logger = LoggerFactory.getLogger(CameraManager.class);

    /**
     * <h1>更新所有相机（主要是对其组件起作用）</h1>
     */
    public static void updateAll(double deltaTime) {
        for (Camera cam : cameras) {
            cam.update(deltaTime);
        }
    }

    /**
     * <h1>添加一个摄像机</h1>
     * <p>无名称检查，但<b>最好不要有相同的名称的摄像机。</b></p>
     * @param camera 摄像机
     */
    public static void addCamera(Camera camera) {
        cameras.add(camera);
        logger.info("Camera(id {}) named '{}' was added.", camera.getId(), camera.getName());
    }

    /**
     * <h1>移除一个摄像机</h1>
     * <p>
     *     若移除失败，打印警告。
     * </p>
     * @param camera 摄像机
     */
    public static void removeCamera(Camera camera) {
        if (cameras.remove(camera)) {
            logger.info("Camera(id {}) named '{}' was removed.", camera.getId(), camera.getName());
        } else {
            logger.warn("Camera(id {}) named '{}' wasn't contained by [list]!", camera.getId(),
                    camera.getName());
        }
    }

    /**
     * <h1>指定主摄像机为</h1>
     * <p>
     *     若列表中没找到相应引用，该方法不起作用。
     * </p>
     * @param nameOfMainCamera 主摄像机的名称
     */
    public static void setMainCamera(String nameOfMainCamera) {
        Camera lastCamera = mainCamera;
        for (Camera camera : cameras) {
            if (camera.getName().equals(nameOfMainCamera)) {
                if (lastCamera != null) lastCamera.dropInputComponents();
                mainCamera = camera;
                mainCamera.reuseInputComponents();
                logger.info("MainCamera changed, named '{}'.", nameOfMainCamera);
                return;
            }
        }
        logger.warn("No camera found in [list] and mainCamera ref failed to change, " +
                "maybe name parameter inputted wrongly or target hasn't been added to [list]?");
    }

    public static Camera getMainCamera() {
        if (mainCamera == null) { // 初始化默认摄像机，设置当前主摄像机。
            // 默认摄像机
            Camera defaultCamera = new Camera();
            defaultCamera.setName("Default Camera");
            cameras.add(defaultCamera);
            mainCamera = defaultCamera;
        }
        return mainCamera;
    }

}
