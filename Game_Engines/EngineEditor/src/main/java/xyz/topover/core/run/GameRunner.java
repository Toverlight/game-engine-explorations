package xyz.topover.core.run;

import javafx.animation.AnimationTimer;
import javafx.application.Application;
import javafx.scene.Scene;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.layout.StackPane;
import javafx.scene.paint.Color;
import javafx.stage.Stage;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import xyz.topover.core.input.InputManager;
import xyz.topover.core.scene.GameScene;
import xyz.topover.core.scene.GameSceneManager;
import xyz.topover.core.view.CameraManager;

/**
 * <h1>游戏运行器</h1>
 * <p>
 *     从该入口类启动。使用run()<br>
 *     <b>注意</b>：1. 如在外部调用（非run方法流程）core中的渲染(get gc之前)，因为core内渲染操作都有用到该类的gc，故请先设置该类的GraphicsContext gc，
 *     以避免渲染时空指针错误。<br>
 *     <b>2. 使用前需用setInitSceneName设置初始GameScene。初始场景自动开启缓存（useCache），之后场景由用户自行决定是否开启缓存。</b>
 * </p>
 */
public class GameRunner extends Application {
    public static final int WINDOW_WIDTH = 800;
    public static final int WINDOW_HEIGHT = 600;
    private static Scene scene;
    private static GraphicsContext gc;
    private static String initSceneName;
    private static long lastTime = 0;
    private static double deltaTime;
    private static String title = "Untitled Game";
//    private static int currentFPS = 0; // 上一秒间平均帧率
//    private static int frameBetweenSeconds = 0; // 帧计数器
    private static final Logger logger = LoggerFactory.getLogger(GameRunner.class);

    /**
     * <h1>获取实际帧间隔</h1>
     */
    public static double getDeltaTime() {
        return deltaTime;
    }

    /**
     * <h1>设置绘图上下文</h1>
     */
    public static void setGc(GraphicsContext gc) {
        GameRunner.gc = gc;
    }

    /**
     * <h1>获取绘图上下文</h1>
     */
    public static GraphicsContext getGc() {
        return gc;
    }

    /**
     * <h1>设置初始场景名称</h1>
     * <p>
     *     根据名称加载初始场景，可手动输入类名，也可用Class.getSimpleName().
     * </p>
     * @param initSceneName 初始场景名称
     */
    public static void setInitSceneName(String initSceneName) {
        GameRunner.initSceneName = initSceneName;
    }

    /**
     * <h1>设置启动时窗口标题</h1>
     * @param title 启动标题
     */
    public static void setTitle(String title) {
        GameRunner.title = title;
    }

    /**
     * <h1>设置画面背景颜色</h1>
     * <p>
     *     默认黑色。
     * </p>
     * @param color 背景颜色
     */
    public static void setBkColor(Color color) {
        scene.setFill(color);
    }

    /**
     * <h1>启动游戏</h1>
     * @param args 命令行参数
     */
    public static void run(String[] args) {
        launch(args);
    }

    @Override
    public void start(Stage primaryStage) throws Exception {
        Canvas canvas = new Canvas(WINDOW_WIDTH, WINDOW_HEIGHT);
        gc = canvas.getGraphicsContext2D();

        if (initSceneName != null)
            GameSceneManager.loadScene(initSceneName, true);
        else {
            logger.error("InitSceneName hasn't been set before! Please set initSceneName before all starts!");
            return;
        }

        AnimationTimer animationTimer = new AnimationTimer() {
            @Override
            public void handle(long now) {
                if (lastTime == 0) {
                    lastTime = now; // 初始化
                    return;
                }

                deltaTime = (now - lastTime) / 1e9;// 将时间差转换为秒
                lastTime = now;

                // 清除上一帧
                gc.clearRect(0, 0, canvas.getWidth(), canvas.getHeight());

                // 场景逻辑
                GameScene curScene = GameSceneManager.getCurrentScene();
                if (curScene != null) {
                    curScene.update(deltaTime); // 更新
                    curScene.renderAll();
                }
                // 相机更新逻辑
                CameraManager.updateAll(deltaTime);

//                ++frameBetweenSeconds;
            }
        };
        animationTimer.start();

        scene = new Scene(new StackPane(canvas));
        scene.setFill(Color.BLACK);
        InputManager.bindListeners(scene);
        primaryStage.setScene(scene);
        primaryStage.setTitle(title);
        primaryStage.show();
    }
}
