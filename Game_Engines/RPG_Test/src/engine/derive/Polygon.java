package engine.derive;

import engine.base.CameraManager;
import engine.base.Point2D;
import engine.base.Shape;
import javafx.scene.canvas.GraphicsContext;

/**
 * <h1>多边形</h1>
 * <p>适用仿射变换，包括但不限于平移、旋转、缩放、剪切。</p>
 */
public class Polygon extends Shape {
    private Point2D[] vertices;

    public Polygon(Point2D[] vertices) {
        this.vertices = vertices;
    }

    public void setVertices(Point2D[] vertices) {
        this.vertices = vertices;
    }

    /**
     * <h1>创建一个等边三角形</h1>
     * @param c2p 中心到y轴上的顶点的向量的y分量（若为负，则窗口系下向上；反之向下）
     * @return 等边三角形
     */
    public static Polygon regularTriangle(double c2p) {
        Point2D one = new Point2D(0, c2p);
        return new Polygon(new Point2D[] {
                one, Point2D.rotate(one, Math.PI * 2 / 3), Point2D.rotate(one, -Math.PI * 2 / 3)
        });
    }

    /**
     * <h1>创建一个正方形</h1>
     * @param len 边长
     * @return 正方形
     */
    public static Polygon square(double len) {
        double half = len / 2;
        return new Polygon(new Point2D[] {
                new Point2D(half, half), new Point2D(half, -half),
                new Point2D(-half, -half), new Point2D(-half, half)
        });
    }

    /**
     * <h1>创建一个矩形</h1>
     * @param w 宽度
     * @param h 长度
     * @return 矩形
     */
    public static Polygon rectangle(double w, double h) {
        double halfW = w / 2, halfH = h / 2;
        return new Polygon(new Point2D[] {
                new Point2D(halfW, halfH), new Point2D(halfW, -halfH),
                new Point2D(-halfW, -halfH), new Point2D(-halfW, halfH)
        });
    }

//    public static Polygon nRegularPolygon(double n) {
//
//    }

    /**
     * <h1>是否包含世界点</h1>
     * <p><b>仅对凸多边形适用。</b></p>
     */
    public boolean contains(Point2D worldPoint) {
        // 对不成形的错误图形，按不包含处理
        if (vertices.length <= 2) return false;
        // 批量转换为世界点
        Point2D[] worldVertices = new Point2D[vertices.length];
        for (int i = 0; i < worldVertices.length; ++i) {
            worldVertices[i] = transform.getLocalToWorldMatrix().transform(vertices[i]);
        }
        // ---叉乘判断世界点是否在该（凸）多边形内---//
        boolean isIn = true;
        boolean isFirstPositive = Point2D.minus(worldVertices[1], worldVertices[0])
                .cross(Point2D.minus(worldPoint, worldVertices[0])) > 0;
        for (int i = 2; i < worldVertices.length; ++i) {
            if ((Point2D.minus(worldVertices[i], worldVertices[i - 1])
                    .cross(Point2D.minus(worldPoint, worldVertices[i - 1])) > 0) != isFirstPositive) {
                isIn = false;
                break;
            }
        }
        // 最后一条边的判断
        if (isIn) {
            if ((Point2D.minus(worldVertices[0], worldVertices[worldVertices.length - 1])
                    .cross(Point2D.minus(worldPoint, worldVertices[worldVertices.length - 1])) > 0) != isFirstPositive){
                isIn = false;
            }
        }
        return isIn;
    }

    @Override
    public void draw(GraphicsContext gc) {
        if (drawParams.isOpaque) {
            gc.setFill(drawParams.fillColor);
            gc.setStroke(drawParams.strokeColor);
            gc.setLineWidth(drawParams.lineWidth);
            int nPoints = vertices.length;
            Point2D[] showVertices = new Point2D[nPoints];
            for (int i = 0; i < nPoints; i++) {
                Point2D worldPoint = transform.getLocalToWorldMatrix().transform(vertices[i]);
                showVertices[i] = CameraManager.getMainCamera().worldToCamera(worldPoint);
            }
            double[] xPoints = new double[nPoints];
            double[] yPoints = new double[nPoints];
            for (int i = 0; i < nPoints; i++) {
                xPoints[i] = showVertices[i].x;
                yPoints[i] = showVertices[i].y;
            }
            gc.fillPolygon(xPoints, yPoints, nPoints);
            gc.strokePolygon(xPoints, yPoints, nPoints);
        }
    }
}
