package xyz.topover.project;

import xyz.topover.core.run.GameRunner;
import xyz.topover.core.scene.GameSceneManager;
import xyz.topover.project.scene.PhysicsScene;
import xyz.topover.project.scene.WelcomeScene;

public class GameLauncher {
    public static void main(String[] args) {
        // TODO 配置
        GameSceneManager.setPakName("xyz.topover.project.scene");

        GameRunner.setInitSceneName(PhysicsScene.class.getSimpleName());
        GameRunner.setTitle("Welcome Scene Test");
        GameRunner.run(args);
    }
}
