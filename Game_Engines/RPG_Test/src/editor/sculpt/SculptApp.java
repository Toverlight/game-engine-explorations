package editor.sculpt;

import engine.base.InputManager;
import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Scene;
import javafx.scene.layout.AnchorPane;
import javafx.stage.Stage;

/**
 * <h1>塑形编辑器</h1>
 * <p>
 *     可视化编辑形状节点和绘制参数。
 * </p>
 */
public class SculptApp extends Application {

    @Override
    public void start(Stage primaryStage) throws Exception {
        AnchorPane root = FXMLLoader.load(getClass().getResource("/editor/SculptView.fxml"));
        Scene scene = new Scene(root);
        InputManager.bindListeners(scene);
        primaryStage.setTitle("塑形编辑器");
        primaryStage.setScene(scene);
        primaryStage.show();
    }

    public static void main(String[] args) {
        launch(args);
    }
}
