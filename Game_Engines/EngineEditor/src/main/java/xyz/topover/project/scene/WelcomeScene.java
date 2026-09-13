package xyz.topover.project.scene;

import javafx.scene.paint.Color;
import xyz.topover.core.base.GameObject;
import xyz.topover.core.calc.Point2D;
import xyz.topover.core.entity.PositionOrbiting;
import xyz.topover.core.input.InputManager;
import xyz.topover.core.paint.*;
import xyz.topover.core.scene.GameScene;
import xyz.topover.core.view.*;
import xyz.topover.project.entity.Player;

public class WelcomeScene extends GameScene {
    private GameObject triangleEntity;
    private GameObject rectEntity;
    private PositionOrbiting positionOrbiting;
    @Override
    protected void loadResources() {

    }

    @Override
    protected void initializeScene() {
        triangleEntity = new GameObject();
        triangleEntity.addComponent(new SpriteRenderer(new Polygon(
                new Point2D[] {
                        new Point2D(0, 20), new Point2D(30, 0), new Point2D(-30, 0)
                }
        ){{
            drawParams = DrawParams.YELLOW_FILL;
        }}));
        triangleEntity.setName("triangleEntity");
        triangleEntity.getTransform().setPosition(-300, 150);
        addGameObject(triangleEntity);

        rectEntity = new GameObject();
        rectEntity.addComponent(new SpriteRenderer(new Polygon(
                new Point2D[] {
                        new Point2D(20, 20), new Point2D(-20, 20),
                        new Point2D(-20, -20), new Point2D(20, -20)
                }
        ){{
            drawParams = DrawParams.GREEN_FILL;
        }}));
//        rectEntity.addComponent(new LightSource(50, 300, 0.3, Color.CYAN, 4));
        triangleEntity.addChild(rectEntity); // 注意，addChild一定要在transform操作之前设置好！
        positionOrbiting = new PositionOrbiting();
        rectEntity.addComponent(positionOrbiting);
        rectEntity.setName("rectEntity");
        rectEntity.getTransform().setPosition(100, 100);
        positionOrbiting.updateLastKeyPos();
        addGameObject(rectEntity);

        Player player = new Player();
        player.setName("player");
//        player.addComponent(new SpriteRenderer(new Polygon(
//                new Point2D[] {
//                        new Point2D(-20, 10), new Point2D(0, 30), new Point2D(20, 10),
//                        new Point2D(10, -20), new Point2D(-10, -20),
//                }
//        ){{
//            drawParams = DrawParams.BLUE_FILL;
//        }}));
        player.addComponent(new LightSource(100, 200, 0.9, Color.WHITE, 6));
        player.getTransform().setPosition(0, 0);
        addGameObject(player);
        InputManager.registerListener(player);

        Camera zoomableCamera = new Camera("Zoomable Camera");
        zoomableCamera.addComponent(new CameraWheelZoom());
        zoomableCamera.addComponent(new CameraFollow() {{
            setFollowing(player);
        }});
        CameraManager.addCamera(zoomableCamera);
        CameraManager.setMainCamera("Zoomable Camera");
    }

    @Override
    protected void perform(double deltaTime) {
        Point2D pixelsPerSec = new Point2D(20, -10);
        Point2D factualInc = Point2D.scale(pixelsPerSec, deltaTime);
        triangleEntity.getTransform().translate(factualInc.x, factualInc.y);

        double radiansPerSecTri = -Math.PI / 4;
        double factualRotTri = radiansPerSecTri * deltaTime;
        triangleEntity.getTransform().rotate(factualRotTri);

        double radiansPerSecRect = Math.PI;
        double factualRotRect = radiansPerSecRect * deltaTime;
        rectEntity.getTransform().rotate(factualRotRect);

        double rpsRectOrbiting = Math.PI / 2;
        double factualRotOrb = rpsRectOrbiting * deltaTime;
        positionOrbiting.rotate(factualRotOrb);
    }

    @Override
    protected void unloadResources() {

    }

}
