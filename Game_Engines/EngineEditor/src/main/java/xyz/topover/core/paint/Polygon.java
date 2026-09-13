package xyz.topover.core.paint;

import xyz.topover.core.run.GameRunner;
import xyz.topover.core.view.CameraManager;
import xyz.topover.core.calc.Point2D;
import javafx.scene.canvas.GraphicsContext;

import java.util.ArrayList;
import java.util.List;
import java.util.function.Consumer;

/**
 * <h1>多边形</h1>
 * <p>适用仿射变换，包括但不限于平移、旋转、缩放、剪切。</p>
 */
public class Polygon extends Shape {
    protected Point2D[] vertices;

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
        return contains(worldPoint, worldVertices);
    }
    private boolean contains(Point2D worldPoint, Point2D[] worldVertices) {
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

    /**
     * <h1>获取多边形上距离给定点最近的点</h1>
     * <p>
     *     <b>注意</b>，不要对少于三个点（不成形）的多边形使用该函数！否则会报错！<br>
     *     该函数不适合用于非凸多边形。
     * </p>
     * @param p 给定点
     */
    public Point2D nearest(Point2D p) {
        // 将顶点转换为世界点
        Point2D[] worldVertices = new Point2D[vertices.length];
        for (int i = 0; i < worldVertices.length; ++i) {
            worldVertices[i] = transform.getLocalToWorldMatrix().transform(vertices[i]);
        }

        if (contains(p, worldVertices)) { // 在多边形内
            return new Point2D(p);
        } else { // 在多边形外
            List<Point2D> candidates = new ArrayList<>(); // 候选点
            // 每条边的正交投影判断
            for (int i = 0; i < worldVertices.length - 1; ++i) {
                Point2D ab = Point2D.minus(worldVertices[i + 1], worldVertices[i]);
                Point2D ap = Point2D.minus(p, worldVertices[i]);
                double t = Point2D.product(ap, ab) / Point2D.product(ab, ab);
                if (t > 0 && t < 1) candidates.add(Point2D.plus(worldVertices[i], Point2D.scale(ab, t)));
                else {
                    if (candidates.isEmpty() || !candidates.get(candidates.size() - 1).equals(worldVertices[i])) {
                        candidates.add(worldVertices[i]);
                    }
                    candidates.add(worldVertices[i + 1]);
                }
            }
            // 最后一条边
            Point2D ab = Point2D.minus(worldVertices[0], worldVertices[worldVertices.length - 1]);
            Point2D ap = Point2D.minus(p, worldVertices[worldVertices.length - 1]);
            double t = Point2D.product(ap, ab) / Point2D.product(ab, ab);
            if (t > 0 && t < 1) candidates.add(Point2D.plus(worldVertices[worldVertices.length - 1], Point2D.scale(ab, t)));
            else {
                if (candidates.isEmpty() || !candidates.get(candidates.size() - 1).equals(worldVertices[worldVertices.length - 1])) {
                    candidates.add(worldVertices[worldVertices.length - 1]);
                }
                candidates.add(worldVertices[0]);
            }

            Point2D nearestPoint = candidates.get(0);
            double minDis = nearestPoint.dis(p);
            for (Point2D candidate : candidates) {
                double curDis = candidate.dis(p);
                if (curDis < minDis) {
                    minDis = curDis;
                    nearestPoint = candidate;
                }
            }
            return nearestPoint;
        }
    }

    /**
     * <h1>获取多边形上距离给定点最远的点</h1>
     * @param p 给定点
     */
    public Point2D farthest(Point2D p) {
        Point2D farthestPoint = transform.getLocalToWorldMatrix().transform(vertices[0]);
        double maxDis = farthestPoint.dis(p);
        for (int i = 1; i < vertices.length; ++i) {
            Point2D on = transform.getLocalToWorldMatrix().transform(vertices[i]);
            double curDis = on.dis(p);
            if (curDis > maxDis) {
                maxDis = curDis;
                farthestPoint = on;
            }
        }
        return farthestPoint;
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

    /**
     * <h1>在多边形的裁剪区域内的渲染等行为</h1>
     * @param runnable 用户自定义动作
     */
    public void funcInClip(Runnable runnable) {
        GraphicsContext gc = GameRunner.getGc();
        // 保存当前状态
        gc.save();

        // 设置多边形裁剪区域
        gc.beginPath();
        Point2D worldFirst = transform.getLocalToWorldMatrix().transform(vertices[0]);
        Point2D firstVertex = CameraManager.getMainCamera().worldToCamera(worldFirst);
        gc.moveTo(firstVertex.x, firstVertex.y);
        for (int i = 1; i < vertices.length; i++) {
            Point2D worldVertex = transform.getLocalToWorldMatrix().transform(vertices[i]);
            Point2D vertex = CameraManager.getMainCamera().worldToCamera(worldVertex);
            gc.lineTo(vertex.x, vertex.y);
        }
        gc.closePath();
        gc.clip();

        // 绘制自定义内容
        runnable.run();

        // 恢复状态
        gc.restore();
    }
}
