package engine.derive;

import engine.base.CameraManager;
import engine.base.Point2D;
import engine.base.Shape;
import engine.base.calc.AffineMatrix;
import javafx.scene.canvas.GraphicsContext;

/**
 * <h1>椭圆</h1>
 * <p>
 *     简单实现的椭圆类，仅具有<b>平移和不完全的缩放能力</b>。
 *     注：对该椭圆类<b>不适用旋转、剪切变换</b>（后续可能会加入）<br>
 *     若用摄像机渲染，该椭圆类可能更适合挂在前面的、不多变的UI组件。<br>
 *     要用具有各种变换能力的曲线图形（包括但不限于绘制变换的椭圆），请考虑使用其他Path、贝塞尔曲线之类实现的图形类。
 * </p>
 */
public class Oval extends Shape {
    private final Point2D size;

    public Oval(double w, double h) {
        size = new Point2D(w, h);
    }

    @Override
    public void draw(GraphicsContext gc) {
        if (drawParams.isOpaque) {
            gc.setFill(drawParams.fillColor);
            gc.setStroke(drawParams.strokeColor);
            gc.setLineWidth(drawParams.lineWidth);
            AffineMatrix localToWorldMatrix = transform.getLocalToWorldMatrix();
            Point2D worldPointLU = localToWorldMatrix.transform(
                    new Point2D(- size.x / 2, size.y / 2)
            );
            Point2D showVertexLU = CameraManager.getMainCamera().worldToCamera(worldPointLU);
            Point2D showSize = Point2D.scale(size, new Point2D(
                    localToWorldMatrix.get(0, 0),
                    localToWorldMatrix.get(1, 1)
            ));
            gc.fillOval(showVertexLU.x, showVertexLU.y, showSize.x, showSize.y);
            gc.strokeOval(showVertexLU.x, showVertexLU.y, showSize.x, showSize.y);
        }
    }
}
