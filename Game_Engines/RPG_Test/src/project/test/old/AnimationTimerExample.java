package project.test.old;

import engine.base.Point2D;
import engine.derive.old.Circle;
import engine.derive.old.Oval;
import javafx.animation.AnimationTimer;
import javafx.application.Application;
import javafx.scene.Scene;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.layout.StackPane;
import javafx.scene.paint.Color;
import javafx.stage.Stage;

public class AnimationTimerExample extends Application {
//    private static final long INTERVAL = 1_000_000_000 / 120; // 120 FPS
    private long lastTime = 0;
    @Override
    public void start(Stage primaryStage) {
        Canvas canvas = new Canvas(800, 600);
        GraphicsContext gc = canvas.getGraphicsContext2D();

        AnimationTimer timer = new AnimationTimer() {
            double x = 100;
            double v = 0;
            double a = 400;
            double width = 80, height = 50;
            Oval oval = new Oval(new Point2D(x, 300), new Point2D(width, height));
            Circle circle = new Circle(new Point2D(400, 50), 50);
            @Override
            public void handle(long now) {
                if (lastTime == 0) {
                    lastTime = now; // 初始化
                    return;
                }
//                if (now - lastTime < INTERVAL) {
//                    return; // 如果时间间隔不足，跳过这一帧
//                }

                double deltaTime = (now - lastTime) / 1e9;// 将时间差转换为秒
                lastTime = now;

                // 执行绘图操作
                // 清除上一帧
                gc.clearRect(0, 0, canvas.getWidth(), canvas.getHeight());

                // 绘制这一帧
                if (oval.getCenterPointRelative().x < 400) {
                    v += a * deltaTime;
                } else {
                    v -= a * deltaTime;
                }

                oval.setFillColor(Color.rgb(128, 186, 231));
                oval.setStrokeColor(Color.rgb(59, 204, 172));
                oval.setLineWidth(3);
                oval.setOpaque(true);
                oval.Draw(gc);
                oval.move(new Point2D(v * deltaTime, 0));

                circle.setFillColor(Color.rgb(229, 226, 244));
                circle.setStrokeColor(Color.rgb(36, 116, 232));
                circle.setOpaque(true);
                circle.Draw(gc);
            }
        };

        timer.start(); // 启动动画计时器

        primaryStage.setScene(new Scene(new StackPane(canvas)));
        primaryStage.setTitle("Animation Timer Example");
        primaryStage.show();
    }

    public static void main(String[] args) {
        launch(args);
    }
}
