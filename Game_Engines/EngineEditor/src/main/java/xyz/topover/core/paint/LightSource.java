package xyz.topover.core.paint;

import javafx.scene.canvas.GraphicsContext;
import javafx.scene.paint.Color;
import javafx.scene.paint.CycleMethod;
import javafx.scene.paint.RadialGradient;
import javafx.scene.paint.Stop;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import xyz.topover.core.base.GameComponent;
import xyz.topover.core.calc.Point2D;
import xyz.topover.core.run.GameRunner;
import xyz.topover.core.view.CameraManager;

/**
 * <h1>简单光源组件</h1>
 * <p>
 *     层级默认为5.
 * </p>
 */
public class LightSource extends GameComponent implements Drawable {
    private double sr; // 光源半径
    private double radius; // 最大光照半径
    private double intensity; // 光照强度
    private Color color; // 光源颜色
    private int layer = 5; // 层级
    private static final Logger logger = LoggerFactory.getLogger(LightSource.class);

    public LightSource(double radius, double intensity) {
        sr = 0.0;
        this.radius = radius;
        setIntensity(intensity);
        color = Color.WHITE;
    }
    public LightSource(double radius, double intensity, Color color) {
        sr = 0.0;
        this.radius = radius;
        setIntensity(intensity);
        this.color = color;
    }
    public LightSource(double sr, double radius, double intensity, Color color, int layer) {
        this.radius = radius;
        setSr(sr);
        setIntensity(intensity);
        this.color = color;
        this.layer = layer;
    }
    /**
     * <h1>设置光源半径</h1>
     * <p>
     *     数值越大光源越大。合法范围0.0-1.0，若超出范围，自动处理成0.0或1.0。
     * </p>
     **/
    public void setSr(double srcRadius) {
        sr = Math.max(0, Math.min(1, srcRadius / radius));
    }

    /**
     * <h1>设置光照强度</h1>
     * <p>
     *     数值越大光源越亮。合法范围0.0-1.0，若超出范围，自动处理成0.0或1.0。
     * </p>
     * @param intensity 光照强度
     */
    public void setIntensity(double intensity) {
        this.intensity = Math.max(0, Math.min(1, intensity));
    }

    public double getIntensity() {
        return intensity;
    }

    public void setRadius(double radius) {
        this.radius = radius;
    }

    public double getRadius() {
        return radius;
    }

    public Color getColor() {
        return color;
    }

    public void setColor(Color color) {
        this.color = color;
    }

    /**
     * <h1>获取光源位置</h1>
     */
    public Point2D getSrcPosition() {
        return gameObject.getTransform().getAbsPosition();
    }


    @Override
    public void start() {
        if (gameObject == null) {
            logger.error("Failed to start LightSource(id {}) because of NO parent gameObject ref provided! And isActive was set false for security.", getId());
            setActive(false);
        } else {
            setActive(true);
        }
    }

    @Override
    public void draw(GraphicsContext gc) {
        Point2D srcCameraPoint = CameraManager.getMainCamera().worldToCamera(gameObject.getTransform().getAbsPosition());
        Point2D zoom = CameraManager.getMainCamera().getZoom();
        Point2D newRadius = Point2D.scale(new Point2D(radius, radius), zoom);
        double srPortion = sr * radius;
        RadialGradient gradient = new RadialGradient(
                0, 0,
                srcCameraPoint.x, srcCameraPoint.y,
                Math.min(newRadius.x, newRadius.y),
                false,
                CycleMethod.NO_CYCLE,
                new Stop(0, color.deriveColor(0, 1, 1, intensity)),
                new Stop(srPortion, color.deriveColor(0, 1, 1, intensity)),
                new Stop(1, Color.TRANSPARENT)
        );
        gc.setFill(gradient);
        gc.fillOval(gradient.getCenterX() - newRadius.x, gradient.getCenterY() - newRadius.y,
                newRadius.x * 2, newRadius.y * 2);
    }

    @Override
    public int getLayer() {
        return layer;
    }

    public void setLayer(int layer) {
        this.layer = layer;
    }
}
