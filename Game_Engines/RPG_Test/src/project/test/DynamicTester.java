package project.test;

import engine.base.CameraManager;
import engine.base.GameScene;
import engine.base.GameSceneManager;
import engine.base.InputManager;
import javafx.animation.Animation;
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
import project.inherent.GameRunner;
import project.test.detail.CameraTest;

/**
 * <h1>动态测试</h1>
 * <p>
 *     以某一帧率动态绘制测试内容。
 * </p>
 */
public class DynamicTester extends Application {
    private static long lastTime = 0;
    private static double deltaTime;

    private static int currentFPS = 0; // 上一秒间平均帧率
    private static int frameBetweenSeconds = 0; // 帧计数器

    @Override
    public void start(Stage primaryStage) throws Exception {
        // 创建一个800x600的画布
        Canvas canvas = new Canvas(GameRunner.WINDOW_WIDTH, GameRunner.WINDOW_HEIGHT);

        // 设置背景颜色为深蓝色
        GraphicsContext gc = canvas.getGraphicsContext2D();
        GameRunner.setGc(gc); // 设置gc，防止空指针错误

        // --- 在这里修改要加载的场景类型 sceneName --- //
        GameSceneManager.loadScene("WelcomeScene", false);

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

                // 场景更新逻辑
                GameScene curScene = GameSceneManager.getCurrentScene();
                if (curScene != null) curScene.update(deltaTime);
                // 相机更新逻辑
                CameraManager.updateAll(deltaTime);

                ++frameBetweenSeconds;
            }
        };
        animationTimer.start();

        Timeline timeline = new Timeline(
                new KeyFrame(Duration.seconds(5), event -> {
                    currentFPS = frameBetweenSeconds / 5;
                    frameBetweenSeconds = 0;
                    System.out.println("currentFPS: " + currentFPS);
                })
        );
        timeline.setCycleCount(Animation.INDEFINITE);
        timeline.play();

        // 创建场景并显示画布
        Scene scene = new Scene(new StackPane(canvas));
        InputManager.bindListeners(scene);
        primaryStage.setTitle("Dynamic Test");
        primaryStage.setScene(scene);
        primaryStage.show();
    }

    public static void main(String[] args) {
        launch(args);
    }

}
