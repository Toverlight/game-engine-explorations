package engine.base;

import engine.base.feature.Drawable;
import javafx.scene.canvas.GraphicsContext;

/**
 * <h1>形状</h1>
 * <p>中心点为原点。通过仿射变换得到实际坐标。</p>
 * <p><b>transform</b> - 存储关联变换。<br>
 *     <b>drawParams</b> - 绘制参数。<br></p>
 */
public abstract class Shape implements Drawable {
    protected Transform transform; // 关联的变换
    protected DrawParams drawParams; // 绘制参数

    public Shape() {
        drawParams = new DrawParams();
    }

    public void setTransform(Transform transform) {
        this.transform = transform;
    }

    public DrawParams getDrawParams() {
        return drawParams;
    }

    public abstract void draw(GraphicsContext gc);
}
