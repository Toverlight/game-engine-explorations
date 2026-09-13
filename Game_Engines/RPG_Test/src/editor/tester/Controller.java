package editor.tester;

import javafx.fxml.FXML;
import javafx.scene.control.Button;
import javafx.scene.control.Label;

public class Controller {
    @FXML
    private Button myButton;

    @FXML
    private Label myLabel;

    @FXML
    private void onButtonClick() {
        myLabel.setText("You clicked me!");
    }
}
