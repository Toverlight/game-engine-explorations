package project.test;

import engine.base.*;
import engine.derive.Oval;
import engine.derive.Polygon;
import engine.derive.SpriteRenderer;
import javafx.application.Application;
import javafx.scene.Scene;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.layout.StackPane;
import javafx.scene.paint.Color;
import javafx.stage.Stage;
import project.inherent.GameRunner;
import project.test.detail.CameraTest;

/**
 * <h1>静态测试</h1>
 * <p>
 *     仅测试一帧内容。
 * </p>
 */
public class StaticTester extends Application {
    @Override
    public void start(Stage primaryStage) {
        // 创建一个800x600的画布
        Canvas canvas = new Canvas(GameRunner.WINDOW_WIDTH, GameRunner.WINDOW_HEIGHT);

        // 设置背景颜色为深蓝色
        GraphicsContext gc = canvas.getGraphicsContext2D();
        GameRunner.setGc(gc); // 设置gc，防止空指针错误

        // --- 修改下面这行的实例化对象类型，以实现具体的静态测试 --- //
        Testable testable = new CameraTest();
        // --- --- //
        testable.test(gc);

        // 创建场景并显示画布
        Scene scene = new Scene(new StackPane(canvas));
        primaryStage.setTitle("Static Test");
        primaryStage.setScene(scene);
        primaryStage.show();
    }

    public static void main(String[] args) {
        launch(args);
    }
}
