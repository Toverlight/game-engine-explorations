package project.inherent.old;

import engine.base.old.SceneMapManager;
import javafx.animation.AnimationTimer;
import javafx.animation.KeyFrame;
import javafx.animation.Timeline;
import javafx.application.Application;
import javafx.scene.Scene;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.layout.StackPane;
import javafx.stage.Stage;
import javafx.util.Duration;

import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.logging.Handler;
import java.util.logging.Level;
import java.util.logging.LogManager;
import java.util.logging.Logger;

/**
 * <h1>游戏运行器（旧）</h1>
 * <p>从该入口类启动。<br>
 *    <b>注意</b>：如用测试类渲染(get gc之前)，请先设置该类的GraphicsContext gc，以避免渲染时空指针错误。</p>
 */
@Deprecated
public final class GameRunner extends Application {
    private static long lastTime = 0;
    private static double deltaTime;
    private static MainSceneMap mainSceneMap;
    private static GraphicsContext gc;
    public static final int WINDOW_WIDTH = 800; // 窗口宽度
    public static final int WINDOW_HEIGHT = 600; // 窗口高度
    private static int currentFPS = 0; // 上一秒间平均帧率
    private static int frameBetweenSeconds = 0; // 帧计数器
    private static Logger logger; // 全局唯一日志类
    private static boolean isLoggerInitialized; // 日志是否已被初始化
    private static boolean isDebugMode; // 是否正在调试模式下

    static {
        isDebugMode = false;
        isLoggerInitialized = false;
        initLogger();
        isLoggerInitialized = true;
    }

    /**
     * 初始化主界面
     */
    public static void initMainSceneMap() {
        mainSceneMap = new MainSceneMap();
    }

    /**
     * 初始化日志
     */
    public static void initLogger() {
        if (!isLoggerInitialized) {
            try {
                InputStream input = new FileInputStream("config/logging.properties");
                //取得日志管理器对象
                LogManager logManager = LogManager.getLogManager();
                //读取自定义的配置文件
                logManager.readConfiguration(input);
            } catch (IOException e) {
                System.out.println("old:Failed to read 'logging.properties'：\n" + e);
            }
            logger = Logger.getLogger(GameRunner.class.getCanonicalName());
            logger.setLevel(Level.FINER); // 在这里修改最开始的level，后面不要将其赋值为null以免报错
            Handler[] handlers = Logger.getLogger("").getHandlers();
            for (Handler handler : handlers) {
                System.out.println("Old Handler: " + handler.getClass().getName());
            }
            logger.info("Old logger initialized.");
            isLoggerInitialized = true;
        }
    }

    /**
     * 获取实际帧间隔
     */
    public static double getDeltaTime() {
        return deltaTime;
    }

    /**
     * 获取上一秒间平均帧率
     */
    public static int getCurrentFPS() {
        return currentFPS;
    }

    /**
     * 获取日志
     */
    public static Logger getLogger() {
        return logger;
    }

    public static void setGc(GraphicsContext gc) {
        GameRunner.gc = gc;
    }
    public static GraphicsContext getGc() {
        return gc;
    }

    /**
     * 开启调试
     */
    public static void DebugON() {
        isDebugMode = true;
    }

    /**
     * 关闭调试
     */
    public static void DebugOFF() {
        isDebugMode = false;
    }

    @Override
    public void start(Stage primaryStage) {
        Canvas canvas = new Canvas(WINDOW_WIDTH, WINDOW_HEIGHT);
        gc = canvas.getGraphicsContext2D();

        SceneMapManager.setSceneMap(mainSceneMap);

        AnimationTimer animationTimer = new AnimationTimer() {
            @Override
            public void handle(long now) {
                if (lastTime == 0) {
                    lastTime = now; // 初始化
                    return;
                }

                deltaTime = (now - lastTime) / 1e9;// 将时间差转换为秒
                lastTime = now;

                // 执行绘图操作
                // 清除上一帧
                gc.clearRect(0, 0, canvas.getWidth(), canvas.getHeight());

                // 绘制这一帧
                SceneMapManager.DrawBases();

                // 输入响应
                SceneMapManager.ForeachListen();

                // 更新这一帧
                SceneMapManager.UpdateBefore();
                SceneMapManager.ForeachObjUpdate();
                SceneMapManager.ForeachCompUpdate();
                SceneMapManager.UpdateAfter();

                ++frameBetweenSeconds;
            }
        };

        animationTimer.start();

        AtomicInteger i = new AtomicInteger();
        i.set(5);
        Timeline timeline = new Timeline(
                new KeyFrame(Duration.millis(1000), event -> {
                    currentFPS = frameBetweenSeconds;
                    frameBetweenSeconds = 0;
                    logger.fine("(Old) currentFPS: " + currentFPS);
                    if (i.get() >= 5) {
                        if (Level.INFO.equals(logger.getLevel())) {
                            logger.info("(Old) currentFPS: " + currentFPS);
                        }
                        i.set(0);
                    }
                    i.incrementAndGet();
                })
        );
        timeline.setCycleCount(Timeline.INDEFINITE);
        timeline.play();

        primaryStage.setScene(new Scene(new StackPane(canvas)));
        primaryStage.setTitle("RPG(old)");
        primaryStage.show();

    }

    public static void main(String[] args) {
        // TODO 读取其他配置文件
        initMainSceneMap();
        // TODO 从数据库加载

        launch(args);// 启动游戏
    }

}
