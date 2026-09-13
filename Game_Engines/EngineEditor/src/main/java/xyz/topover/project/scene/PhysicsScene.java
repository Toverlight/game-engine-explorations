package xyz.topover.project.scene;

import javafx.scene.paint.Color;
import xyz.topover.core.base.GameObject;
import xyz.topover.core.entity.Physics;
import xyz.topover.core.entity.PhysicsHandler;
import xyz.topover.core.paint.CBoxRectangle;
import xyz.topover.core.paint.SpriteRenderer;
import xyz.topover.core.scene.GameScene;
import xyz.topover.core.view.Camera;
import xyz.topover.core.view.CameraManager;
import xyz.topover.core.view.CameraWheelZoom;

public class PhysicsScene extends GameScene {
    @Override
    protected void loadResources() {

    }

    @Override
    protected void initializeScene() {
        setGlobalIntensity(1);

        GameObject rectA = new GameObject();
        GameObject rectB = new GameObject();
        CBoxRectangle cBoxA = new CBoxRectangle(100, 50) {{
            drawParams.fillColor = Color.YELLOW;
        }};
        CBoxRectangle cBoxB = new CBoxRectangle(150, 120) {{
            drawParams.fillColor = Color.BLUE;
        }};
        rectA.addComponent(new SpriteRenderer(cBoxA));
        rectA.addComponent(new Physics(cBoxA));
        rectB.addComponent(new SpriteRenderer(cBoxB));
        rectB.addComponent(new Physics(cBoxB));
        rectA.getTransform().setPosition(-200, 0);
        rectB.getTransform().setPosition(200, 0);
        Physics physicsA = rectA.getComponent(Physics.class);
        Physics physicsB = rectB.getComponent(Physics.class);
        physicsA.setVx(100).setE(0.5).setMu(0.5).setM(1);
        physicsB.setVx(-50).setE(0.5).setMu(0.5).setM(3);

        addGameObject(rectA);
        addGameObject(rectB);

        Camera zoomableCamera = new Camera("Zoomable Camera");
        zoomableCamera.addComponent(new CameraWheelZoom());
        CameraManager.addCamera(zoomableCamera);
        CameraManager.setMainCamera("Zoomable Camera");
    }

    @Override
    protected void unloadResources() {

    }

    @Override
    protected void perform(double deltaTime) {
        PhysicsHandler.handleCollision();
//        PhysicsHandler.resetCollisionInfo();
    }
}
