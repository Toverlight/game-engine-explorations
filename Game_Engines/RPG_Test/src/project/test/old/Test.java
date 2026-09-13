package project.test.old;

import javafx.application.Application;
import javafx.application.Platform;
import javafx.scene.Scene;
import javafx.scene.control.Alert;
import javafx.scene.control.Button;
import javafx.scene.control.ButtonType;
import javafx.scene.control.Label;
import javafx.scene.image.Image;
import javafx.scene.layout.BorderPane;
import javafx.stage.Modality;
import javafx.stage.Stage;
import javafx.stage.StageStyle;

import java.util.Optional;

public class Test extends Application{
    public static void main(String[] args) {
        Application.launch(args);
        System.out.println("launch() over.");
    }


    @Override
    public void start(Stage primaryStage) {
        System.out.println("start()...");

        Button button = new Button("Click me");
        BorderPane pane = new BorderPane(button);

        button.setOnAction(event -> {
            Stage stage = new Stage();
            stage.setHeight(200);
            stage.setWidth(300);
            stage.initModality(Modality.NONE);
            stage.show();
        });

        Platform.setImplicitExit(false);

        primaryStage.setOnCloseRequest(event -> {
            event.consume();
            Alert alert = new Alert(Alert.AlertType.CONFIRMATION);
            alert.setTitle("退出程序");
            alert.setHeaderText(null);
            alert.setContentText("您是否要退出程序？");

            Optional<ButtonType> result = alert.showAndWait();
            if (result.get() == ButtonType.OK) {
                Platform.exit();
            }
        });

        Scene scene = new Scene(pane, 800, 600);
        primaryStage.setScene(scene);
        primaryStage.setTitle("A window");
        primaryStage.getIcons().add(new Image("image/icon.png"));
//      primaryStage.setResizable(false);
        primaryStage.initStyle(StageStyle.UTILITY);
        primaryStage.show();
    }

    @Override
    public void init() throws Exception {
        super.init();
        System.out.println("init()...");
    }

    @Override
    public void stop() throws Exception {
        super.stop();
        System.out.println("stop()...");
    }
}
