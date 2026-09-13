package xyz.topover.core.paint;

import javafx.animation.AnimationTimer;
import javafx.application.Application;
import javafx.scene.Scene;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.layout.StackPane;
import javafx.scene.paint.Color;
import javafx.stage.Stage;
import xyz.topover.core.calc.Point2D;

public class ParticleSystemTest extends Application {
    private static long lastTime = 0;
    private static double deltaTime;

    @Override
    public void start(Stage primaryStage) throws Exception {
        Canvas canvas = new Canvas(600, 400);
        GraphicsContext gc = canvas.getGraphicsContext2D();

        ParticleSystem particleSystem = new ParticleSystem();
        particleSystem.emit(1000, new Point2D(300, 200));

        AnimationTimer animationTimer = new AnimationTimer() {
            @Override
            public void handle(long now) {
                if (lastTime == 0) {
                    lastTime = now; // 初始化
                    return;
                }

                deltaTime = (now - lastTime) / 1e9;// 将时间差转换为秒
                lastTime = now;

                // 持续发射一些粒子（例如每帧发射2个）
                particleSystem.emit(5, new Point2D(300, 200));

                // 清除上一帧
                gc.clearRect(0, 0, canvas.getWidth(), canvas.getHeight());
                particleSystem.update(deltaTime);
                particleSystem.render(gc);

            }
        };
        animationTimer.start();

        Scene scene = new Scene(new StackPane(canvas));
        scene.setFill(Color.BLACK);
        primaryStage.setScene(scene);
        primaryStage.setTitle("Particle System Test");
        primaryStage.show();
    }

    public static void main(String[] args) {
        launch(args);
    }
}
