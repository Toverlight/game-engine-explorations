package project.content;

import engine.base.*;
import engine.derive.*;
import project.test.ones.Player;

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
        triangleEntity.addChild(rectEntity); // 注意，addChild一定要在transform操作之前设置好！
        positionOrbiting = new PositionOrbiting();
        rectEntity.addComponent(positionOrbiting);
        rectEntity.setName("rectEntity");
        rectEntity.getTransform().setPosition(100, 100);
        positionOrbiting.updateLastKeyPos();
        addGameObject(rectEntity);

        Player player = new Player();
        player.setName("player");
        player.addComponent(new SpriteRenderer(new Polygon(
                new Point2D[] {
                        new Point2D(-20, 10), new Point2D(0, 30), new Point2D(20, 10),
                        new Point2D(10, -20), new Point2D(-10, -20),
                }
        ){{
            drawParams = DrawParams.BLUE_FILL;
        }}));
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
