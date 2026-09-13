package project.inherent.old;

import engine.base.*;
import engine.base.old.Camera;
import engine.base.old.SceneMap;
import engine.base.old.SceneMapManager;
import engine.derive.old.Circle;
import engine.derive.old.Trapezoid;
import javafx.animation.KeyFrame;
import javafx.animation.Timeline;
import javafx.scene.paint.Color;
import javafx.util.Duration;
import project.test.ones.Entity;

/**
 * 主界面
 */
public class MainSceneMap extends SceneMap {
    private final Entity circleCenter;
    private final Entity trapezoidChild;
    public MainSceneMap() {
        super();

        Point2D centerPointCircle = new Point2D(400, 180);

        Circle circle = new Circle(centerPointCircle, 10);
        circle.setFillColor(Color.rgb(218, 57, 57));
        circle.setStrokeColor(Color.TRANSPARENT);

        circleCenter = new Entity(circle);
        circleCenter.setName("circleCenter");
        circleCenter.setParent(this);
        circleCenter.getTransform2D().setLocationIgnoreShape(circle.getCenterPointRelative());
        circleCenter.addToDrawList();

        Point2D centerPointTrapezoid = new Point2D(100, 0);

        Trapezoid trapezoid = new Trapezoid(centerPointTrapezoid, new Point2D(50, -30),
                new Point2D(50, 80), new Point2D(-20, 50), new Point2D(-60, -20));
        trapezoid.setFillColor(Color.rgb(28, 88, 116));
        trapezoid.setStrokeColor(Color.TRANSPARENT);

        trapezoidChild = new Entity(trapezoid);
        trapezoidChild.setName("trapezoidChild");
        trapezoidChild.setParent(circleCenter);
        trapezoidChild.getTransform2D().setLocationIgnoreShape(centerPointTrapezoid);
        trapezoidChild.getTransform2D().getShape().setCenterSticking(true);
        trapezoidChild.getTransform2D().getShape().setRotationBinding(true);
        trapezoidChild.addToDrawList();

        Camera camera = SceneMapManager.getCamera();
        camera.setTargetWithoutFlash(circleCenter);

    }

    @Override
    public void Enter() {
        LogUtils.info("Enter MainSceneMap.");
    }

    @Override
    public void UpdateBefore() {
        double deltaTime = GameRunner.getDeltaTime();
        circleCenter.getTransform2D().rotateShapeWithoutFlash(Math.PI * deltaTime);
        circleCenter.getTransform2D().moveWithoutFlash(new Point2D(-10.0 * deltaTime, 5.0 * deltaTime));
        Camera camera = SceneMapManager.getCamera();
        camera.zoom(0.05 * deltaTime);
    }

    @Override
    public void Exit() {
        LogUtils.info("Exit MainSceneMap.");
    }

    public void testOutput() {
        // 创建一个 Timeline，每隔 1 秒执行一次任务
//        Point2D[] arr = new Point2D[3];
//        arr[0] = new Point2D();
//        arr[1] = new Point2D();
//        arr[2] = new Point2D();
        Timeline timeline = new Timeline(new KeyFrame(Duration.seconds(1), e -> {
//            arr[0].add(new Point2D(1.0,1.0));
//            arr[1].assign(Point2D.plus(arr[1], new Point2D(1.0,1.0)));
//            arr[2] = Point2D.plus(arr[2], new Point2D(1.0, 1.0));
            System.out.println("UpdateBefore Activate: circleCenter loc: " +
                    circleCenter.getLocationAbsolute() + ", cp:" +
                    circleCenter.getTransform2D().getShape().getCenterPointAbsolute() +
                    ", cp(r)_norm: " +
                    circleCenter.getTransform2D().getShape().getCenterPointRelative().norm() +
                    "; \n\ttrapezoidChild loc: " + trapezoidChild.getLocationAbsolute() +
                    ", cp:" + trapezoidChild.getTransform2D().getShape().getCenterPointAbsolute() +
                    ", cp(r)_norm: " +
                    trapezoidChild.getTransform2D().getShape().getCenterPointRelative().norm() +
                    ", \n\tvertexList:" +
                    trapezoidChild.getTransform2D().getShape().vertexListToString() +
                    "\n\ttest: ");
        }));

        timeline.setCycleCount(Timeline.INDEFINITE); // 无限循环
        timeline.play(); // 开始执行
    }
}
