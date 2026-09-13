package project.test.detail;

import engine.base.*;
import engine.derive.Oval;
import engine.derive.Polygon;
import engine.derive.SpriteRenderer;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.paint.Color;
import project.test.Testable;

public class CameraTest implements Testable {

    @Override
    public void test(GraphicsContext gc) {
        GameObject player = new GameObject();
        player.setName("Player");
        player.getTransform().setPosition(200, 300);

        Shape playerShape = new Oval(100, 50);
        SpriteRenderer spriteRenderer = player.addComponent(new SpriteRenderer(playerShape));

        Transform transform = new Transform(Transform.I.T);
        transform.rotate(Math.PI / 2);
        transform.translate(300, 200);

        Polygon polygon = new Polygon(new Point2D[]{
                new Point2D(50, 0),
                new Point2D(0, 50),
                new Point2D(-50, 0),
                new Point2D(0, -50)
        });
        polygon.getDrawParams().fillColor = Color.GRAY;
        polygon.setTransform(transform);

        Camera camera = new Camera();
        camera.setPosition(200, 200);
//        camera.setRotation(-Math.PI / 2);
        camera.setZoom(2);
        camera.setName("Camera 1");
        CameraManager.addCamera(camera);
        CameraManager.setMainCamera("Camera 1");

        spriteRenderer.update(0.016);
        polygon.draw(gc);
    }
}
