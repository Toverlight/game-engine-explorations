package editor.sculpt;


import engine.base.*;
import engine.base.feature.InputListener;
import engine.derive.Polygon;
import engine.derive.SpriteRenderer;
import javafx.fxml.FXML;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.control.*;
import javafx.scene.input.*;
import javafx.scene.layout.AnchorPane;
import javafx.scene.paint.Color;

import java.util.ArrayList;
import java.util.List;
import java.util.Stack;
import java.util.function.UnaryOperator;

public class Controller {
    private final List<Point2D> shapeVertices = new ArrayList<>();
    private final Stack<Operation> undoStack = new Stack<>();
    private final Stack<Operation> redoStack = new Stack<>();
    private int selectedIndex = -1;
    private int touchedIndex = -1;
    private int lineWidth = 1;
    private final GameObject shapeObject = new GameObject();
    private final Polygon curPolygon = new Polygon(new Point2D[0]);
    private final Camera mainCamera = new Camera("Main Camera");
    private static final double ZOOM_MIN = 0.6;
    private static final double ZOOM_MAX = 5.0;
    private static final double ZOOM_DELTA = 0.4;
    private Point2D dragStartWorldPos;
    private boolean isDragging = false;
    private boolean suppressSpinnerListener = false;
    private static final int POINT_RADIUS = 10;

    @FXML
    private void initialize() {
        mainCamera.setViewport(new Point2D(canvas.getWidth(), canvas.getHeight()));
        mainCamera.setPosition(0, 0);
        CameraManager.addCamera(mainCamera);
        CameraManager.setMainCamera("Main Camera");

        shapeObject.addComponent(new SpriteRenderer(curPolygon));

        strokeColorPicker.setValue(Color.BLACK);

        lineWidthSpinner.setValueFactory(new SpinnerValueFactory.
                IntegerSpinnerValueFactory(0, 100, 1, 1)
        );
        lineWidthSpinner.valueProperty().addListener(((observable, oldValue, newValue) -> {
            if (!suppressSpinnerListener) {
                DrawParams recordedDrawParams = new DrawParams();
                if (newValue == 0) {
                    recordedDrawParams.strokeColor = curPolygon.getDrawParams().strokeColor;
                    curPolygon.getDrawParams().strokeColor = Color.TRANSPARENT;
                } else if (oldValue == 0) {
                    curPolygon.getDrawParams().strokeColor = strokeColorPicker.getValue();
                }
                recordedDrawParams.lineWidth = oldValue;
                lineWidth = newValue;
                undoStack.push(new Operation(OpType.DrawParam_LineWidth, recordedDrawParams));
                redoStack.clear();
                flashDrawParamShowing();
            }
        }));

        UnaryOperator<TextFormatter.Change> filter = change -> {
            String newText = change.getControlNewText();
            if (newText.matches("-?\\d*")) { // 只允许符号和整数
                return change;
            }
            return null;
        };
        xTf.setTextFormatter(new TextFormatter<>(filter));
        yTf.setTextFormatter(new TextFormatter<>(filter));

    }

    @FXML
    private AnchorPane anchorPane;
    @FXML
    private MenuItem fileNew;
    @FXML
    private Canvas canvas;
    @FXML
    private ColorPicker strokeColorPicker;
    @FXML
    private ColorPicker fillColorPicker;
    @FXML
    private Spinner<Integer> lineWidthSpinner;
    @FXML
    private TextField xTf;
    @FXML
    private TextField yTf;
    @FXML
    private Button removeVertexButton;
    @FXML
    private Label mouseCorLabel;
    @FXML
    private Label mouseWorldCorLabel;

    private void flashShapeVerticesShowing() {
        curPolygon.setVertices(shapeVertices.toArray(new Point2D[0]));
        GraphicsContext gc = canvas.getGraphicsContext2D();
        gc.clearRect(0, 0, canvas.getWidth(), canvas.getHeight());
        curPolygon.draw(gc);
        showSelected(gc);
        showTouched(gc);
    }
    private void flashDrawParamShowing() {
        GraphicsContext gc = canvas.getGraphicsContext2D();
        gc.clearRect(0, 0, canvas.getWidth(), canvas.getHeight());
        curPolygon.getDrawParams().lineWidth = (int) Math.round(
                lineWidth * mainCamera.getZoom().x);
        curPolygon.draw(gc);
        showSelected(gc);
        showTouched(gc);
    }
    private void flashTranslationShowing() {
        GraphicsContext gc = canvas.getGraphicsContext2D();
        gc.clearRect(0, 0, canvas.getWidth(), canvas.getHeight());
        curPolygon.draw(gc);
        showSelected(gc);
        showTouched(gc);
    }
    private void showSelected(GraphicsContext gc) {
        if (selectedIndex >= 0 && selectedIndex < shapeVertices.size()) {
            xTf.setDisable(false);
            yTf.setDisable(false);
            xTf.setText(Integer.toString((int)shapeVertices.get(selectedIndex).x));
            yTf.setText(Integer.toString((int)shapeVertices.get(selectedIndex).y));
            Point2D center = mainCamera.worldToCamera(shapeVertices.get(selectedIndex));
            double radius = 20;
            gc.setFill(Color.rgb(175, 122, 253, 0.7));
            gc.fillOval(center.x - radius, center.y - radius, radius * 2, radius * 2);
            removeVertexButton.setDisable(false);
        } else {
            xTf.setDisable(true);
            yTf.setDisable(true);
            xTf.clear();
            yTf.clear();
            removeVertexButton.setDisable(true);
        }
    }
    private void showTouched(GraphicsContext gc) {
        if (touchedIndex >= 0 && touchedIndex < shapeVertices.size()) {
            Point2D center = mainCamera.worldToCamera(shapeVertices.get(touchedIndex));
            double radius = 20;
            gc.setFill(Color.rgb(82, 220, 136, 0.7));
            gc.fillOval(center.x - radius, center.y - radius, radius * 2, radius * 2);
        }
    }

    @FXML
    private void onFileNew() {

    }

    @FXML
    private void onAnchorPressed(KeyEvent keyEvent) {
        if (keyEvent.isControlDown() && keyEvent.getCode() == KeyCode.Z) { // 撤销
            if (!undoStack.isEmpty()) {
                Operation operation = undoStack.pop();
                handleOperation(operation);
                redoStack.push(operation);
            }
        }
        else if (keyEvent.isControlDown() && keyEvent.getCode() == KeyCode.Y) { // 重做
            if (!redoStack.isEmpty()) {
                Operation operation = redoStack.pop();
                handleOperation(operation);
                undoStack.push(operation);
            }
        }
    }

    @FXML
    private void onCanvasClicked(MouseEvent mouseEvent) {
        MouseButton mouseButton = mouseEvent.getButton();
        if (mouseButton == MouseButton.PRIMARY) {
            if (touchedIndex == -1) { // 添加新顶点
                Point2D mouseWorldPos = mainCamera.cameraToWorld(new Point2D(mouseEvent.getX(), mouseEvent.getY()));
                shapeVertices.add(mouseWorldPos);
                selectedIndex = shapeVertices.size() - 1;
                undoStack.push(new Operation(OpType.Add, selectedIndex, new Point2D(mouseWorldPos)));
                redoStack.clear();
            } else { // 选中顶点
                selectedIndex = touchedIndex;
            }
            flashShapeVerticesShowing();
        } else if (mouseButton == MouseButton.SECONDARY) { // 右键执行某些取消操作
            if (selectedIndex != -1) { // 取消选中顶点
                selectedIndex = -1;
                flashShapeVerticesShowing();
            }
        }
    }

    @FXML
    private void onCanvasReleased(MouseEvent mouseEvent) {
//        if (isDragging) {
//            isDragging = false;
//        }
    }

    @FXML
    private void onCanvasMoved(MouseEvent mouseEvent) {
        Point2D mouseCameraPos = new Point2D(mouseEvent.getX(), mouseEvent.getY());
        mouseCorLabel.setText("r x:" + (int)Math.round(mouseCameraPos.x) + ",y:" + (int)Math.round(mouseCameraPos.y));
        Point2D mouseWorldPos = mainCamera.cameraToWorld(mouseCameraPos);
        mouseWorldCorLabel.setText("a x:" + (int)Math.round(mouseWorldPos.x) + ",y:" + (int)Math.round(mouseWorldPos.y));
        detectTouching(mouseCameraPos);
        flashTranslationShowing();
    }
    private void detectTouching(Point2D mouseCameraPos) {
        boolean detected = false;
        for (int i = 0; i < shapeVertices.size(); ++i) {
            if (mouseCameraPos.dis(mainCamera.worldToCamera(shapeVertices.get(i))) < POINT_RADIUS) {
                touchedIndex = i;
                detected = true;
                break;
            }
        }
        if (!detected) touchedIndex = -1;
    }

    @FXML
    private void onCanvasExited(MouseEvent mouseEvent) {
        mouseCorLabel.setText("relative");
        mouseWorldCorLabel.setText("absolute");
    }


    @FXML
    private void onCanvasDragged(MouseEvent mouseEvent) {
        isDragging = true;
        MouseButton mouseButton = mouseEvent.getButton();
        if (mouseButton == MouseButton.SECONDARY) {
            Point2D curWorldPos = mainCamera.cameraToWorld(new Point2D(mouseEvent.getX(), mouseEvent.getY()));
            if (selectedIndex < 0) { // 选中canvas时
                Point2D displacementInvert = Point2D.minus(dragStartWorldPos, curWorldPos);
                mainCamera.translate(displacementInvert.x, displacementInvert.y);
            } else { // 选中顶点时
                // TODO drag vertex

            }
        }
        flashTranslationShowing();
    }

    @FXML
    private void onCanvasPressed(MouseEvent mouseEvent) {
        if (mouseEvent.isSecondaryButtonDown()) {
            dragStartWorldPos = mainCamera.cameraToWorld(new Point2D(mouseEvent.getX(), mouseEvent.getY()));
        }
    }

    @FXML
    private void onCanvasScroll(ScrollEvent scrollEvent) {
        Point2D oriZoom = mainCamera.getZoom();
        if (scrollEvent.getDeltaY() > 0) { // 上滚放大
            if (oriZoom.x + ZOOM_DELTA >= ZOOM_MAX || oriZoom.y + ZOOM_DELTA >= ZOOM_MAX) {
                mainCamera.setZoom(ZOOM_MAX);
            } else {
                mainCamera.zoom(ZOOM_DELTA);
            }
            curPolygon.getDrawParams().lineWidth = (int) Math.round(
                    lineWidth * mainCamera.getZoom().x);
        } else if (scrollEvent.getDeltaY() < 0) { // 下滚缩小
            if (oriZoom.x - ZOOM_DELTA <= ZOOM_MIN || oriZoom.y - ZOOM_DELTA <= ZOOM_MIN) {
                mainCamera.setZoom(ZOOM_MIN);
            } else {
                mainCamera.zoom(-ZOOM_DELTA);
            }
            curPolygon.getDrawParams().lineWidth = (int) Math.round(
                    lineWidth * mainCamera.getZoom().x);
        }
        detectTouching(new Point2D(scrollEvent.getX(), scrollEvent.getY()));
        flashShapeVerticesShowing();
    }

    private void handleOperation(Operation operation) {
        if (operation.opType == OpType.Pop) { // 上一步是删除
            shapeVertices.add(operation.selectedIndex, new Point2D(operation.curPoint));
            selectedIndex = operation.selectedIndex;
            flashShapeVerticesShowing();
        } else if (operation.opType == OpType.Add) { // 上一步是添加
            operation.curPoint = new Point2D(shapeVertices.remove(operation.selectedIndex));
            selectedIndex = operation.selectedIndex - 1;
            flashShapeVerticesShowing();
        } else if (operation.opType == OpType.Modify) { // 上一步是修改点坐标
            Point2D tempVertex = new Point2D(shapeVertices.get(selectedIndex));
            shapeVertices.set(selectedIndex, new Point2D(operation.curPoint));
            operation.curPoint = tempVertex;
            flashShapeVerticesShowing();
        } else if (operation.opType == OpType.DrawParam_Stroke) { // 上一步是修改绘制参数StrokeColor
            Color stroke = curPolygon.getDrawParams().strokeColor;
            curPolygon.getDrawParams().strokeColor = operation.curDrawParams.strokeColor;
            operation.curDrawParams.strokeColor = stroke;
            flashDrawParamShowing();
        } else if (operation.opType == OpType.DrawParam_Fill) { // 上一步是修改绘制参数FillColor
            Color fill = curPolygon.getDrawParams().fillColor;
            curPolygon.getDrawParams().fillColor = operation.curDrawParams.fillColor;
            operation.curDrawParams.fillColor = fill;
            flashDrawParamShowing();
        } else if (operation.opType == OpType.DrawParam_LineWidth) { // 上一步是修改绘制参数LineWidth
            int lw = lineWidth;
            lineWidth = operation.curDrawParams.lineWidth;
            operation.curDrawParams.lineWidth = lw;
            if (lineWidth == 0) {
                curPolygon.getDrawParams().strokeColor = Color.TRANSPARENT;
            } else if (operation.curDrawParams.lineWidth == 0) {
                curPolygon.getDrawParams().strokeColor = operation.curDrawParams.strokeColor;
            }
            suppressSpinnerListener = true;
            if (lineWidth < operation.curDrawParams.lineWidth) {
                lineWidthSpinner.decrement();
            } else if (lineWidth > operation.curDrawParams.lineWidth) {
                lineWidthSpinner.increment();
            }
            suppressSpinnerListener = false;
            flashDrawParamShowing();
        }

        if (operation.opType == OpType.Pop) operation.opType = OpType.Add;
        if (operation.opType == OpType.Add) operation.opType = OpType.Pop;
    }

    @FXML
    private void onStrokePick() {
        DrawParams newDrawParams = curPolygon.getDrawParams();
        DrawParams oldDrawParams = new DrawParams(newDrawParams.strokeColor, newDrawParams.fillColor,
                newDrawParams.lineWidth, true);
        newDrawParams.strokeColor = strokeColorPicker.getValue();
        undoStack.push(new Operation(OpType.DrawParam_Stroke, oldDrawParams));
        redoStack.clear();
        flashDrawParamShowing();
    }
    @FXML
    private void onFillPick() {
        DrawParams newDrawParams = curPolygon.getDrawParams();
        DrawParams oldDrawParams = new DrawParams(newDrawParams.strokeColor, newDrawParams.fillColor,
                newDrawParams.lineWidth, true);
        newDrawParams.fillColor = fillColorPicker.getValue();
        undoStack.push(new Operation(OpType.DrawParam_Fill, oldDrawParams));
        redoStack.clear();
        flashDrawParamShowing();
    }

    @FXML
    private void onRemoveVertex() {
        if (selectedIndex >= 0 && selectedIndex < shapeVertices.size()) {
            undoStack.push(new Operation(OpType.Pop, selectedIndex, shapeVertices.remove(selectedIndex)));
            redoStack.clear();
            selectedIndex = -1;
            flashShapeVerticesShowing();
        }
    }


}
enum OpType {
    Add,
    Pop,
    Modify,
    DrawParam_Stroke,
    DrawParam_Fill,
    DrawParam_LineWidth
}
class Operation {
    public OpType opType;
    public Point2D curPoint;
    public int selectedIndex;
    public DrawParams curDrawParams;

    public Operation(OpType opType, DrawParams drawParams) {
        this.opType = opType;
        this.curDrawParams = drawParams;
    }
    public Operation(OpType opType, int selectedIndex, Point2D curPoint) {
        this.opType = opType;
        this.selectedIndex = selectedIndex;
        this.curPoint = curPoint;
    }
}